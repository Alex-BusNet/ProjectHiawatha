#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QWheelEvent>
#include "tile.h"
#include "datatypes.h"

class GameScene : public QGraphicsScene
{
public:
    GameScene(QObject *parent = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    TileData ProcessTile(bool unitAwaitingRelocation);

    bool isTileSelected, eventQueued, redrawTile, unitMoveOrdered, citySelected, findUnit;

    int column, row;
    Tile* unitTargetTile, *unitSelectedTile, *cityTileSelected;

private:

    TileData processedData;
    QPointF mpScenePos, mpScreenPos, mrScenePos, mrScreenPos, lastScenePos, lastScreenPos;

};

#endif // GAMESCENE_H
