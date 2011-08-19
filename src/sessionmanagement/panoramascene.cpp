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


// Self Includes
#include "panoramascene.h"
#include "panoramascene.moc"

// Local Includes
#include "application.h"
#include "mainview.h"
#include "mainwindow.h"
#include "previewwidget.h"
#include "sessionmanager.h"
#include "sessionwidget.h"
#include "session.h"
#include "tabbar.h"
#include "webtab.h"

// Qt Includes
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneDragDropEvent>

PanoramaScene::PanoramaScene(SessionManager *sessionManager)
        : QGraphicsScene(sessionManager)
        , m_sessionManager(sessionManager)
{
    connect(sessionManager, SIGNAL(sessionActivated(Session*)), this, SLOT(activateSession(Session*)));
    connect(sessionManager, SIGNAL(sessionAdded(Session*)), this, SLOT(addSession(Session*)));
    connect(sessionManager, SIGNAL(sessionDeactivated(Session*)), this, SLOT(deactivateSession(Session*)));
    connect(sessionManager, SIGNAL(sessionRemoved(Session*)), this, SLOT(removeSession(Session*)));

    setBackgroundBrush(Qt::lightGray);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setSpacing(20);
    layout->setSpacing(20);
}


PanoramaScene::~PanoramaScene()
{
}


SessionWidget* PanoramaScene::widgetForSession(Session* session)
{
    return m_sessionMap[session];
}


SessionWidget* PanoramaScene::currentSessionWidget()
{
    return m_currentSessionWidget.data();
}


void PanoramaScene::setCurrentSessionWidget(SessionWidget* sessionWidget)
{
    if (m_currentSessionWidget.data())
    {
        m_currentSessionWidget.data()->setCurrent(false);
    }
    m_currentSessionWidget = sessionWidget;
    sessionWidget->setCurrent(true);
}


void PanoramaScene::activateSession(Session* session)
{
    kDebug() << "session activated";
    SessionWidget *sw;
    if ((sw = m_sessionMap[session]) != 0)
    {
        sw->setSessionActive(true);
    }
}


void PanoramaScene::addSession(Session* session)
{
    kDebug() << "session added";
    SessionWidget *sessionWidget = new SessionWidget(session);
    addItem(sessionWidget);
    sessionWidget->setPos(findBestPosition());
    m_sessionMap[session] = sessionWidget;
}


void PanoramaScene::deactivateSession(Session* session)
{
    kDebug() << "session deactivated";
    SessionWidget *sw;
    if ((sw = m_sessionMap[session]) != 0)
    {
        sw->setSessionActive(false);
    }
}


void PanoramaScene::removeSession(Session* session)
{
    kDebug() << "session deleted";
    SessionWidget *sw;
    if ((sw = m_sessionMap[session]) != 0)
    {
        m_sessionMap.remove(session);
        sw->deleteLater();
    }
}


QPointF PanoramaScene::findBestPosition()
{
    QList<SessionWidget*> swList = m_sessionMap.values();
    qreal maxY = 0, maxX = 0;
    kDebug() << "count is" << swList.count();
    foreach (SessionWidget* sw, swList)
    {
        maxY = qMax (maxY, sw->y() + 30);
        maxX = qMax (maxX, sw->x() + 30);
        //TODO: find a better algo for placing stuff.
        //going with this for now.
        kDebug() << "new sessionWidget placed at" << maxX << "," << maxY;
    }
    return QPointF(maxX, maxY);
}
