#include "gamescene.h"
#include <QDebug>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent)
{
    qDebug() << "Scene size: " << this->sceneRect().width() << "x" << this->sceneRect().height();
    isTileSelected = false;
    eventQueued = false;
    redrawTile = false;
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

void GameScene::ProcessTile(Map *map)
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

        column = mrScenePos.x() / 74;
        row = mrScenePos.y() / 44;

        qDebug() << "Pre modulo" <<"Col:" << column << "Row:" << row;

        if((column % 2 == 0) && (row % 2 != 0))
        {
            row--;
        }
        else if ((column % 2 != 0) && (row % 2 == 0))
        {
            row--;
        }

        qDebug() << "Post modulo" <<"Col:" << column << "Row:" << row;

        if(map->GetTileFromCoord(column, row)->HasCity)
        {
            //This may not be needed.
            map->GetTileFromCoord(column, row)->GetGoverningCity();
        }
        else if(map->GetTileFromCoord(column, row)->ContainsUnit)
        {
            // set the global isTileSelected flag
            isTileSelected = true;
        }

        // Tell GameManager that a tile needs to be redrawn.
        redrawTile = true;
    }
}

void GameScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
}
