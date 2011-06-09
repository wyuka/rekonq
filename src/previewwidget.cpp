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
#include "previewwidget.moc"

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
    QString title = m_tabData->title();
    QPixmap thumbnail = m_tabData->thumbnail();
    
    QSizeF sh = this->size();
    qreal thumbheight = sh.height() * thumbToTextRatio;
    qreal thumbwidth = thumbheight * (1 / thumbAspectRatio);

    painter->drawPixmap(QRectF(0, 0, thumbwidth, thumbheight), thumbnail, thumbnail.rect());
    if (painter->fontMetrics().width(title) > sh.width())
    {
        painter->drawText(QRectF(0, thumbheight, sh.width(), sh.height() - thumbheight), Qt::AlignLeft | Qt::AlignVCenter, title);
    }
    else
    {
        painter->drawText(QRectF(0, thumbheight, sh.width(), sh.height() - thumbheight), Qt::AlignCenter | Qt::AlignVCenter, title);
    }
}


QSizeF PreviewWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    QSizeF sh = constraint;

    switch (which)
    {
        case Qt::PreferredSize:
            if (constraint.width() > 0)
            {
                sh = QSizeF(constraint.width(), getHeightForWidth(constraint.width()));
            }
            else if(constraint.height() > 0)
            {
                sh = QSizeF(getWidthForHeight(constraint.height()), constraint.height());
            }
            else
            {
                sh = QSizeF(200,getHeightForWidth(200));
            }
            break;
        case Qt::MinimumSize:
            sh = QSizeF(100, getHeightForWidth(100));
            break;
        case Qt::MaximumSize:
            sh = QSizeF(400, getHeightForWidth(400));
            break;
        default:
            break;
    }
    return sh;
}


qreal PreviewWidget::getWidthForHeight(qreal height) const
{
    return (height*thumbToTextRatio)*(1/thumbAspectRatio);
}


qreal PreviewWidget::getHeightForWidth(qreal width) const
{
    return width*thumbAspectRatio*(1/thumbToTextRatio);
}
