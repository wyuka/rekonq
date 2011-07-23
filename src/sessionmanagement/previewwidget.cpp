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

#include "previewwidget.h"
#include "previewwidget.moc"

#include "application.h"
#include "sessiontabdata.h"
#include "sessionwidget.h"

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include "session.h"
#include "sessionmanager.h"
#include "panoramascene.h"

PreviewWidget::PreviewWidget(SessionTabData *tabData, SessionWidget* parent)
        : QGraphicsWidget(parent)
        , m_current(false)
        , m_parent(parent)
{
    m_tabData = tabData;

    m_dropShadow = new QGraphicsDropShadowEffect(this);
    m_dropShadow->setOffset(QPointF(1, 1));
    m_dropShadow->setColor(QColor(50,50,255));
    m_dropShadow->setBlurRadius(20);
    setGraphicsEffect(m_dropShadow);
    m_dropShadow->setEnabled(false);
}


PreviewWidget::~PreviewWidget()
{
}


SessionTabData* PreviewWidget::tabData()
{
    return m_tabData.data();
}


void PreviewWidget::setCurrent(bool current)
{
    m_current = current;
    if (m_current == false)
    {
        m_dropShadow->setEnabled(false);
    }
    else
    {
        m_dropShadow->setEnabled(true);
    }
    update();
}


void PreviewWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (m_tabData.data() == 0)
    {
        return;
    }

    QString title = m_tabData.data()->title();
    QPixmap thumbnail = m_tabData.data()->thumbnail();

    QSizeF sh = size();
    qreal thumbheight = sh.height() * thumbToTextRatio - 3;
    qreal thumbwidth = thumbheight * (1 / thumbAspectRatio);

    painter->setBrush(Qt::white);
    QPen borderPen = QPen(Qt::lightGray);
    borderPen.setWidth(2);
    painter->setPen(borderPen);

    painter->drawRoundedRect(rect(), 3, 3);
    painter->drawPixmap(QRectF(3, 3, thumbwidth, thumbheight), thumbnail, thumbnail.rect());
    painter->setPen(Qt::black);

    if (painter->fontMetrics().width(title) > sh.width() - 6)
    {
        painter->drawText(QRectF(3, 3 + thumbheight, sh.width() - 3, sh.height() - thumbheight - 3), Qt::AlignLeft | Qt::AlignVCenter, title);
    }
    else
    {
        painter->drawText(QRectF(3, 3 + thumbheight, sh.width() - 3, sh.height() - thumbheight - 3), Qt::AlignCenter | Qt::AlignVCenter, title);
    }
}


QSizeF PreviewWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    QSizeF sh = constraint;

    switch (which)
    {
        case Qt::PreferredSize:
            if (constraint.width() > 0)
            {
                sh = QSizeF(constraint.width(), heightForWidth(constraint.width()));
            }
            else if(constraint.height() > 0)
            {
                sh = QSizeF(widthForHeight(constraint.height()), constraint.height());
            }
            else
            {
                sh = QSizeF(200,heightForWidth(200));
            }
            break;
        case Qt::MinimumSize:
            sh = QSizeF(100, heightForWidth(100));
            break;
        case Qt::MaximumSize:
            sh = QSizeF(400, heightForWidth(400));
            break;
        default:
            break;
    }
    return sh;
}


qreal PreviewWidget::widthForHeight(qreal height) const
{
    return (height*thumbToTextRatio)*(1/thumbAspectRatio);
}


qreal PreviewWidget::heightForWidth(qreal width) const
{
    return width*thumbAspectRatio*(1/thumbToTextRatio);
}

void PreviewWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    emit mousePressed();
    event->accept();
}


void PreviewWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length()
        < rApp->startDragDistance())
    {
        return;
    }

    if (!m_parent->session() || !m_parent->session()->isActive())
    {
        kDebug() << "cannot drag tab from deactivated session";
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setText("previewwidget");

    drag->setPixmap(m_tabData.data()->thumbnail().scaled(boundingRect().width(), boundingRect().height()));
    drag->setHotSpot(event->pos().toPoint());

    rApp->sessionManager()->panoramaScene()->setCurrentlyDragged(this);

    drag->exec();
}
