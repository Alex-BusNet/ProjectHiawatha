#include "gamescene.h"
#include "unitcontroller.h"
#include <qmath.h>
#include <QSet>
#include <QDebug>
#include <ctime>
#include <QTime>
UnitController::UnitController()
{

}

void UnitController::FindPath(Tile *startTile, Tile *endTile, Map *map, GameScene *scene, Unit *unit)
{
    if(startTile==endTile){
        qDebug()<<"Start = End";
        return;
    }
    qDebug() << "UnitController finding path";
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
            if(!neighbor->Walkable || map->setContains(closedSet, neighbor) || neighbor->ContainsUnit)
            {
                continue;
            }

            int newMoveCostToNeighbor = currentHex->gCost + GetDistance(currentHex, neighbor);

            if(newMoveCostToNeighbor < neighbor->gCost || !map->listContains(openSet, neighbor))
            {
                neighbor->gCost = newMoveCostToNeighbor;
                neighbor->hCost = GetDistance(neighbor, endTile);
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

void UnitController::MoveUnit(Unit *unit, Map *map, GameScene *scene, int civListIndex)
{
    if(!unit->isPathEmpty())
    {
        qDebug() << "   Clearing Tile Data";
        // Clear the data from the current tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
        map->GetTileAt(unit->GetTileIndex())->SetCivListIndex(-1);

        if(map->GetTileAt(unit->GetTileIndex())->Selected)
                map->GetTileAt(unit->GetTileIndex())->Selected = false;

        //// FOR DEBUGGING PURPOSES
        if(!unit->isPathEmpty())
        {
            qDebug() << "       Tiles in path:" << unit->GetPath().size();
//            foreach(Tile* tile, unit->GetPath())
//            {
//                qDebug() << "       " << tile->GetTileIDString();
//            }
        }

        qDebug() << "   Updating Position";
        //update the unit's position
        unit->SetPositionIndex((unit->GetPath().first()->GetTileID().column / 2) + (map->GetMapSizeX() * unit->GetPath().at(0)->GetTileID().row));
        unit->SetPosition(unit->GetPath().first()->GetTileID().column, unit->GetPath().first()->GetTileID().row);
        map->GetTileAt(unit->GetTileIndex())->SetCivListIndex(civListIndex);
        //=========================================
        // DAMAGE TESTING
        unit->SetHealth(unit->GetMaxHealth() / 2);
        //=========================================

        qDebug() << "   Setting new tile data";
        // Set the data for the unit's new tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

        qDebug() << "   Removing Point";
        // Remove the point from path
        if(!unit->isPathEmpty())
        {
            unit->UpdatePath();

            //// FOR DEBUGGING PURPOSES
//            qDebug() << "       Tiles left in path:" << unit->GetPath().size();
//            foreach(Tile* tile, unit->GetPath())
//            {
//                qDebug() << "       " << tile->GetTileIDString();
//            }
        }

        qDebug() << "   Redrawing Tile";
        // Alert the renderer to redraw the map.
        scene->redrawTile = true;
    }
}

void UnitController::Attack(Unit *attacker, Unit *target, bool attackFromWater)
{
    float waterPenalty = 0.0f, fortifyBonus = 0.0f;
    float AtkBonus, melee;

    qDebug() << "--Attacking";
    qDebug() << "   FromWater:" << attackFromWater;

    if(attackFromWater == true)
    {
        waterPenalty = 0.8f;
    }
    else
    {
        waterPenalty = 1.0f; // No penalty
    }

    qDebug() << "   waterPenalty:" << waterPenalty;

    if(target->isFortified)
        fortifyBonus = 0.4f;
    else
        fortifyBonus = 0.4f;

    qDebug() << "   forifyBonus:" << fortifyBonus;

    if(target->isMelee)
        melee = 1.0f;
    else
        melee = 0.0f;

    qDebug() << "   melee:" << melee;

    srand(QTime::currentTime().msec());

    AtkBonus = (static_cast<double>(rand())/ RAND_MAX) * 100.0;
    qDebug() << "   AtkBonus:" << AtkBonus;

    //Need to adjust this for range units attacking.
    qDebug() << "       Damage Dealt Attacker:" << (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty));
    qDebug() << "       Damage Dealt Target:" << ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));

    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty)) - ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));
//    //Deal minimum 1 damage for any attack.
    qDebug() << "   DamageDealt before adjust:" << damageDealt;
    damageDealt = damageDealt < 0 ? 1 : damageDealt > 100 ? damageDealt - 100 : damageDealt;
    float damageReceived = damageDealt * (fortifyBonus / AtkBonus) * melee;

    qDebug() << "           Damage Dealt:" << damageDealt << "Damage Recieved:" << damageReceived;

    target->SetHealth(target->GetHealth() - damageDealt);
    attacker->SetHealth(attacker->GetHealth() - damageReceived);
}

void UnitController::FoundCity(Unit *unit, Tile *CurrentTile, Civilization *currentCiv)
{
    if(unit->isSettler() && (CurrentTile->GetTileType() == (WATER | MOUNTAIN | ICE)) )
    {
       QVector <QString> tempCityList = currentCiv->GetInitialCityList();
       int index = currentCiv->getCityIndex();
       City* newCity = new City();
       newCity->SetName(tempCityList.at(index));
       newCity->SetCityIndex(currentCiv->GetCityList().size());
       currentCiv->AddCity(newCity);
    }else{
        //QMESSAGEBOX SAYING CANT FOUND CITY HERE
        //OR MAYBE PLAY A SOUND
    }
}

void UnitController::BuildImprovement(Unit *unit, Tile *currentTile, Civilization *currentCiv, TileImprovement improvement)
{
    if(unit->isNonCombat() && !unit->isSettler())
    {
        currentTile->SetTileImprovement(improvement);

        unit->Use();

        if(unit->GetRemainingUses() <= 0)
        {
            currentCiv->RemoveUnit(unit->GetUnitListIndex());
        }
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

    return new Unit(NO_NATION, WORKER);
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

    int i = 0;
    int j = path.size() - 1;

    while(i < j)
    {
        Tile* temp = path[i];
        path[i] = path[j];
        path[j] = temp;
        i++;
        j--;
    }

    //// This is for debugging purposes
    ///  Displays the path that the unit is to take;
//    qDebug() << "   Path:";
//    foreach(Tile* hex, path)
//    {
//          qDebug() << "   "<<hex->GetTileIDString();
//    }

    //This sets the path the unit needs to take.
    qDebug() << "   Setting unit path";
    unit->SetPath(path);
    qDebug() << "   Setting RequiresOrders to false";
    unit->RequiresOrders = false;
}
