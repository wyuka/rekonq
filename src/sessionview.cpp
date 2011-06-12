/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2010-2011 by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "application.h"
#include "flowlayout.h"
#include "mainwindow.h"
#include "session.h"
#include "sessionmanager.h"
#include "sessiontabdata.h"
#include "sessionview.h"
#include "sessionwidget.h"

#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtGui/QGraphicsLinearLayout>


SessionView::SessionView(QWidget* parent)
        : QGraphicsView(parent)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    scene->setBackgroundBrush(Qt::lightGray);

    //start off hidden
    hide();
}


void SessionView::showEvent(QShowEvent* event)
{
    SessionList sessionList = rApp->sessionManager()->sessionList();
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout;
    layout->setSpacing(20);
    layout->setSpacing(20);
    QGraphicsWidget* form = new QGraphicsWidget;
    form->setLayout(layout);
    scene()->addItem(form);
    Session* currentSession = rApp->sessionManager()->currentSession();
    foreach(Session* session, sessionList)
    {
        SessionWidget* sw = new SessionWidget(form);
        sw->setSession(session);
        connect(sw, SIGNAL(mousePressed()), this, SLOT(setCurrentSessionWidget()));
        layout->addItem(sw);
        if (session == currentSession)
        {
            m_currentSessionWidget = sw;
            m_currentSessionWidget.data()->setCurrent(true);
        }
    }
    QGraphicsView::showEvent(event);
}


void SessionView::hideEvent(QHideEvent* event)
{
    scene()->clear();
    QWidget::hideEvent(event);
}


void SessionView::setCurrentSessionWidget()
{
    SessionWidget* toBeCurrent = static_cast<SessionWidget*>(sender());
    if (m_currentSessionWidget.data() == toBeCurrent)
        return;

    if (m_currentSessionWidget.data())
    {
        m_currentSessionWidget.data()->setCurrent(false);
    }
    m_currentSessionWidget = toBeCurrent;
    m_currentSessionWidget.data()->setCurrent(true);
}

