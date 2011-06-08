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
#include "mainwindow.h"
#include "session.h"
#include "sessionmanager.h"
#include "sessiontabdata.h"
#include "sessionview.h"

#include <QtGui/QGraphicsItem>



SessionView::SessionView(QWidget* parent)
        : QGraphicsView(parent)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);

    //start off hidden
    hide();
}


void SessionView::showEvent(QShowEvent* event)
{
    scene()->clear();

    SessionList sessionList = rApp->sessionManager()->sessionList();
    int windowY = 0;
    foreach(Session* session, sessionList)
    {
        QGraphicsTextItem* text;
        if (session->isActive())
        {
            text = scene()->addText(session->window()->windowTitle());
        }
        else
        {
            text = scene()->addText("Inactive session");
        }
        text->setPos(-100, windowY);

        TabDataList tabDataList = session->tabDataList();
        int tabY = windowY;
        foreach(SessionTabData* tabData, tabDataList)
        {
            QGraphicsTextItem* text;
            text = scene()->addText(tabData->title());
            tabY += 20;
            text->setPos(0, tabY);

            QGraphicsPixmapItem* pixmap;
            QGraphicsRectItem* rect;
            QPixmap thumb = tabData->thumbnail();
            pixmap = scene()->addPixmap(thumb);
            rect = scene()->addRect(thumb.rect());
            tabY += 20;
            pixmap->setPos(0, tabY);
            rect->setPos(0, tabY);
            tabY += thumb.height();
        }

        windowY = tabY + 20;
    }

    QGraphicsView::showEvent(event);
}
