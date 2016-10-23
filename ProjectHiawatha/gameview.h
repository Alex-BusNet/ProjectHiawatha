#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "gamescene.h"
#include "map.h"

#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QPushButton>
#include <QWheelEvent>

class GameView : public QGraphicsView
{

public:
    GameView(QWidget *parent = 0, bool fullscreen = false);

    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon);
    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon, const QPen &pen);
    QGraphicsPixmapItem* addPixmap(const QPixmap &pixmap);
    QGraphicsProxyWidget* addWidget(QWidget *widget);
    QGraphicsRectItem* addRect(QRect *rect, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    QGraphicsRectItem* addRect(int x, int y, int width, int height, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    QGraphicsTextItem* addText(QString text);


    void removeRect(QGraphicsRectItem* rect);

    GameScene *GetScene();
    void ConfigureGraphicsView();

    virtual void wheelEvent(QWheelEvent *e);

    void SetGameMap(Map *map);
    void SceneProcess(QPainter *paint);


private:
    QGraphicsView gameView;
    GameScene *game;

//    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
//    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void paintEvent(QPaintEvent *e);

    Map *map;

    QPointF clickedPos;

    int zoomScale;

protected:

public slots:
    void closeGame();
    void zoomIn();
    void zoomOut();
};

#endif // GAMEVIEW_H
