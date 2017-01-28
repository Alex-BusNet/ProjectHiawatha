#include "gamescene.h"
#include <QDebug>
#include <qmath.h>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent)
{
    isTileSelected = false;
    eventQueued = false;
    redrawTile = false;
    unitMoveOrdered = false;
    citySelected = false;
    findUnit = false;

    processedData.column = 0;
    processedData.row = 0;
    processedData.newData = false;
    processedData.relocateOrderGiven = false;
}

GameScene::~GameScene()
{

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

/*
 * ProcessTile is used to find the player clicked on. This function
 * is not 100% accurate, but tends to be the most accurate if a tile
 * is the area where a unit icon would normally be drawn.
 */
TileData GameScene::ProcessTile(bool unitAwaitingRelocation)
{
    if(eventQueued == true)
    {
        eventQueued = false;

        if((mpScreenPos != mrScreenPos) && (mpScenePos == mrScenePos))
        {
            // Drag Occured
            processedData.newData = false;
            processedData.relocateOrderGiven = false;
        }
        else if((mpScreenPos != lastScreenPos) && (mpScenePos != lastScenePos) && unitAwaitingRelocation)
        {
            // Move unit command Issued
            column = (mrScenePos.x() / 44);
            row = (mrScenePos.y() / 75);

            qDebug() << "       Tile selected before adjust:" << column << "," << row;

            if((column % 2 == 0) && (row % 2 != 0))
            {
                column--;
            }
            else if ((column % 2 != 0) && (row % 2 == 0))
            {
                column--;
            }

            qDebug() << "       Tile selected:" << column << "," << row;

            processedData.column = column;
            processedData.row = row;
            processedData.newData = true;
            processedData.relocateOrderGiven = true;

            lastScreenPos = mrScreenPos;
            lastScenePos = mrScenePos;
        }
        else if((mpScreenPos == mrScreenPos) && (mpScenePos == mrScenePos) && !unitAwaitingRelocation)
        {
            // Tile was Selected

            column = (mrScenePos.x() / 44);
            row = (mrScenePos.y() / 74);

             qDebug() << "       Tile selected before adjust:" << column << "," << row;

            if((column % 2 == 0) && (row % 2 != 0))
            {
                column--;
            }
            else if ((column % 2 != 0) && (row % 2 == 0))
            {
                column--;
            }

            qDebug() << "       Tile selected:" << column << "," << row;

            processedData.column = column;
            processedData.row = row;
            processedData.newData = true;
            processedData.relocateOrderGiven = false;

            lastScreenPos = mrScreenPos;
            lastScenePos = mrScenePos;
        }

        //Save the last mouse release position values

    }
    else
    {
        processedData.newData = false;
        processedData.relocateOrderGiven = false;
    }

    return processedData;
}
