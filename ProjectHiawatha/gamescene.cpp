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
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    column = e->scenePos().x() / 74;
    row = e->scenePos().y() / 44;
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
        qDebug() << "Pre modulo" <<"Col:" << column << "Row:" << row;

        if((column % 2 == 0) && (row % 2 != 0))
        {
            row--;
        }

        qDebug() << "Post modulo" <<"Col:" << column << "Row:" << row;

        //Testing with Duel sized map
        qDebug() << "Getting Tile at index:" << (column / 2) + (40 * row);
        qDebug() << "newSelectedTile is null: " << (newSelectedTile == NULL);

        newSelectedTile = map->GetTileFromCoord(column, row);

         qDebug() << "ContainsUnit:" << newSelectedTile->ContainsUnit;

        ////Uncomment this once cities have been added.
        //    if(selectedTile->HasCity)
        //    {
        //        selectedTile->GetGoverningCity();
        //    }
            /*else*/if(newSelectedTile->ContainsUnit && !(newSelectedTile->Selected) && (lastSelectedTile != newSelectedTile))
            {
                if(lastSelectedTile != NULL)
                    lastSelectedTile->Selected = false;

                isTileSelected = true;
                newSelectedTile->Selected = true;
                lastSelectedTile = newSelectedTile;
                delete newSelectedTile;
            }
            else
            {
                isTileSelected = false;
                newSelectedTile->Selected = false;
                lastSelectedTile->Selected = false;
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
