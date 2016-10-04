#include "gamescene.h"
#include <QDebug>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent)
{
    qDebug() << "Scene size: " << this->sceneRect().width() << "x" << this->sceneRect().height();
    isTileSelected = false;
    eventQueued = false;
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    mpScenePos = e->scenePos();
    mpScreenPos = e->screenPos();
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    mrScenePos = e->scenePos();
    mrScreenPos = e->screenPos();
    eventQueued = true;
}


Tile *GameScene::GetSelectedTile()
{
    return lastSelectedTile;
}

void GameScene::ProcessTile(Map *map)
{
    if(eventQueued == true)
    {
        eventQueued = false;

        //==================================
        //
        //  If ((mpScreenPos != mrScreenPos) && (mrScenePos == lastPos))
        //      then drag occured
        //  else if ((mpScreenPos != lastScreenPos) && (mpScenePos != lastScenePos))
        //      then move unit command issued
        //  else if ((mrScreenPos == lastScreenPos) && (mrScenePos == lastScenePos))
        //      then column = mrScenePos.x() / 74;
        //           row = mrScenePos.y() / 44;
        //
        //           if (tileAt(column, row).hasCity)
        //               then tile.at(column,row).GetControllingCity().activateCityScreen();
        //           else if (tileAt(column, row).ContainsUnit)
        //               then tile.at(column, row).GetUnit().activateUnitControls();
        //
        //==================================


        column = mrScenePos.x() / 74;
        row = mrScenePos.y() / 44;

        qDebug() << "Pre modulo" <<"Col:" << column << "Row:" << row;

        if((column % 2 == 0) && (row % 2 != 0))
        {
            row--;
        }

        qDebug() << "Post modulo" <<"Col:" << column << "Row:" << row;

        newSelectedTile = map->GetTileFromCoord(column, row);

        if(newSelectedTile->HasCity)
        {
            qDebug() << "Tile has City";
            newSelectedTile->GetGoverningCity();
        }
        else if(newSelectedTile->ContainsUnit && !(newSelectedTile->Selected))
        {
            qDebug() << "Tile has Unit";
            if(lastSelectedTile != newSelectedTile)
                lastSelectedTile = newSelectedTile;

            if(lastSelectedTile != NULL)
                lastSelectedTile->Selected = false;

            isTileSelected = true;
//            newSelectedTile->Selected = true;
            delete newSelectedTile;
        }
        else
        {
            isTileSelected = false;
//            newSelectedTile->Selected = false;
//                lastSelectedTile->Selected = false;
            delete newSelectedTile;
        }
    }
}

void GameScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);

    if(isTileSelected)
    {
        painter->setPen(lastSelectedTile->GetTilePen());
        painter->drawPolygon(lastSelectedTile->GetTilePolygon());
    }
}
