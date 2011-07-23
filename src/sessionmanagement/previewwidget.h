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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QGraphicsWidget>
#include <QWeakPointer>

class SessionWidget;
class SessionTabData;
class QGraphicsDropShadowEffect;

static const qreal thumbAspectRatio = 0.75;
static const qreal thumbToTextRatio = 0.85;

class PreviewWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    PreviewWidget(SessionTabData *tabData, SessionWidget* parent = 0);
    virtual ~PreviewWidget();

    SessionTabData* tabData();

    inline SessionWidget *parentSessionWidget()
    {
        return m_parent;
    }

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    qreal widthForHeight(qreal height) const;
    qreal heightForWidth(qreal width) const;

signals:
    void mousePressed();

public slots:
    void setCurrent(bool current = true);

private:
    QWeakPointer<SessionTabData> m_tabData;

    bool m_current;
    QGraphicsDropShadowEffect *m_dropShadow;
    SessionWidget *m_parent;
};

#endif // PREVIEWWIDGET_H
