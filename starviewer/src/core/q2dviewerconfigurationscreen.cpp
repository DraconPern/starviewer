#include "q2dviewerconfigurationscreen.h"

#include "coresettings.h"

namespace udg {

Q2DViewerConfigurationScreen::Q2DViewerConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    initialize();
    createConnections();
}

Q2DViewerConfigurationScreen::~Q2DViewerConfigurationScreen()
{
}

void Q2DViewerConfigurationScreen::initialize()
{
    Settings settings;
    
    m_sliceScrollLoopCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool());
    m_phaseScrollLoopCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool());
    m_referenceLinesMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR).toBool());
    m_referenceLinesCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT).toBool());
    m_automaticSynchronizationMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR).toBool());
    m_automaticSynchronizationCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT).toBool());

    initializeModalitiesWithZoomByDefault();
    initializeMagnifyingGlassToolZoomFactor();
}

void Q2DViewerConfigurationScreen::createConnections()
{
    connect(m_sliceScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updateSliceScrollLoopSetting(bool)));
    connect(m_phaseScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updatePhaseScrollLoopSetting(bool)));
    connect(m_referenceLinesMRCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForMRSetting(bool)));
    connect(m_referenceLinesCTCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForCTSetting(bool)));
    connect(m_automaticSynchronizationMRCheckBox,SIGNAL(toggled(bool)), SLOT(updateAutomaticSynchronizationForMRSetting(bool)));
    connect(m_automaticSynchronizationCTCheckBox,SIGNAL(toggled(bool)), SLOT(updateAutomaticSynchronizationForCTSetting(bool)));

    connect(m_zoomByDefaultModalitiesGroupBox, SIGNAL(checkedModalitiesChanged(QStringList)), SLOT(updateModalitiesWithZoomByDefaultSetting(QStringList)));
    connect(m_1point5XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_2XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_4XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_6XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_8XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
}

void Q2DViewerConfigurationScreen::initializeModalitiesWithZoomByDefault()
{
    m_zoomByDefaultModalitiesGroupBox->enableAllModalitiesCheckBox(false);
    m_zoomByDefaultModalitiesGroupBox->enableOtherModalitiesCheckBox(false);
    m_zoomByDefaultModalitiesGroupBox->setExclusive(false);
    
    Settings settings;
    QStringList modalitiesWithZoomList = settings.getValue(CoreSettings::ModalitiesWithZoomToolByDefault).toString().split(";", QString::SkipEmptyParts);
    m_zoomByDefaultModalitiesGroupBox->checkModalities(modalitiesWithZoomList);
}

void Q2DViewerConfigurationScreen::initializeMagnifyingGlassToolZoomFactor()
{
    Settings settings;

    QString zoomFactor = settings.getValue(CoreSettings::MagnifyingGlassZoomFactor).toString();

    if (zoomFactor == "1.5")
    {
        m_1point5XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "2")
    {
        m_2XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "4")
    {
        m_4XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "6")
    {
        m_6XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "8")
    {
        m_8XZoomFactorRadioButton->setChecked(true);
    }
    else
    {
        // Si no hi ha cap valor v�lid, l'augment ser� 4x per defecte
        m_4XZoomFactorRadioButton->setChecked(true);
    }
}

void Q2DViewerConfigurationScreen::updateSliceScrollLoopSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerSliceScrollLoop, enable);
}

void Q2DViewerConfigurationScreen::updatePhaseScrollLoopSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop, enable);
}

void Q2DViewerConfigurationScreen::updateReferenceLinesForMRSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR, enable);
}

void Q2DViewerConfigurationScreen::updateReferenceLinesForCTSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT, enable);
}

void Q2DViewerConfigurationScreen::updateAutomaticSynchronizationForMRSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR, enable);
}

void Q2DViewerConfigurationScreen::updateAutomaticSynchronizationForCTSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT, enable);
}

void Q2DViewerConfigurationScreen::updateModalitiesWithZoomByDefaultSetting(const QStringList &modalities)
{
    Settings settings;
    QString modalitiesWithZoom = modalities.join(";");
    settings.setValue(CoreSettings::ModalitiesWithZoomToolByDefault, modalitiesWithZoom);
}

void Q2DViewerConfigurationScreen::updateMagnifyingGlassZoomFactorSetting()
{
    QString zoomFactor;

    if (m_1point5XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "1.5";
    }
    else if (m_2XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "2";
    }
    else if (m_4XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "4";
    }
    else if (m_6XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "6";
    }
    else if (m_8XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "8";
    }
    else
    {
        // Si no hi ha cap bot� marcat, l'augment ser� 4x per defecte. Tot i aix�, aix� no hauria de passar mai
        zoomFactor = "4";
    }

    Settings settings;
    settings.setValue(CoreSettings::MagnifyingGlassZoomFactor, zoomFactor);
}
}
