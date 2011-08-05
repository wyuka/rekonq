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

#include "panoramaview.h"
#include "panoramaview.moc"

#include "application.h"
#include "sessionmanager.h"
#include "session.h"

#include <QGraphicsView>
#include <QToolButton>

#include <KIcon>
#include <QMouseEvent>
#include <qscrollbar.h>

PanoramaView::PanoramaView(QWidget* parent)
        : QGraphicsView(parent)
{
    m_toolButton = new QToolButton(this); 
    KIcon addIcon("list-add");
    m_toolButton->setIcon(addIcon);
    connect(m_toolButton, SIGNAL(clicked(bool)), this, SLOT(addSession()));
    hide();
}


PanoramaView::~PanoramaView()
{
}


void PanoramaView::resizeEvent(QResizeEvent* event)
{
    //FIXME: not really a nice way to position items, but doing it to avoid overlaying on top of the verticalScrollBar
    if (verticalScrollBar()->isVisible())
    {
        m_toolButton->move(width() - verticalScrollBar()->width() - m_toolButton->width() - 5, 5);
        kDebug() << verticalScrollBar()->width();
    }
    else
    {
        m_toolButton->move(width() - m_toolButton->width() - 5, 5);
    }
    QGraphicsView::resizeEvent(event);
}


void PanoramaView::addSession()
{
    Session* s = rApp->sessionManager()->newSession(false);
    SessionTabData * tabData = new SessionTabData(s);
    tabData->setUrl(KUrl("about:blank"));
    s->addTab(tabData);
}
