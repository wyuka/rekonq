#ifndef REMOVEBUTTON_H
#define REMOVEBUTTON_H

#include <QGraphicsWidget>
#include <QPixmap>

class QPainter;
class SessionWidget;

class RemoveButton : public QGraphicsWidget
{
    Q_OBJECT
public:
    RemoveButton(QGraphicsItem* parent = 0);
    virtual ~RemoveButton();

signals:
    void clicked();

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    QPixmap m_pixmap;
};

#endif //REMOVEBUTTON_H