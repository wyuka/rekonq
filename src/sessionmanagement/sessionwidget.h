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

#ifndef SESSIONWIDGET_H
#define SESSIONWIDGET_H

#include <QtGui/QGraphicsWidget>
#include <QtCore/QWeakPointer>

class SimilarItemLayout;
class FlowLayout;
class Session;
class PreviewWidget;

class QGraphicsDropShadowEffect;

class SessionWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    SessionWidget(QGraphicsItem* parent = 0);
    void setSession(Session* session);
    
    Session* session();

signals:
    void mousePressed();

public slots:
    void setCurrent(bool current = true);

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

protected slots:
    void setCurrentPreviewWidget();

private:
    Session* m_session;
    bool m_current;
    QGraphicsDropShadowEffect* m_dropShadow;
    bool m_inMotion;
    SimilarItemLayout *m_layout;

    QWeakPointer<PreviewWidget> m_currentPreviewWidget;
};

#endif // SESSIONWIDGET_H
