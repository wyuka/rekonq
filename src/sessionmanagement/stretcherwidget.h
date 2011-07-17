#ifndef STRETCHERWIDGET_H
#define STRETCHERWIDGET_H

#include <QGraphicsWidget>
#include <QColor>

class QPainter;
class QGraphicsSceneMoveEvent;
class SessionWidget;

class StretcherWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    StretcherWidget(SessionWidget* parent = 0);
    virtual ~StretcherWidget();

    inline void setColor(QColor color)
    {
        m_color = color;
    }
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    SessionWidget *m_parent;
    QColor m_color;
};

#endif //STRETCHERWIDGET_H