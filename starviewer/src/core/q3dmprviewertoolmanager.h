/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ3DMPRVIEWERTOOLMANAGER_H
#define UDGQ3DMPRVIEWERTOOLMANAGER_H

#include "toolmanager.h"

namespace udg {

/**
Tool Manager per al visor q3dmprviewer

	@author Grup de Gr�fics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q3DMPRViewer;

class Q3DMPRViewerToolManager : public ToolManager
{
Q_OBJECT
public:
    Q3DMPRViewerToolManager( QObject *parent = 0 );
    Q3DMPRViewerToolManager( Q3DMPRViewer *viewer, QObject *parent = 0);

    ~Q3DMPRViewerToolManager();

    /// Li assignem el visor \TODO aquest m�tode podria quedar \deprecated obligant a proporcionar el visor en el moment de la construcci� i prou, eliminant el constructor per defecte
    void setViewer( Q3DMPRViewer *viewer );

private:
    /// Inst�ncia del visor sobre el qual estem treballant
    Q3DMPRViewer *m_viewer;

    void initToolRegistration();
};

}

#endif
