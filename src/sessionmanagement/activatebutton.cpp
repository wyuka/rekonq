#include "activatebutton.h"
#include "activatebutton.moc"

#include "sessionwidget.h"

#include "application.h"

#include <QGraphicsWidget>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <KIcon>

ActivateButton::ActivateButton(QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}


ActivateButton::~ActivateButton()
{
}


void ActivateButton::setActive(bool active)
{
    m_active = active;
    if (m_active)
    {
        m_pixmap = KIcon("media-playback-pause").pixmap(16, 16);
    }
    else
    {
        m_pixmap = KIcon("media-playback-start").pixmap(16, 16);
    }
}


void ActivateButton::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawPixmap(boundingRect(), m_pixmap, m_pixmap.rect());
    QGraphicsWidget::paint(painter, option, widget);
}

QSizeF ActivateButton::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QSizeF(16, 16);
}


void ActivateButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
}


void ActivateButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit toggled();
}