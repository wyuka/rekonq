#include "stretcherwidget.h"
#include "stretcherwidget.moc"

#include "sessionwidget.h"

#include "rekonq_defines.h"

#include <QGraphicsWidget>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

StretcherWidget::StretcherWidget(SessionWidget *parent)
        : QGraphicsWidget(parent)
        , m_parent(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}


StretcherWidget::~StretcherWidget()
{
}


void StretcherWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setBrush(QColor(Qt::gray));
    painter->setPen(Qt::NoPen);
    static const QPointF points[3] = {
        boundingRect().topRight(),
        boundingRect().bottomRight(),
        boundingRect().bottomLeft()
    };
    painter->drawPolygon(points, 3);
    QGraphicsWidget::paint(painter, option, widget);
}

QSizeF StretcherWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QSizeF(15, 15);
}


void StretcherWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
}


void StretcherWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QSizeF size = m_parent->size();
    size.rwidth() += event->pos().x() - event->lastPos().x();
    size.rheight() += event->pos().y() - event->lastPos().y();
    m_parent->resize(size);
}
