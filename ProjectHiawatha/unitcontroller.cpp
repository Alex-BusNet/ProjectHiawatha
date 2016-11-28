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

void UnitController::FindPath(Tile *startTile, Tile *endTile, Map *map, Unit *unit, WarData wDat)
{
    qDebug() << " End tile has unit:" << endTile->ContainsUnit;

    if(startTile==endTile)
    {
        qDebug()<<"Start = End";
        return;
    }

    if(endTile->ContainsUnit && !unit->isPathEmpty())
    {
        endTile = unit->GetPath().at(unit->GetPath().size() - 2);
    }

    qDebug() << "UnitController finding path";
    QList<Tile*> openSet;
    QSet<Tile*> closedSet;
    Tile *currentHex;

    openSet.push_back(startTile);

    while(openSet.count() > 0)
    {
        currentHex = openSet[0];

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
            //Skip the tile if it is:
            //  -Impassable terrain
            //  -Has already been searched
            //  -Contains a unit
            //  -Is occupied or controlled by the civilization that the owner is at war with
            //      AND is not controlled/occupied by NO_NATION (-1)
            if(!neighbor->Walkable || map->setContains(closedSet, neighbor) || neighbor->ContainsUnit
                    || ((neighbor->GetCivListIndex() != wDat.warCivIndex
                        || neighbor->GetControllingCiv() != wDat.warringCiv)
                        && (wDat.warCivIndex != -1)
                        && (wDat.warringCiv != NO_NATION)))
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

    // This is for if the last tile is occupied and the algorithm ends
    // without finding the last tile.
    qDebug() << "----End tile not found; setting path to currentHex";
    RetracePath(startTile, currentHex, map, unit);
}

void UnitController::MoveUnit(Unit *unit, Map *map, int civListIndex)
{
    if(map->GetTileAt(unit->GetNextTileInPath()->GetTileIndex())->ContainsUnit)
    {
        qDebug() << "Next tile occupied;";
        unit->ClearPath();
        qDebug() << " Requesting New orders";
        unit->RequiresOrders = true;
        return;

    }

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
        unit->SetPositionIndex(unit->GetNextTileInPath()->GetTileIndex());
        unit->SetPosition(unit->GetNextTileInPath()->GetTileID().column, unit->GetNextTileInPath()->GetTileID().row);
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
    float AtkBonus = 1.5f, a_melee, t_melee;

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

    if(attacker->isMelee)
        a_melee = 1.0f;
    else
        a_melee = 0.0f;

    if(target->isMelee && attacker->isMelee)
        t_melee = 1.0f;
    else
        t_melee = 0.0f;

    qDebug() << "   a_melee:" << a_melee << "t_melee:" << t_melee;

    //Need to adjust this for range units attacking.
    qDebug() << "       Damage Dealt by Attacker:" << (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty));
    qDebug() << "       Damage Sustained by Target:" << ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));

    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus * waterPenalty));
    float damageSustained = ((target->GetHealth() / target->GetStrength()) + (target->GetStrength() * fortifyBonus));

    float damageReceived = (damageSustained) * (fortifyBonus / AtkBonus) * a_melee * t_melee;

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
    float AtkBonus, melee;

    if(attacker->isMelee)
    {
        AtkBonus = 0.8f;
        melee = 1.0f;
    }
    else if(attacker->isSiege)
    {
        AtkBonus = 4.0f;
        melee = 0.0f;
    }
    else
    {
        AtkBonus = 0.8f;
        melee = 0.0f;
    }
    qDebug() << "--City strength:" << city->GetCityStrength();
    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus));
    float damageSustained = (city->GetCityStrength() - damageDealt) * melee;

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

       currentCiv->AddCity(newCity);
       CurrentTile->HasCity = true;

    }else{
        //QMESSAGEBOX SAYING CANT FOUND CITY HERE
        //OR MAYBE PLAY A SOUND
    }
}

bool UnitController::BuildImprovement(Unit *unit, Tile *currentTile, Civilization *currentCiv, TileImprovement improvement)
{
    int gold = 0;
    int production = 0;
    int culture = 0;
    int science = 0;
    int food = 0;
    bool unitRemoved = false;
    switch (improvement) {
    case FARM:
        food = 1;
        break;
    case ROAD:
        break;
    case PLANTATION:
        food = 1;
        gold = 2;
        break;
    case MINE:
        production = 2;
        break;
    case TRADE_POST:
        gold = 2;
        break;
    default:
        break;
    }



    if((currentTile->GetTileImprovement()) == NONE)
    {
        currentTile->SetTileImprovement(improvement);
        currentTile->SetYield(gold, production, science, food, culture);
        unit->Use();

    }


    qDebug()<<"UNIT USES: "<<unit->GetRemainingUses();

        if(unit->GetRemainingUses() <= 0)
        {
            int unitIndex = unit->GetUnitListIndex();
            int unitIndexCopy = unitIndex;
            if(currentCiv->GetUnitList().size() > unitIndex)
            {
                for(unitIndexCopy; unitIndexCopy < currentCiv->GetUnitList().size();unitIndexCopy++)
                {
                    int oldIndex = currentCiv->GetUnitList().at(unitIndexCopy)->GetUnitListIndex();
                    currentCiv->GetUnitList().at(unitIndexCopy)->SetUnitListIndex(oldIndex-1);
                }
            }
            currentCiv->RemoveUnit(unitIndex);
            unitRemoved = true;

        }

        return unitRemoved;

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

    if(end->ContainsUnit)
    {
        qDebug() << "----Target tile contains unit; stopping one short";
        path.removeLast();
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
    unit->SetUnitTargetTileIndex(path.last()->GetTileIndex());
    qDebug() << "   Setting RequiresOrders to false";
    unit->RequiresOrders = false;
}
