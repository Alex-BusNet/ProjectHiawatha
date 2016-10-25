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

void GameScene::ProcessTile(Map *map, bool unitAwaitingRelocation)
{
    if(eventQueued == true)
    {
        eventQueued = false;

        //==================================
        // Mouse input filter function:
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

        if((mpScreenPos != mrScreenPos) && (mpScenePos == mrScenePos))
        {
            // Drag Occured
            qDebug() << "Drag Screen";
        }
        else if((mpScreenPos != lastScreenPos) && (mpScenePos != lastScenePos) && unitAwaitingRelocation)
        {
            // Move unit command Issued
            qDebug() << "Move Unit";
            column = floor(mrScenePos.x() / 46);
            row = floor(mrScenePos.y() / 76);

            qDebug() << "   Before Adjust: " << column << "," << row;

            if((column % 2 == 0) && (row % 2 != 0))
            {
//                row--;
                column--;
            }
            else if ((column % 2 != 0) && (row % 2 == 0))
            {
//                row--;
                column--;
            }

            qDebug() << "   After Adjust: " << column << "," << row;

            unitTargetTile = map->GetTileFromCoord(column, row);
            unitMoveOrdered = true;

        }
        else if((mpScreenPos == mrScreenPos) && (mpScenePos == mrScenePos) && !unitAwaitingRelocation)
        {
            // Tile was Selected
            qDebug() << "Tile selected";
            qDebug() << "ScenePos: " << mrScenePos;

            column = floor(mrScenePos.x() / 44);
            row = floor(mrScenePos.y() / 75);

            qDebug() << "   Before Adjust: " << column << "," << row;

            if((column % 2 == 0) && (row % 2 != 0))
            {
//                row--;
                column--;
            }
            else if ((column % 2 != 0) && (row % 2 == 0))
            {
//                row--;
                column--;
            }

            qDebug() << "   After Adjust: " << column << "," << row;

            if(map->GetTileFromCoord(column, row)->HasCity)
            {
                //This may not be needed.
                citySelected = true;
                cityTileSelected = map->GetTileFromCoord(column, row);
            }
            else if(map->GetTileFromCoord(column, row)->ContainsUnit)
            {
                // set the global isTileSelected flag
                qDebug() << "Unit tile selected";
                unitSelectedTile = map->GetTileFromCoord(column, row);
                isTileSelected = true;
            }

            // Tell GameManager that a tile needs to be redrawn.
            redrawTile = true;
        }

        //Save the last mouse release position values
        lastScreenPos = mrScreenPos;
        lastScenePos = mrScenePos;
    }
}

void GameScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
}
