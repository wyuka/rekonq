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
#include "sessionmanager.h"
#include "sessionwidget.h"
#include "session.h"
#include "tabbar.h"
#include "webtab.h"

// Qt Includes
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>

PanoramaScene::PanoramaScene(SessionManager *sessionManager)
        : QGraphicsScene(sessionManager)
        , m_sessionManager(sessionManager)
{
    connect(sessionManager, SIGNAL(sessionActivated(Session*)), this, SLOT(activateSession(Session*)));
    connect(sessionManager, SIGNAL(sessionAdded(Session*)), this, SLOT(addSession(Session*)));
    connect(sessionManager, SIGNAL(sessionDeactivated(Session*)), this, SLOT(deactivateSession(Session*)));
    connect(sessionManager, SIGNAL(sessionDeleted(Session*)), this, SLOT(deleteSession(Session*)));

    setBackgroundBrush(Qt::lightGray);
    m_layout = new QGraphicsLinearLayout;
    m_layout->setSpacing(20);
    m_layout->setSpacing(20);
    QGraphicsWidget* form = new QGraphicsWidget;
    form->setLayout(m_layout);
    addItem(form);
}


void PanoramaScene::activateSession(Session* session)
{
    kDebug() << "session activated";
}


void PanoramaScene::addSession(Session* session)
{
    kDebug() << "session added";
    SessionWidget *sessionWidget = new SessionWidget(session);
    m_layout->addItem(sessionWidget);
    m_sessionMap[session] = sessionWidget;
}


void PanoramaScene::deactivateSession(Session* session)
{
    kDebug() << "session deactivated";
}


void PanoramaScene::deleteSession(Session* session)
{
    kDebug() << "session deleted";
}
