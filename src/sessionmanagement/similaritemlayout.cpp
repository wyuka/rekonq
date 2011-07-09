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

SimilarItemLayout::SimilarItemLayout(QGraphicsLayoutItem* parent)
        : QGraphicsLayout(parent)
{
    m_spacing[0] = 6;
    m_spacing[1] = 6;
    QSizePolicy sp = sizePolicy();
    sp.setHeightForWidth(true);
    setSizePolicy(sp);
    m_itemSize = QSizeF(0,0);
}

void SimilarItemLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    static bool firstUse = true;
    if (firstUse)
    {
        m_itemSize = item->effectiveSizeHint(Qt::PreferredSize);
        m_aspectRatio = m_itemSize.width() / m_itemSize.height();
        firstUse = false;
    }

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
    QGraphicsLayout::setGeometry(geom);
    if (m_items.count() < 1)
    {
        return;
    }
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    int itemsPerRow;
    m_itemSize = findItemSize(geom, itemsPerRow);
    if (itemsPerRow < 1)
        return;

    qreal x = 0;
    qreal y = -(m_itemSize.height() + spacing(Qt::Vertical));
    
    for (int i = 0; i < m_items.count(); ++i)
    {
        QGraphicsLayoutItem *item = m_items.at(i);
        if (i % itemsPerRow == 0)
        {
            x = 0;
            y += m_itemSize.height() + spacing(Qt::Vertical);
        }
        else
        {
            x += m_itemSize.width() + spacing(Qt::Horizontal);
        }

        item->setGeometry(QRectF(QPointF(left + x, top + y), m_itemSize));
        kDebug() << "set item number" << i << "to rect" << QRectF(QPointF(left + x, top + y), m_itemSize);
    }
}


QSizeF SimilarItemLayout::findItemSize(const QRectF &geom, int &itemsPerRow)
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF curSize = geom.size() - QSizeF(left + right, top + bottom);
    QSizeF toReturn;
    qreal width, height, maxWidth = 0;

    int i;
    for (i = 1; i <= m_items.count(); ++i)
    {
        width = ((curSize.width() + spacing(Qt::Horizontal)) / i) - spacing(Qt::Horizontal);
        height = width / m_aspectRatio;
        int rows = qCeil(qreal(m_items.count()) / qreal (i));
        qreal totalHeight = height * rows + spacing(Qt::Vertical) * (rows - 1);
        kDebug() << "trying" << i;
        kDebug() << "width" << width;
        kDebug() << "height" << height;
        kDebug() << "rows" << rows;
        kDebug() << "totalHeight" << totalHeight;
        if (totalHeight <= curSize.height() && width > maxWidth)
        {
            kDebug() << "tried" << i << "items";
            kDebug() << "tried width" << width;
            maxWidth = width;
            itemsPerRow = i;
        }
    }
    toReturn = QSizeF(maxWidth, maxWidth / m_aspectRatio);
    kDebug() << "curSize is " << curSize;
    kDebug() << "items per row found to be" << itemsPerRow;
    kDebug() << "itemSize row found to be" << toReturn;
    return toReturn;
}


QSizeF SimilarItemLayout::sizeHint(Qt::SizeHint sizeHint,const QSizeF &constraint) const
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF curSize = geometry().size() - QSizeF(left + right, top + bottom);
    QSizeF toReturn;

    if (m_items.count() < 1)
    {
        kDebug() << "zero items";
        kDebug() << "geom was" << curSize;
        kDebug() << "returning sizeHint" << QSizeF(left + right, top + bottom);
        return QSizeF(left + right, top + bottom);
    }

    QSizeF minSize = itemAt(0)->effectiveSizeHint(Qt::MinimumSize);
    int maxItemsPerRow = qFloor((curSize.width() + spacing(Qt::Horizontal)) / (minSize.width() + spacing(Qt::Horizontal)));
    qreal effectiveMinWidth = (curSize.width() + spacing(Qt::Horizontal)) / qreal (maxItemsPerRow);
    QSizeF effectiveMinSize = QSizeF(effectiveMinWidth, effectiveMinWidth / m_aspectRatio);
    if (curSize.width() <= 0 || curSize.height() <= 0)
    {
        kDebug() << "starting from zero" << m_items.count();
        toReturn = m_itemSize + QSizeF(left + right, top + bottom) + QSizeF(1, 1);
    }
    else if ((qreal(m_items.count()) * (effectiveMinSize.height() * effectiveMinSize.width())
        < (curSize.height() + spacing(Qt::Vertical)) * (curSize.width() + spacing(Qt::Horizontal))))
    {
        toReturn = curSize + QSizeF(left + right, top + bottom);
    }
    else
    {
        kDebug() << "m_itemSize is" << m_itemSize;
        qreal width1 = curSize.width();
        qreal height1 = curSize.height() + spacing(Qt::Vertical) + m_itemSize.height();
        qreal ratio1 = width1 < height1 ? width1 / height1 : height1 / width1;

        qreal width2 = curSize.width() + spacing(Qt::Horizontal) + m_itemSize.width();
        qreal height2 = curSize.height();
        qreal ratio2 = width2 < height2 ? width2 / height2 : height2 / width2;

        if (ratio1 > ratio2)
        {
            toReturn = QSizeF(width1, height1) + QSizeF(left + right, top + bottom);
        }
        else
        {
            toReturn = QSizeF(width2, height2) + QSizeF(left + right, top + bottom);
        }
    }
    kDebug() << "geom was" << curSize;
    kDebug() << "returning sizeHint" << toReturn;
    return toReturn;
}
