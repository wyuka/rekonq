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

#include "previewwidget.h"

#include "sessiontabdata.h"

#include <QtGui/QPainter>

PreviewWidget::PreviewWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags): QGraphicsWidget(parent, wFlags)
{

}


void PreviewWidget::setTabData(SessionTabData* tabData)
{
    m_tabData = tabData;
}


SessionTabData* PreviewWidget::tabData()
{
    return m_tabData;
}


void PreviewWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawText(QRectF(0, 0, 200, 20), Qt::AlignCenter, m_tabData->title());
    painter->drawPixmap(0, 20, m_tabData->thumbnail());
    painter->drawRoundedRect(boundingRect(), 10, 10);
}


QSizeF PreviewWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QSizeF(200,170);
}
