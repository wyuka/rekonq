#include "removebutton.h"
#include "removebutton.moc"

#include "sessionwidget.h"

#include "application.h"

#include <QGraphicsWidget>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <KIcon>

RemoveButton::RemoveButton(QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pixmap = KIcon("edit-delete").pixmap(16, 16);
}


RemoveButton::~RemoveButton()
{
}


void RemoveButton::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawPixmap(boundingRect(), m_pixmap, m_pixmap.rect());
    QGraphicsWidget::paint(painter, option, widget);
}

QSizeF RemoveButton::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QSizeF(16, 16);
}


void RemoveButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
}


void RemoveButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit clicked();
}