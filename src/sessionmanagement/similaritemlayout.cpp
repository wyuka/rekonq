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

#include "similaritemlayout.h"
#include <QtGui/qwidget.h>
#include <QtCore/qmath.h>
#include "kdebug.h"

SimilarItemLayout::SimilarItemLayout()
{
    m_spacing[0] = 6;
    m_spacing[1] = 6;
    QSizePolicy sp = sizePolicy();
    sp.setHeightForWidth(true);
    setSizePolicy(sp);
}

void SimilarItemLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    item->setParentLayoutItem(this);
    if (uint(index) > uint(m_items.count()))
        index = m_items.count();
    m_items.insert(index, item);
    invalidate();
}

int SimilarItemLayout::count() const
{
    return m_items.count();
}

QGraphicsLayoutItem *SimilarItemLayout::itemAt(int index) const
{
    return m_items.value(index);
}

void SimilarItemLayout::removeAt(int index)
{
    QGraphicsLayoutItem* item = m_items.value(index);
    m_items.removeAt(index);
    item->setParentLayoutItem(0);
    invalidate();
}

void SimilarItemLayout::removeItem(QGraphicsLayoutItem* item)
{
    if (item && m_items.contains(item))
    {
        
        m_items.removeOne(item);
        item->setParentLayoutItem(0);
        invalidate();
    }
}

qreal SimilarItemLayout::spacing(Qt::Orientation o) const
{
    return m_spacing[int(o) - 1];
}

void SimilarItemLayout::setSpacing(Qt::Orientations o, qreal spacing)
{
    if (o & Qt::Horizontal)
        m_spacing[0] = spacing;
    if (o & Qt::Vertical)
        m_spacing[1] = spacing;
}

void SimilarItemLayout::setGeometry(const QRectF &geom)
{
    kDebug() << "setGeometry called, items=" << m_items.count();
    QGraphicsLayout::setGeometry(geom);
    if (m_items.count() < 1)
    {
        return;
    }
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF itemSize = itemAt(0)->effectiveSizeHint(Qt::PreferredSize);
    int horizItems = qFloor((geom.width() + spacing(Qt::Horizontal) - left - right) / (itemSize.width() + spacing(Qt::Horizontal)));
    if (horizItems <= 0)
    {
        return;
    }
    qreal x = 0;
    qreal y = -(itemSize.height() + spacing(Qt::Vertical));
    
    for (int i = 0; i < m_items.count(); ++i)
    {
        QGraphicsLayoutItem *item = m_items.at(i);
        if (i % horizItems == 0)
        {
            x = 0;
            y += itemSize.height() + spacing(Qt::Vertical);
        }
        else
        {
            x += itemSize.width() + spacing(Qt::Horizontal);
        }
        
        item->setGeometry(QRectF(QPointF(left + x, top + y), itemSize));
        kDebug() << "set item number" << i << "to rect" << QRectF(QPointF(left + x, top + y), itemSize);
    }
}

QSizeF SimilarItemLayout::sizeHint(Qt::SizeHint sizeHint,const QSizeF &constraint) const
{
    kDebug() << "sizeHint requested";
    QSizeF size;
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    int maxw = constraint.width() - left - right;
    int maxh = constraint.height() - top - bottom;
    QSizeF itemSize;
    if (count() < 1)
    {
        kDebug() << left+right << top+bottom;
        return QSizeF(left + right, top + bottom);
    }
    else
    {
        itemSize = itemAt(0)->effectiveSizeHint(sizeHint);
    }
    
    if (constraint.height() >= 0 && itemSize.height() < maxh) // width for height
    {
        int vertItems = qFloor((maxh + spacing(Qt::Vertical)) / (itemSize.height() + spacing(Qt::Vertical)));
        int horizItems = qCeil(qreal(count()) / vertItems);
        qreal width = horizItems*itemSize.width() + (horizItems - 1) * spacing(Qt::Horizontal) + left + right;
        size = QSizeF(width, constraint.height());
        //kDebug() << "case A";
    }
    else if (constraint.width() >= 0 && itemSize.width() < maxw) // height for width
    {   
        int horizItems = qFloor((maxw + spacing(Qt::Horizontal)) / (itemSize.width() + spacing(Qt::Horizontal)));
        int vertItems = qCeil(qreal(count()) / horizItems);
        qreal height = vertItems*itemSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
        size = QSizeF(constraint.width(), height);
        //kDebug() << "case B";
    }
    else
    {
        // try to be as square-ish in demension as possible ( means keep the aspect ratio as near to 1 as possible )
        int horizItems, vertItems;
        // when i prefer width over height
        int horizItems1 = qCeil(qSqrt(qreal(count())*itemSize.height()/itemSize.width()));
        int vertItems1 = qCeil(qreal(count())/qreal(horizItems1));
        // when i prefer height over width
        int horizItems2 = qFloor(qSqrt(qreal(count())*itemSize.height()/itemSize.width()));
        int vertItems2 = qCeil(qreal(count())/qreal(horizItems2));
        // here squareness is an inverse measure of squareness :-P
        qreal squareness1 = qAbs<qreal>((vertItems1 < horizItems1 ?
            qreal(vertItems1)/qreal(horizItems1) : qreal(horizItems1) / qreal(vertItems1)) - 1.0);
        qreal squareness2 = qAbs<qreal>((vertItems2 < horizItems2 ?
            qreal(vertItems2)/qreal(horizItems2) : qreal(horizItems2) / qreal(vertItems2)) - 1.0);
        // select the one which is more square-ish
        if ( squareness1 < squareness2 )
        {
            horizItems = horizItems1;
            vertItems = vertItems1;
        }
        else
        {
            horizItems = horizItems2;
            vertItems = vertItems2;
        }
        qreal width = horizItems*itemSize.width() + (horizItems - 1) * spacing(Qt::Horizontal) + left + right;
        qreal height = vertItems*itemSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
        size = QSizeF(width, height);
        //kDebug() << "case C";
    }
    //if (size.width() < size.height())
    //    kDebug() << "for" << count() << "items size is" << size.width() << size.height();
    kDebug() << size.width() << size.height();
    return size;
}

void SimilarItemLayout::getContentsMargin(qreal* left, qreal* top, qreal* right, qreal* bottom)
{
    if (left)
    {
        *left = m_left;
    }

    if (top)
    {
        *top = m_top;
    }

    if (right)
    {
        *right = m_right;
    }

    if (bottom)
    {
        *bottom = m_bottom;
    }
}


void SimilarItemLayout::setContentsMargin(qreal left, qreal top, qreal right, qreal bottom)
{
    m_left = left;
    m_top = top;
    m_right = right;
    m_bottom = bottom;
}
