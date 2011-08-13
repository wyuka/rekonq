/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2011 by Tirtha Chatterjee <tirtha.p.chatterjee@gmail.com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */


#ifndef PANORAMA_SCENE_H
#define PANORAMA_SCENE_H


// Rekonq Includes
#include "rekonq_defines.h"

// Qt Includes
#include <QGraphicsScene>

// Forward Declarations
class QGraphicsWidget;
class PreviewWidget;
class Session;
class SessionManager;
class SessionWidget;

/**
  * A QGraphicsScene to show the panorama view
  */

class PanoramaScene : public QGraphicsScene
{
    Q_OBJECT

public:
    PanoramaScene(SessionManager *sessionManager);
    virtual ~PanoramaScene();

    inline PreviewWidget* currentlyDragged()
    {
        return m_currentlyDragged;
    }

    inline void setCurrentlyDragged(PreviewWidget *draggedWidget)
    {
        m_currentlyDragged = draggedWidget;
    }

    SessionWidget* widgetForSession(Session* session);

    void setCurrentSessionWidget(SessionWidget* sessionWidget);
    SessionWidget* currentSessionWidget();
protected slots:
    void addSession(Session* session);
    void activateSession(Session* session);
    void deactivateSession(Session* session);
    void removeSession(Session* session);

private:
    QGraphicsWidget *m_form;
    SessionManager *m_sessionManager;
    QMap<Session*, SessionWidget*> m_sessionMap;

    PreviewWidget *m_currentlyDragged;
    QWeakPointer<SessionWidget> m_currentSessionWidget;
};


#endif // PANORAMA_SCENE_H
