#include "magicroitool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolygon.h"

#include <qmath.h>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

static const double MagicSize = 3.0;
static const double MagicFactor = 1.0;

MagicROITool::MagicROITool(QViewer *viewer, QObject *parent)
: ROITool(viewer, parent)
{
    m_magicSize = MagicSize;
    m_magicFactor = MagicFactor;
    m_lowerLevel = 0.0;
    m_upperLevel = 0.0;
    m_toolName = "MagicROITool";

    m_roiPolygon = NULL;

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

MagicROITool::~MagicROITool()
{
    deleteTemporalRepresentation();
}

void MagicROITool::deleteTemporalRepresentation()
{
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }
}

void MagicROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = NULL;
}

void MagicROITool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            startRegion();
            break;
        case vtkCommand::LeftButtonReleaseEvent:
            endRegion();
            break;
        case vtkCommand::MouseMoveEvent:
            modifyRegionByFactor();
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27) // ESC
            {
                deleteTemporalRepresentation();
            }
            break;
    }
}

void MagicROITool::assignBounds(int &minX, int &minY, int &maxX, int &maxY)
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    minX = ext[xIndex * 2];
    maxX = ext[(xIndex * 2) + 1];
    minY = ext[yIndex * 2];
    maxY = ext[(yIndex * 2) + 1];

}

double MagicROITool::getVoxelValue(int x, int y, int z)
{
    double value = 0;
    switch(m_2DViewer->getView())
    {
        case Q2DViewer::Axial:
            value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(x, y, z, 0);
            break;
        case Q2DViewer::Sagital:
            value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(z, x, y, 0);
            break;
        case Q2DViewer::Coronal:
            value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(x, z, y, 0);
            break;
        default:
        DEBUG_LOG("Bad parameter");
    }
    return value;

}

void MagicROITool::startRegion()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getCurrentCursorImageCoordinate(m_pickedPosition))
        {
            m_magicFactor = MagicFactor;
            m_magicSize = MagicSize;
            m_roiPolygon = new DrawerPolygon;
            m_roiPolygon->increaseReferenceCount();
            m_2DViewer->getDrawer()->draw(m_roiPolygon);

            this->generateRegion();
        }
    }
}

void MagicROITool::endRegion()
{
    if (m_roiPolygon)
    {
        this->generateRegion();
        this->printData();
        // Col·loquem el dibuix al lloc corresponent
        m_2DViewer->getDrawer()->erasePrimitive(m_roiPolygon);
        m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        m_roiPolygon->decreaseReferenceCount();
        m_roiPolygon = NULL;
    }
}

void MagicROITool::modifyRegionByFactor()
{
    if (m_roiPolygon)
    {
        double displacementY = 0.05 * (m_viewer->getLastEventPositionY() - m_viewer->getEventPositionY());
        if (m_magicFactor - displacementY > 0.0)
        {
            m_magicFactor -= displacementY;
            this->generateRegion();
        }
    }
}

void MagicROITool::generateRegion()
{
    this->computeLevelRange();

    // Posem a true els punts on la imatge està dins els llindard i connectat amb la llavor (region growing)
    this->computeRegionMask();

    // Trobem els punts frontera i creem el polígon
    this->computePolygon();

    m_2DViewer->render();
}

void MagicROITool::computeLevelRange()
{
    int index[3];
    m_2DViewer->getInput()->getPixelData()->computeCoordinateIndex(m_pickedPosition, index);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    // Calculem la desviació estàndard dins la finestra que ens marca la magic size
    double standardDeviation = getStandardDeviation(index[xIndex], index[yIndex], index[zIndex]);
    
    // Calculem els llindars com el valor en el pixel +/- la desviació estàndard * magic factor
    QVector<double> voxelValue;
    m_2DViewer->getInput()->getVoxelValue(m_pickedPosition, voxelValue);
    m_lowerLevel = voxelValue.at(0) - m_magicFactor * standardDeviation;
    m_upperLevel = voxelValue.at(0) + m_magicFactor * standardDeviation;
}

