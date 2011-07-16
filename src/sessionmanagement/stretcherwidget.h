#ifndef STRETCHERWIDGET_H
#define STRETCHERWIDGET_H

#include <QGraphicsWidget>

class QPainter;
class QGraphicsSceneMoveEvent;
class SessionWidget;

class StretcherWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    StretcherWidget(SessionWidget* parent = 0);
    virtual ~StretcherWidget();

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    SessionWidget *m_parent;
};

#endif //STRETCHERWIDGET_H