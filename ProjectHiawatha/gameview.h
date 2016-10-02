#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QPushButton>
#include <QWheelEvent>
#include <qtimeline.h>



class GameView : public QGraphicsView
{

public:
    GameView(QWidget *parent = 0, bool fullscreen = false);

    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon);
    QGraphicsPixmapItem* addPixmap(const QPixmap &pixmap);
    QGraphicsProxyWidget* addWidget(QWidget *widget);
    QGraphicsRectItem* addRect(QRect *rect, QPen pen, QBrush brush);
    QGraphicsTextItem* addText(QString text);

    QGraphicsScene* GetScene();
    void ConfigureGraphicsView();

    virtual void wheelEvent(QWheelEvent *e);

private:
    QGraphicsView gameView;
    QGraphicsScene *game;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
//    void paintEvent(QPaintEvent *e);

    QPointF clickedPos;

    int zoomScale;

protected:

public slots:
    void closeGame();
    void zoomIn();
    void zoomOut();
};

#endif // GAMEVIEW_H