void MagicROITool::computeRegionMask()
{
    // Busquem el voxel inicial
    int index[3];
    m_2DViewer->getInput()->getPixelData()->computeCoordinateIndex(m_pickedPosition, index);
    int minX, minY;
    int maxX, maxY;
    int x, y, z;

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    x = index[xIndex];
    y = index[yIndex];
    z = index[zIndex];

    this->assignBounds(minX, minY, maxX, maxY);

    // Creem la màscara
    if (minX == 0 && minY == 0)
    {
        m_mask = QVector<bool>((maxX + 1) * (maxY + 1), false);
    }
    else
    {
        DEBUG_LOG("ERROR: extension no comença a 0");
    }
    QVector<double> voxelValue;
    
    m_2DViewer->getInput()->getVoxelValue(m_pickedPosition, voxelValue);
    double value = voxelValue.at(0);
    if ((value >= m_lowerLevel) && (value <= m_upperLevel))
    {
        m_mask[y * maxX + x] = true;
    }
    else
    {
        DEBUG_LOG("Ha petat i sortim");
        return;
    }

    // Comencem el Region Growing
    QVector<int> movements;
    // First movement \TODO Codi duplicat amb main loop
    int i = 0;
    bool found = false;
    while (i < 4 && !found)
    {
        this->doMovement(x, y, i);
        value = this->getVoxelValue(x, y, z);

        if ((value >= m_lowerLevel) && (value <= m_upperLevel))
        {
            m_mask[y * maxX + x] = true;
            found = true;
            movements.push_back(i);
        }
        if (!found)
        {
            this->undoMovement(x, y, i);
        }
        ++i;
    }

    // Main loop
    i = 0;
    while (movements.size() > 0)
    {
        found = false;
        while (i < 4 && !found)
        {
            this->doMovement(x, y, i);
            if ((x > minX) && (x < maxX) && (y > minY) && (y < maxY))
            {
                value = this->getVoxelValue(x, y, z);

                if ((value >= m_lowerLevel) && (value <= m_upperLevel) && (!m_mask[y * maxX + x]))
                {
                    m_mask[y * maxX + x] = true;
                    found = true;
                    movements.push_back(i);
                    i = 0;
                }
            }
            if (!found)
            {
                this->undoMovement(x, y, i);
                ++i;
            }
        }
        if (!found)
        {
            this->undoMovement(x, y, movements.back());
            i = movements.back();
            ++i;
            movements.pop_back();
        }
    }
}

