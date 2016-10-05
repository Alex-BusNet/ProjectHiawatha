#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "map.h"

#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QWheelEvent>

class GameScene : public QGraphicsScene
{
public:
    GameScene(QObject *parent = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    void SetMap(Map *map);
    void ProcessTile(Map *map);

    bool isTileSelected, eventQueued, redrawTile;

    void drawForeground(QPainter *painter, const QRectF &rect);

    int column, row;
private:

    QPointF mpScenePos, mpScreenPos, mrScenePos, mrScreenPos, lastScenePos, lastScreenPos;

};

#endif // GAMESCENE_H
