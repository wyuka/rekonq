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
    painter->setPen(m_color);
    painter->setBrush(m_color);
    for (int i = 0; i <= 3; ++i)
    {
        for (int j = 3; j >= 3 - i; --j)
        {
            painter->drawEllipse(QRectF(j * 5, i * 5, 1.2, 1.2));
        }
    }
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