void MagicROITool::doMovement(int &x, int &y, int movement)
{
    switch (movement)
    {
        case MoveRight:
            x++;
            break;
        case MoveLeft:
            x--;
            break;
        case MoveUp:
            y++;
            break;
        case MoveDown:
            y--;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicROITool::undoMovement(int &x, int &y, int movement)
{
    switch (movement)
    {
        case MoveRight:
            x--;
            break;
        case MoveLeft:
            x++;
            break;
        case MoveUp:
            y--;
            break;
        case MoveDown:
            y++;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicROITool::computePolygon()
{    
    int minX;
    int minY;
    int maxX;
    int maxY;
    
    this->assignBounds(minX, minY, maxX, maxY);

    int i = minX;
    int j;

    // Busquem el primer punt
    bool found = false;
    while ((i <= maxX) && !found)
    {
        j = minY;
        while ((j <= maxY) && !found)
        {
            if (m_mask[j * maxX + i])
            {
                found = true;
            }
            ++j;
        }
        ++i;
    }

    int x;
    int y;
    int z;

    // L'índex és -1 pq els hem incrementat una vegada més    
    x = i - 1;
    y = j - 1;
    z = m_2DViewer->getCurrentSlice();
    m_roiPolygon->removeVertices();
    
    this->addPoint(7, x, y, z);
    this->addPoint(1, x, y, z);
    
    int nextX;
    int nextY;
    int nextZ;
    nextZ = z;

    int direction = 0;

    bool loop = false;
    bool next = false;
    while (!loop)
    {
        this->getNextIndex(direction, x, y, nextX, nextY);
        next = m_mask[nextY * maxX + nextX];
        while (!next && !loop)
        {
            if (((direction % 2) != 0) && !next)
            {
                this->addPoint(direction, x, y, z);
                loop = this->isLoopReached();
            }
            direction = this->getNextDirection(direction);
            this->getNextIndex(direction, x, y, nextX, nextY);
            next = m_mask[nextY * maxX + nextX];
        }
        x = nextX;
        y = nextY;
        direction = this->getInverseDirection(direction);
        direction = this->getNextDirection(direction);
    }

    m_roiPolygon->update();
    m_hasToComputeStatisticsData = true;
}

void MagicROITool::getNextIndex(int direction, int x, int y, int &nextX, int &nextY)
{
    switch (direction)
    {
        case LeftDown:
            nextX = x - 1;
            nextY = y - 1;
            break;
        case Down:
            nextX = x;
            nextY = y - 1;
            break;
        case RightDown:
            nextX = x + 1;
            nextY = y - 1;
            break;
        case Right:
            nextX = x + 1;
            nextY = y;
            break;
        case RightUp:
            nextX = x + 1;
            nextY = y + 1;
            break;
        case Up:
            nextX = x;
            nextY = y + 1;
            break;
        case LeftUp:
            nextX = x - 1;
            nextY = y + 1;
            break;
        case Left:
            nextX = x - 1;
            nextY = y;
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }
}

int MagicROITool::getNextDirection(int direction)
{
    int nextDirection = direction + 1;
    return (nextDirection == 8)? 0: nextDirection;
}

int MagicROITool::getInverseDirection(int direction)
{
    return (direction + 4) % 8;
}

void MagicROITool::addPoint(int direction, int x, int y, double z)
{    
    double origin[3];
    double spacing[3];
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
 
    double point[3];
    switch (direction)
    {
        case Down:
            point[xIndex] = x * spacing[xIndex] + origin[xIndex];
            point[yIndex] = (y - 0.5) * spacing[yIndex] + origin[yIndex];
            break;
        case Right:
            point[xIndex] = (x + 0.5) * spacing[xIndex] + origin[xIndex];
            point[yIndex] = y * spacing[yIndex] + origin[yIndex];
            break;
        case Up:
            point[xIndex] = x * spacing[xIndex] + origin[xIndex];
            point[yIndex] = (y + 0.5) * spacing[yIndex] + origin[yIndex];
            break;
        case Left:
            point[xIndex] = (x - 0.5) * spacing[xIndex] + origin[xIndex];
            point[yIndex] = y * spacing[yIndex] + origin[yIndex];
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }
    point[zIndex] = z * spacing[zIndex] + origin[zIndex];

    m_roiPolygon->addVertix(point);
}

bool MagicROITool::isLoopReached()
{
    const double *firstVertix = this->m_roiPolygon->getVertix(0);
    const double *lastVertix = this->m_roiPolygon->getVertix(m_roiPolygon->getNumberOfPoints() - 1);
    return ((qAbs(firstVertix[0] - lastVertix[0]) < 0.0001) && (qAbs(firstVertix[1] - lastVertix[1]) < 0.0001));
}

double MagicROITool::getStandardDeviation(int x, int y, int z)
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);

    int minX;
    int minY;
    int maxX;
    int maxY;
    
    this->assignBounds(minX, minY, maxX, maxY);

    minX = qMax(x - m_magicSize, minX);
    maxX = qMin(x + m_magicSize, maxX);
    minY = qMax(y - m_magicSize, minY);
    maxY = qMin(y + m_magicSize, maxY);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    int index[3];
    index[zIndex] = z;

    // Calculem la mitjana
    double mean = 0.0;
    double value;

    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            index[xIndex] = i;
            index[yIndex] = j;
            value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(index[0], index[1], index[2], 0);
            mean += value;
        }
    }

    int numberOfSamples = (maxX - minX + 1) * (maxY - minY + 1);
    mean = mean / (double)numberOfSamples;

    // Calculem la desviació estandard
    double deviation = 0.0;
    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            index[xIndex] = i;
            index[yIndex] = j;
            value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(index[0], index[1], index[2], 0);
            deviation += qPow(value - mean, 2);
        }
    }
    deviation = qSqrt(deviation / (double)numberOfSamples);
    return deviation;
}

}
