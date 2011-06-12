/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "similaritemlayout.h"
#include <QtGui/qwidget.h>
#include <QtCore/qmath.h>

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
    QGraphicsLayout::setGeometry(geom);
    doLayout(geom, true);
}

qreal SimilarItemLayout::doLayout(const QRectF &geom, bool applyNewGeometry) const
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    const qreal width = geom.width() - left - right;
    const QSizeF itemSize = itemAt(0)->effectiveSizeHint(Qt::PreferredSize);
    qreal x = 0;
    qreal y = 0;
    qreal maxRowHeight = 0;
    QSizeF pref;
    for (int i = 0; i < m_items.count(); ++i)
    {
        QGraphicsLayoutItem *item = m_items.at(i);
        pref = item->effectiveSizeHint(Qt::PreferredSize);
        maxRowHeight = qMax(maxRowHeight, pref.height());

        qreal next_x;
        next_x = x + pref.width();
        if (next_x > maxw)
        {
            if (x == 0)
            {
                pref.setWidth(maxw);
            }
            else
            {
                x = 0;
                next_x = pref.width();
            }
            y += maxRowHeight + spacing(Qt::Vertical);
            maxRowHeight = 0;
        }

        if (applyNewGeometry)
            item->setGeometry(QRectF(QPointF(left + x, top + y), pref));
        x = next_x + spacing(Qt::Horizontal);
     }
    maxRowHeight = qMax(maxRowHeight, pref.height());
    return top + y + maxRowHeight + bottom;
}

QSizeF SimilarItemLayout::sizeHint(Qt::SizeHint sizeHint, const QSizeF &constraint) const
{
    QSizeF size(0, 0);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QSizeF itemSize;
    if (count() < 1)
    {
        return QSizeF(left + right, top + bottom);
    }
    else
    {
        itemSize = itemAt(0)->effectiveSizeHint(sizeHint);
    }

    if (constraint.height() >= 0) // width for height
    {
        const int vertItems = qFloor((constraint.height() + spacing(Qt::Vertical) - top - bottom) / (itemSize.height() + spacing(Qt::Vertical)));
        const int horizItems = qCeil(qreal(count()) / vertItems);
        const qreal width = horizItems*itemSize.width() + (horizItems - 1) * spacing(Qt::Horizontal) + left + right;
        size = QSizeF(width, constraint.height());
    }
    else if (constraint.width() >= 0) // height for width
    {   
        const int horizItems = qFloor((constraint.width() + spacing(Qt::Horizontal) - left - right) / (itemSize.width() + spacing(Qt::Horizontal)));
        const int vertItems = qCeil(qreal(count()) / horizItems);
        const qreal height = vertItems*itemSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
        size = QSizeF(constraint.width(), height);
    }
    else
    {
        const int vertItems = qFloor(qSqrt(count()));
        const int horizItems = qCeil(qSqrt(count()));
        const qreal width = horizItems*itemSize.width() + (horizItems - 1) * spacing(Qt::Horizontal) + left + right;
        const qreal height = vertItems*itemSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
        size = QSizeF(width, height);
    }
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
