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

TileData GameScene::ProcessTile(bool unitAwaitingRelocation)
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
            processedData.newData = false;
        }
        else if((mpScreenPos != lastScreenPos) && (mpScenePos != lastScenePos) && unitAwaitingRelocation)
        {
            // Move unit command Issued
            qDebug() << " Move Unit";
            column = 0.5 + (mrScenePos.x() / 45);
            row = 0.5 + (mrScenePos.y() / 75);

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

            processedData.column = column;
            processedData.row = row;
            processedData.newData = true;
            processedData.relocateOrderGiven = true;
        }
        else if((mpScreenPos == mrScreenPos) && (mpScenePos == mrScenePos) && !unitAwaitingRelocation)
        {
            // Tile was Selected
            qDebug() << "Tile selected";
            qDebug() << "ScenePos: " << mrScenePos;

            column = 0.5 + (mrScenePos.x() / 45);
            row = 0.5 + (mrScenePos.y() / 75);

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
            processedData.column = column;
            processedData.row = row;
            processedData.newData = true;
            processedData.relocateOrderGiven = false;
        }

        //Save the last mouse release position values
        lastScreenPos = mrScreenPos;
        lastScenePos = mrScenePos;
    }
    else
    {
        processedData.newData = false;
        processedData.relocateOrderGiven = false;
    }

    return processedData;
}

void GameScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
}
