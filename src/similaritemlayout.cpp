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
#include "kdebug.h"

SimilarItemLayout::SimilarItemLayout()
{
    m_spacing[0] = 6;
    m_spacing[1] = 6;
    QSizePolicy sp = sizePolicy();
    sp.setHeightForWidth(false);
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
     }
}

QSizeF SimilarItemLayout::sizeHint(Qt::SizeHint sizeHint,const QSizeF &constraint) const
{
    QSizeF size;
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    int maxw = constraint.width() - left - right;
    int maxh = constraint.height() - top - bottom;
    QSizeF itemSize;
    if (count() < 1)
    {
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
        kDebug() << "case A";
    }
    else if (constraint.width() >= 0 && itemSize.width() < maxw) // height for width
    {   
        int horizItems = qFloor((maxw + spacing(Qt::Horizontal)) / (itemSize.width() + spacing(Qt::Horizontal)));
        int vertItems = qCeil(qreal(count()) / horizItems);
        qreal height = vertItems*itemSize.height() + (vertItems - 1) * spacing(Qt::Vertical) + top + bottom;
        size = QSizeF(constraint.width(), height);
        kDebug() << "case B";
    }
    else
    {
        // try to be as square-ish in demension as possible ( means keep the aspect ratio as near to 1 as possible )
        int horizItems, vertItems;
        int horizItems1 = qCeil(qSqrt(qreal(count())*itemSize.height()/itemSize.width()));        // when i prefer width over height
        int vertItems1 = qCeil(qreal(count())/qreal(horizItems1));
        int horizItems2 = qFloor(qSqrt(qreal(count())*itemSize.height()/itemSize.width()));       // when i prefer height over width
        int vertItems2 = qCeil(qreal(count())/qreal(horizItems2));
        qreal squareness1 = qAbs<qreal>((vertItems1 < horizItems1 ? qreal(vertItems1)/qreal(horizItems1) : qreal(horizItems1) / qreal(vertItems1)) - 1.0); // here squareness is an inverse measure of squareness :-P
        qreal squareness2 = qAbs<qreal>((vertItems2 < horizItems2 ? qreal(vertItems2)/qreal(horizItems2) : qreal(horizItems2) / qreal(vertItems2)) - 1.0);
        if ( squareness1 < squareness2 )        // select the one which is more square-ish
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
        kDebug() << "case C";
    }
    if (size.width() < size.height())
        kDebug() << "for" << count() << "items size is" << size.width() << size.height();
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
