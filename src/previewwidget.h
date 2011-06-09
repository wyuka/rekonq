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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QtGui/QGraphicsWidget>

class SessionTabData;

static const qreal thumbAspectRatio = 0.75;
static const qreal thumbToTextRatio = 0.9;

class PreviewWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    PreviewWidget(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    void setTabData(SessionTabData* tabData);

    SessionTabData* tabData();

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    qreal getWidthForHeight(qreal height) const;
    qreal getHeightForWidth(qreal width) const;

private:
    
    SessionTabData* m_tabData;
};

#endif // PREVIEWWIDGET_H
