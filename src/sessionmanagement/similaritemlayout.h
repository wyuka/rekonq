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

#ifndef SIMILARITEMLAYOUT_H
#define SIMILARITEMLAYOUT_H

#include <QGraphicsLayout>

class SimilarItemLayout : public QGraphicsLayout
{
public:
    SimilarItemLayout(QGraphicsLayoutItem* parent = 0);
    virtual ~SimilarItemLayout();

    inline void addItem(QGraphicsLayoutItem *item);
    void insertItem(int index, QGraphicsLayoutItem *item);
    void setSpacing(Qt::Orientations o, qreal spacing);
    void setSpacing(qreal spacing);
    qreal spacing(Qt::Orientation o) const;
    // inherited functions
    void setGeometry(const QRectF &geom);

    int count() const;
    QGraphicsLayoutItem *itemAt(int index) const;
    void removeAt(int index);
    void removeItem(QGraphicsLayoutItem *item);

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    QSizeF findItemSize(const QRectF &geom, int &itemsPerRow);

private:

    QList<QGraphicsLayoutItem*> m_items;
    qreal m_spacing[2];
    QSizeF m_itemSize;
    qreal m_aspectRatio;

    bool m_firstUse;
};

void SimilarItemLayout::addItem(QGraphicsLayoutItem* item)
{
     insertItem(-1, item);
}

#endif