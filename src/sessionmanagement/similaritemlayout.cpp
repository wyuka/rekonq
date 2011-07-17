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
#include <QGraphicsLayoutItem>
#include <QSizePolicy>
#include "kdebug.h"

SimilarItemLayout::SimilarItemLayout(QGraphicsLayoutItem* parent)
        : QGraphicsLayout(parent)
        , m_firstUse(true)
{
    m_spacing[0] = 6;
    m_spacing[1] = 6;
    QSizePolicy sp = QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sp.setHeightForWidth(false);
    setSizePolicy(sp);
    sizePolicy().setHeightForWidth(false);
    m_itemSize = QSizeF(0,0);
}


SimilarItemLayout::~SimilarItemLayout()
{
    for (int i = count() - 1; i >= 0; --i) {
        QGraphicsLayoutItem *item = itemAt(i);
        // The following lines are added to prevent a crash, which seems to be
        // a bug in Qt, which occurs with nested layouts.
        removeAt(i);
        if (item) {
            item->setParentLayoutItem(0);
            if (item->ownedByLayout())
                delete item;
        }
    }
}

void SimilarItemLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    if (m_firstUse)
    {
        m_itemSize = item->effectiveSizeHint(Qt::MinimumSize);
        m_aspectRatio = m_itemSize.width() / m_itemSize.height();
        m_firstUse = false;
    }

    item->setParentLayoutItem(this);
    if (uint(index) > uint(m_items.count()))
        index = m_items.count();
    m_items.insert(index, item);
    updateMinimumSize();
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
    updateMinimumSize();
    invalidate();
}

void SimilarItemLayout::removeItem(QGraphicsLayoutItem* item)
{
    if (item && m_items.contains(item))
    {
        
        m_items.removeOne(item);
        item->setParentLayoutItem(0);
        updateMinimumSize();
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

void SimilarItemLayout::setSpacing(qreal spacing)
{
    m_spacing[0] = spacing;
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
    m_itemSize = findItemSize(geom, &itemsPerRow);
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

        item->setGeometry(QRectF(geom.topLeft() + QPointF(left + x, top + y), m_itemSize));
    }
}


QSizeF SimilarItemLayout::findItemSize(const QRectF &geom, int *itemsPerRow)
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF curSize = geom.size() - QSizeF(left + right, top + bottom);
    QSizeF minSize = m_items.at(0)->effectiveSizeHint(Qt::MinimumSize);
    qreal width, height, maxWidth = minSize.width();

    int i;
    *itemsPerRow = 0;
    for (i = 1; i <= m_items.count(); ++i)
    {
        width = ((curSize.width() + spacing(Qt::Horizontal)) / i) - spacing(Qt::Horizontal);
        height = width / m_aspectRatio;
        int rows = qCeil(qreal(m_items.count()) / qreal (i));
        qreal totalHeight = height * rows + spacing(Qt::Vertical) * (rows - 1);
        if (totalHeight <= curSize.height() && width >= maxWidth)
        {
            maxWidth = width;
            *itemsPerRow = i;
        }
    }
    if (*itemsPerRow)
        return QSizeF(maxWidth, maxWidth / m_aspectRatio);
    else
    {
        //too little height;
        int i;
        qreal maxHeight = minSize.height();
        for (i = 1; i <= m_items.count(); ++i)
        {
            height = ((curSize.height() + spacing(Qt::Vertical)) / i) - spacing(Qt::Vertical);
            width = height / m_aspectRatio;
            int cols = qCeil(qreal(m_items.count()) / qreal (i));
            qreal totalWidth = width * cols + spacing(Qt::Horizontal) * (cols - 1);
            if (totalWidth <= curSize.width() && height >= maxHeight)
            {
                maxHeight = height;
                *itemsPerRow = cols;
            }
        }
        return QSizeF(maxHeight * m_aspectRatio, maxHeight);
    }
}


void SimilarItemLayout::updateMinimumSize()
{
    if (m_items.size() == 0)
    {
        setMinimumSize(200, 200);
        return;
    }
    QSizeF minSize = m_items.at(0)->effectiveSizeHint(Qt::MinimumSize);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    if (minSize.width() <= 0)
    {
        minSize = m_itemSize;
    }
  
    // try to be as square-ish in demension as possible ( means keep the aspect ratio as near to 1 as possible )
    int horizItems, vertItems;
    // when i prefer width over height
    int horizItems1 = qCeil(qSqrt(qreal(m_items.count())/m_aspectRatio));
    int vertItems1 = qCeil(qreal(m_items.count())/qreal(horizItems1));
    // when i prefer height over width
    int horizItems2 = qFloor(qSqrt(qreal(m_items.count())/m_aspectRatio));
    int vertItems2 = qCeil(qreal(count())/qreal(horizItems2));
        
    qreal squareness1 = vertItems1 * minSize.height() < horizItems1 * minSize.width() ?
    (vertItems1 * minSize.height()) / (horizItems1 * minSize.width()) : (horizItems1 * minSize.width()) / (vertItems1 * minSize.height());
    qreal squareness2 = vertItems2 * minSize.height() < horizItems2 * minSize.width() ?
    (vertItems2 * minSize.height()) / (horizItems2 * minSize.width()) : (horizItems2 * minSize.width()) / (vertItems2 * minSize.height());
    if (squareness1 > squareness2)
    {
        horizItems = horizItems1;
        vertItems = vertItems1;
    }
    else
    {
        horizItems = horizItems2;
        vertItems = vertItems2;
    }
    qreal width = horizItems*minSize.width() + (horizItems - 1) * spacing(Qt::Horizontal) + left + right;
    qreal height = vertItems*minSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
    setMinimumSize(QSizeF(width+0.1, height+0.1));
}


QSizeF SimilarItemLayout::sizeHint(Qt::SizeHint sizeHint,const QSizeF &constraint) const
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF curSize = geometry().size() - QSizeF(left + right, top + bottom);
    QSizeF toReturn;

    if (sizeHint == Qt::MinimumSize)
        return QSizeF(60, 60);
    else if (sizeHint == Qt::MaximumSize)
        return QSizeF(-1, -1);
    else
    {
        if (constraint.width() <= 0 && constraint.height() <= 0)
            return QSizeF(60, 60);
        else if (constraint.width() <= 0)
            return QSizeF(constraint.height()*2., constraint.height());
        else
            return QSizeF(constraint.width(), constraint.width() /2.);
    }
}
