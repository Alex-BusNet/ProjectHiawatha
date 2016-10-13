#include "gamescene.h"
#include "unitcontroller.h"
#include <qmath.h>
#include <QSet>
#include <QDebug>

UnitController::UnitController()
{

}

void UnitController::FindPath(Tile *startTile, Tile *endTile, Map *map, GameScene *scene, Unit *unit)
{
    QList<Tile*> openSet;
    QSet<Tile*> closedSet;

    openSet.push_back(startTile);

    while(openSet.count() > 0)
    {
        Tile *currentHex = openSet[0];

        for(int i = 1; i < openSet.count(); i++)
        {
            if(openSet.at(i)->fCost() < currentHex->fCost() || openSet.at(i)->fCost() == currentHex->fCost())
            {
                if((openSet.at(i)->hCost < currentHex->hCost))
                {
                    currentHex = openSet[i];
                }
            }
        }

        openSet.removeOne(currentHex);
        closedSet.insert(currentHex);

        if(currentHex == endTile)
        {
            RetracePath(startTile, endTile, map, unit);
            return;
        }

        QList<Tile*> neighborList = map->GetNeighbors(currentHex);

        foreach(Tile* neighbor, neighborList)
        {
            if(!neighbor->Walkable || map->setContains(closedSet, neighbor))
            {
                continue;
            }

            int newMoveCostToNeighbor = currentHex->gCost + GetDistance(currentHex, neighbor);

            if(newMoveCostToNeighbor < neighbor->gCost || !map->listContains(openSet, neighbor))
            {
                neighbor->gCost = newMoveCostToNeighbor;
                neighbor->hCost = GetDistance(neighbor, endTile);
                //This needs to be set.
                neighbor->parent = currentHex->GetTileID();

                if(!(map->listContains(openSet, neighbor)))
                {
                    openSet.push_back(neighbor);
                }
            }
        }
    }

    scene->unitMoveOrdered = false;
}

void UnitController::MoveUnit(Unit *unit, Map *map, GameScene *scene)
{
    if(!unit->isPathEmpty())
    {
        qDebug() << "   Clearing Data";
        // Clear the data from the current tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;

        if(map->GetTileAt(unit->GetTileIndex())->Selected)
                map->GetTileAt(unit->GetTileIndex())->Selected = false;

        qDebug() << "   Updating Position";
        //update the unit's position
        unit->SetPosition((unit->GetPath().at(0)->GetTileID().column / 2) + (map->GetMapSizeX() * unit->GetPath().at(0)->GetTileID().row));

        qDebug() << "   Setting new tile data";
        // Set the data for the unit's new tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

        qDebug() << "   Removing Point";
        // Remove the point from path
        unit->GetPath().removeAt(0);

        qDebug() << "   Redrawing Tile";
        // Alert the renderer to redraw the map.
        scene->redrawTile = true;
    }
    else
    {

    }
}

Unit* UnitController::FindUnitAtTile(Tile *tile, Map *map, QVector<Unit *> unitList)
{
    int tIndex = (tile->GetTileID().column / 2) + (map->GetMapSizeX() * tile->GetTileID().row);
    foreach(Unit* unit, unitList)
    {
        qDebug() << "           Unit belongs to:" << unit->GetOwner();
        if(unit->GetTileIndex() == tIndex)
        {
            return unit;
        }
    }
}

int UnitController::GetDistance(Tile *a, Tile *b)
{
    int dstX = b->GetTileID().column - a->GetTileID().column;
    int dstY = b->GetTileID().row - a->GetTileID().row;

    if(std::signbit(dstX) == std::signbit(dstY))
    {
        return std::max(std::abs(dstX), std::abs(dstY));
    }
    else
    {
        return (abs(dstX) + abs(dstY));
    }
}

void UnitController::RetracePath(Tile *start, Tile *end, Map *map, Unit *unit)
{
    QList<Tile*> path;
    Tile *current = end;

    do
    {
        path.push_back(current);
        current = map->GetTileAt((current->parent.column / 2) + (map->GetMapSizeX() * current->parent.row));
    }
    while(current != start);

//    path.push_back(start);

    int i = 0;
    int j = path.size() - 1;

    while(i < j)
    {
//        path[i]->inPath = true;
//        path[j]->inPath = true;

        Tile* temp = path[i];
        path[i] = path[j];
        path[j] = temp;
        i++;
        j--;
    }

    //// This is for debugging purposes
    ///  Displays the path that the unit is to take;
    qDebug() << "   Path:";
    foreach(Tile* hex, path)
    {
          qDebug() << "   "<<hex->GetTileIDString();
    }

    //This sets the path the unit needs to take.
    qDebug() << "   Setting unit path";
    unit->SetPath(path);
    qDebug() << "   Setting RequiresOrders to false";
    unit->RequiresOrders = false;
}
