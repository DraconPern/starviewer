/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERTOOLMANAGER_H
#define UDGQ2DVIEWERTOOLMANAGER_H

#include "toolmanager.h"

namespace udg {

class Q2DViewer;

/**
Tool Manager per al visor Q2DViewer

	@author Grup de Gr�fics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerToolManager : public ToolManager
{
Q_OBJECT
public:
    Q2DViewerToolManager(QObject *parent = 0);
    Q2DViewerToolManager( Q2DViewer *viewer, QObject *parent = 0 );

    ~Q2DViewerToolManager();

    /// Li assignem el visor \TODO aquest m�tode podria quedar \deprecated obligant a proporcionar el visor en el moment de la construcci� i prou, eliminant el constructor per defecte
    void setViewer( Q2DViewer *viewer );

private:
    /// Inst�ncia del visor sobre el qual estem treballant
    Q2DViewer *m_viewer;

    void initToolRegistration();
};

}

#endif
