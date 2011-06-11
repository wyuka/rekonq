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

#include "sessionwidget.h"
#include "sessionwidget.moc"

#include "flowlayout.h"
#include "previewwidget.h"
#include "session.h"
#include "sessiontabdata.h"


#include <QtGui/QGraphicsDropShadowEffect>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>


SessionWidget::SessionWidget(FlowLayout* layout, QGraphicsItem* parent, Qt::WindowFlags wFlags)
        : QGraphicsWidget(parent, wFlags)
        , m_current(false)
{
    m_session = 0;
    m_layout = layout;
}


void SessionWidget::setCurrent(bool current)
{
    m_current = current;
    if (m_current == false)
    {
        setGraphicsEffect(0);
        if (m_currentPreviewWidget.data())
            m_currentPreviewWidget.data()->setCurrent(false);
        if (m_dropShadow.data())
            m_dropShadow.data()->deleteLater();
    }
    else
    {
        m_dropShadow = new QGraphicsDropShadowEffect(this);
        m_dropShadow.data()->setOffset(QPointF(1, 1));
        m_dropShadow.data()->setColor(Qt::black);
        m_dropShadow.data()->setBlurRadius(20);
        setGraphicsEffect(m_dropShadow.data());
        if (m_currentPreviewWidget.data())
            m_currentPreviewWidget.data()->setCurrent(true);
    }
    update();
}


void SessionWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setBrush(QColor(224,224,224));
    if (m_current)
    {
        painter->setPen(Qt::NoPen);
    }
    else
    {
        QPen pen(QColor(240,240,240));
        pen.setWidth(3);
        painter->setPen(pen);
    }
    painter->drawRoundedRect(boundingRect(), 5, 5);
    QGraphicsWidget::paint(painter, option, widget);
}


QSizeF SessionWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QGraphicsWidget::sizeHint(which, constraint);
}


Session* SessionWidget::session()
{
    return m_session;
}


void SessionWidget::setSession(Session* session)
{
    m_session = session;
    FlowLayout *layout = new FlowLayout;
    layout->setContentsMargins(10 , 10, 10, 10);
    layout->setSpacing(Qt::Horizontal, 10);
    layout->setSpacing(Qt::Vertical, 10);
    TabDataList tabDataList = m_session->tabDataList();
    SessionTabData* currentTabData = session->currentTabData();
    foreach (SessionTabData* tabData, tabDataList)
    {
        PreviewWidget* pw = new PreviewWidget;
        pw->setTabData(tabData);
        connect(pw, SIGNAL(mousePressed()), this, SLOT(setCurrentPreviewWidget()));
        layout->addItem(pw);
        if (tabData == currentTabData)
        {
            m_currentPreviewWidget = pw;
            m_currentPreviewWidget.data()->setCurrent(true);
        }
    }
    setLayout(layout);
}


void SessionWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_inMotion = false;
}

void SessionWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_inMotion = true;
    //m_layout->removeItem(this);
    emit mousePressed();
}

void SessionWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    setPos(mapToParent(event->pos()-event->lastPos()));
}


void SessionWidget::setCurrentPreviewWidget()
{
    PreviewWidget* toBeCurrent = static_cast<PreviewWidget*>(sender());
    if (m_currentPreviewWidget.data() == toBeCurrent)
        return;

    if (m_currentPreviewWidget.data())
    {
        m_currentPreviewWidget.data()->setCurrent(false);
    }
    m_currentPreviewWidget = toBeCurrent;
    m_currentPreviewWidget.data()->setCurrent(true);
}