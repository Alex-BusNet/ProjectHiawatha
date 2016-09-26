#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QWheelEvent>



class GameView : public QGraphicsView
{
public:
    GameView(QWidget *widget = 0, bool fullscreen = false);

    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon);
    QGraphicsPixmapItem* addPixmap(const QPixmap &pixmap);
    QGraphicsProxyWidget* addWidget(QWidget *widget);
    QGraphicsRectItem* addRect(QRect *rect, QPen pen, QBrush brush);
    QGraphicsTextItem* addText(QString text);

private:
    QGraphicsView gameView;
    QGraphicsScene *game;

    bool eventFilter(QObject *watched, QEvent *event);
    virtual void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *event);

    int zoomScale;

public slots:
    void closeGame();
    void zoomIn();
    void zoomOut();
};

#endif // GAMEVIEW_H
