#ifndef ACTIVATEBUTTON_H
#define ACTIVATEBUTTON_H

#include <QGraphicsWidget>
#include <QPixmap>

class QPainter;
class SessionWidget;

class ActivateButton : public QGraphicsWidget
{
    Q_OBJECT
public:
    ActivateButton(QGraphicsItem* parent = 0);
    virtual ~ActivateButton();
    void setActive(bool);

signals:
    void toggled();

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    QPixmap m_pixmap;
    bool m_active;
};

#endif //ACTIVATEBUTTON_H