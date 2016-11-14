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
//    qDebug() << "UnitController finding path";
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

void UnitController::MoveUnit(Unit *unit, Map *map, int civListIndex)
{
    if(!unit->isPathEmpty())
    {
        // Clear the data from the current tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
        map->GetTileAt(unit->GetTileIndex())->SetCivListIndex(-1);

        if(map->GetTileAt(unit->GetTileIndex())->Selected)
                map->GetTileAt(unit->GetTileIndex())->Selected = false;

        //// FOR DEBUGGING PURPOSES
        if(!unit->isPathEmpty())
        {
            qDebug() << "       Tiles in path:" << unit->GetPath().size();
        }

        //update the unit's position
        unit->SetPositionIndex((unit->GetPath().first()->GetTileID().column / 2) + (map->GetMapSizeX() * unit->GetPath().at(0)->GetTileID().row));
        unit->SetPosition(unit->GetPath().first()->GetTileID().column, unit->GetPath().first()->GetTileID().row);
        map->GetTileAt(unit->GetTileIndex())->SetCivListIndex(civListIndex);

        // Set the data for the unit's new tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

        // Remove the point from path
        if(!unit->isPathEmpty())
        {
            unit->UpdatePath();
        }
    }
}

void UnitController::Attack(Unit *attacker, Unit *target, bool attackFromWater)
{
    float waterPenalty = 0.0f, fortifyBonus = 0.0f;
    float AtkBonus = 1.5f, melee;

    qDebug() << "--Attacking";
    qDebug() << "   Attacker belongs to:" << NationName(attacker->GetOwner()) << "Target belongs to:" << NationName(target->GetOwner());
    qDebug() << "   FromWater:" << attackFromWater;

    if(attackFromWater == true)
        waterPenalty = 0.8f;
    else
        waterPenalty = 1.0f; // No penalty

    qDebug() << "   waterPenalty:" << waterPenalty;

    if(target->isFortified)
        fortifyBonus = 0.4f;
    else
        fortifyBonus = 1.0f;

    qDebug() << "   forifyBonus:" << fortifyBonus;

    if(target->isMelee)
        melee = 1.0f;
    else
        melee = 0.0f;

    qDebug() << "   melee:" << melee;

    //Need to adjust this for range units attacking.
    qDebug() << "       Damage Dealt by Attacker:" << (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty));
    qDebug() << "       Damage Sustained by Target:" << ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));

    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty));
    float damageSustained = ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));

    float damageReceived = (damageSustained) * (fortifyBonus / AtkBonus) * melee;

    qDebug() << "           Damage taken by target:" << damageDealt << "Damage Recieved by attacker:" << damageReceived;

    target->DealDamage(damageDealt);
    attacker->DealDamage(damageReceived);
    attacker->RequiresOrders = false;
}

void UnitController::RangeAttack(Unit *attacker, Unit *target)
{

}

void UnitController::AttackCity(Unit *attacker, City *city)
{
    qDebug() << "targetCity belongs to:" << NationName(city->GetControllingCiv());
    float AtkBonus = 1.2f, melee;

    if(attacker->isMelee)
        melee = 1.0f;
    else
        melee = 0.0f;

    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus));
    float damageSustained = ((city->GetCityHealth() / city->GetCityStrength()) * melee);

    qDebug() << "           Damage taken by city:" << damageDealt << "Damage sustained by attacker:" << damageSustained;

    city->SetCityHealth(city->GetCityHealth() - damageDealt);
    attacker->DealDamage(damageSustained);
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
        if(unit->GetTileIndex() == tIndex)
        {
            qDebug() << "           Unit belongs to:" << this->NationName(unit->GetOwner());
            return unit;
        }
    }

    return new Unit(NO_NATION, WORKER);
}

City *UnitController::FindCityAtTile(Tile *tile, Map* map, QVector<City *> cityList)
{
    int tIndex = (tile->GetTileID().column / 2) + (map->GetMapSizeX() * tile->GetTileID().row);
    int cIndex;
    foreach(City* city, cityList)
    {
        cIndex = (city->GetCityTile()->GetTileID().column / 2) + (map->GetMapSizeX() * city->GetCityTile()->GetTileID().row);
        if(cIndex == tIndex)
        {
            return city;
        }
    }

    return new City();
}

void UnitController::HealUnit(Unit *unit)
{
    qDebug() << "Healing unit";
    if((unit->GetHealth() + 10) > 100)
        unit->SetHealth(100);
    else
        unit->SetHealth(unit->GetHealth() + 10);
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
