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

/*
 * FindPath, in conjunction with GetNeighbors, GetDistance,
 * and RetracePath, uses the A* pathfinding algorithm to determine the
 * most efficient path for a unit to reach the target tile while avoiding
 * obstacles on the map. The A* pathfinding algorithm uses a heuristic cost system
 * and by calculating 'cost' to move to an adjacent tile (stored as gCost) and the distance
 * of the adjacent tile to the target tile (stored as hCost), and the summation of
 * the two (known as fCost). The algorithm determines what direction it travels by looking at
 * the fCost of all surrounding tiles and choosing the lowest one, if there is a tie in lowest
 * fCosts, then the lower hCost of the two is used, if hCosts tie, then one of the two is chosed
 * and the algorithm proceeds. Tiles that are determined to be impassable (indicated by a tile's walkable flag or the AtWar check)
 * then a gCost and hCost are not calculated for that tile. As tiles are searched, any tile that is determined
 * to be a valid tile, and has not already been searched, is added to the openSet, as tiles are checked, they are
 * added to the closedSet. As new tiles are searched and are determined to be valid options, the neighboring
 * tiles' parent attribute is set to the current tile. Tiles that have been determined to be part of the path will
 * always a higher fCost, preventing the algorithm from revistiing a tile it has already visited. Once FindPath
 * reaches it's destination the end tile and start tile are sent to RetracePath.
 */
void UnitController::FindPath(Tile *startTile, Tile *endTile, Map *map, Unit *unit, WarData wDat)
{
    bool warCheckFailed = false;
    if(startTile==endTile)
    {
        return;
    }

    if(endTile->ContainsUnit && !unit->isPathEmpty())
    {
        endTile = unit->GetPath().at(unit->GetPath().size() - 2);
    }

    QList<Tile*> openSet;
    QSet<Tile*> closedSet;
    Tile *currentHex;

    openSet.push_back(startTile);

    while(openSet.count() > 0)
    {
        currentHex = openSet[0];

        for(int i = 1; i < openSet.count(); i++)
        {
            // Compare the fCost of the current tile to the fCost of
            // the tile at index i of the openSet. CurrentHex is always
            // the tile at index 0 of the openSet.
            if(openSet.at(i)->fCost() < currentHex->fCost() || openSet.at(i)->fCost() == currentHex->fCost())
            {
                if((openSet.at(i)->hCost < currentHex->hCost))
                {
                    currentHex = openSet[i];
                }
            }
        }

        // Remove the tile from the open set
        // and add it to the closed set
        openSet.removeOne(currentHex);
        closedSet.insert(currentHex);

        // If we haved reached our destination,
        // Quit searching and retrace the path
        // back to the start.
        if(currentHex == endTile)
        {
            RetracePath(startTile, endTile, map, unit);
            return;
        }

        // Get the tiles adjacent to currentHex
        QList<Tile*> neighborList = map->GetNeighbors(currentHex);

        foreach(Tile* neighbor, neighborList)
        {
            //Skip the tile if it
            //  -Is impassable terrain
            //  -Has already been searched
            //  -Contains a unit
            //  -Is occupied or controlled by the civilization that the owner is not at war with
            //      AND is not controlled/occupied by NO_NATION (-1)
            if(neighbor->GetControllingCiv() != unit->GetOwner())
            {
                warCheckFailed = this->AtPeaceWith(neighbor, wDat);
            }
            else
            {
                warCheckFailed = false;
            }

            if(!neighbor->Walkable || map->setContains(closedSet, neighbor) || neighbor->ContainsUnit || warCheckFailed)
            {
                warCheckFailed = false;
                continue;
            }

            //Calculate the cost of moving to the neighboring tile.
            int newMoveCostToNeighbor = currentHex->gCost + GetDistance(currentHex, neighbor);

            // Check if the the cost to move to the neighbor is lower than the tile's gCost
            // or the tile is not already in the openSet.
            if(newMoveCostToNeighbor < neighbor->gCost || !map->listContains(openSet, neighbor))
            {
                // Set the gCost of the neighbor to the calculated move cost.
                neighbor->gCost = newMoveCostToNeighbor;
                // update the hCost of the tile to our destination
                neighbor->hCost = GetDistance(neighbor, endTile);
                // Set the parent of the neighbor tile to the current tile
                neighbor->parent = currentHex->GetTileID();

                // If the tile is not in the open set, add it.
                if(!(map->listContains(openSet, neighbor)))
                {
                    openSet.push_back(neighbor);
                }
            }
        }
    }
}

void UnitController::MoveUnit(Unit *unit, Map *map, int civListIndex)
{
    if(map->GetTileAt(unit->GetNextTileInPath()->GetTileIndex())->ContainsUnit)
    {
        unit->ClearPath();
        unit->RequiresOrders = true;
        return;

    }

    if(!unit->isPathEmpty())
    {
        // Clear the data from the current tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
        map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);

        if(map->GetTileAt(unit->GetTileIndex())->Selected)
                map->GetTileAt(unit->GetTileIndex())->Selected = false;

        //update the unit's position
        unit->SetPositionIndex(unit->GetNextTileInPath()->GetTileIndex());
        unit->SetPosition(unit->GetNextTileInPath()->GetTileID().column, unit->GetNextTileInPath()->GetTileID().row);
        map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(civListIndex);

        // Set the data for the unit's new tile
        map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

        // Remove the point from path
        if(!unit->isPathEmpty())
        {
            unit->UpdatePath();
        }
    }
}

/*
 * Attack is used for unit-to-unit combat. Attackers are granted a 1.5 bonus
 * to their attack, this is to prevent two units of the same type from dealing
 * equal damage to each other (assuming both the attacker and target are melee units).
 * a 40% reduction in damge taken is granted to the target if they are fortified
 * and attackers will only deal 80% of the normal damage dealt if they attack from water.
 * if the attacker, target, or both are ranged type units, then the attacking unit will
 * not take any damage. However, if both units are melee type, then the attacker will
 * recieve some damage equal to a ratio of the target's current health to their overall strength
 * mulitplied by the ratio of the fortify bonus over the attack bonus.
 */
void UnitController::Attack(Unit *attacker, Unit *target, bool attackFromWater)
{
    float damageDealt, damageReceived;

    float csr, mdr, percentReduced = 0.0;
    if(attacker->GetStrength() > target->GetStrength())
    {
        csr = attacker->GetStrength() / target->GetStrength();
    }
    else
    {
        csr = target->GetStrength() / attacker->GetStrength();
    }

    for(int i = 100 - attacker->GetHealth(); i > 0; i--)
    {
        percentReduced += 0.005f;
    }

    damageDealt = 6.66*pow(csr, 2) + 3.33*csr + 20;

    if(percentReduced > 0.0f)
    {
        damageDealt -= (damageDealt * percentReduced);
    }

    if(target->isFortified)
    {
        damageDealt -= (damageDealt * 0.4);
    }

    if(attackFromWater)
    {
        damageDealt *= 0.8;
    }

    if(target->isMelee && attacker->isMelee)
    {
        mdr = 0.05*pow(csr, 3) - 0.49*pow(csr, 2) + 1.58*csr - 0.25;
        damageReceived = damageDealt * mdr;
    }
    else
    {
        damageReceived = 0.0f;
    }

    qDebug() << "Damage dealt:" << damageDealt << "Damage received:" << damageReceived;

    target->DealDamage(damageDealt);
    attacker->DealDamage(damageReceived);
    attacker->RequiresOrders = false;
}

/*
 * AttackCity functions similarly to Attack however there is no damage received calulation.
 * Melee units that attack a city will automatically recieve a 80% damage penalty for attacking
 * cities and also will take on damage for attacking. If a unit is classified as siege type, then it will receive a 400% bouns to the
 * damage it deals to the city. Range units do not receive any bonus or penalty for attacking cities.
 */
void UnitController::AttackCity(Unit *attacker, City *city)
{
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
    else if(!attacker->isMelee)
    {
        AtkBonus = 1.0f;
        melee = 0.0f;
    }
    else if(attacker->isNonCombat())
    {
        return;
    }

    float damageDealt = (((attacker->GetHealth() / attacker->GetStrength()) * AtkBonus));
    float damageSustained = (city->GetCityStrength() - damageDealt) * melee;

    qDebug() << "Damage dealt to city:" << damageDealt << "damage sustained:" << damageSustained;

    city->SetCityHealth(city->GetCityHealth() - damageDealt);
    attacker->DealDamage(damageSustained);
    attacker->RequiresOrders = false;
}

/*
 * FoundCity is used to set up premilimary information when a player tries to found a city.
 */
void UnitController::FoundCity(Unit *unit, Tile *CurrentTile, Civilization *currentCiv)
{
    if(unit->isSettler() && (CurrentTile->GetTileType() == (WATER | MOUNTAIN | ICE)) )
    {
       City* newCity = new City();
       currentCiv->AddCity(newCity);
       CurrentTile->HasCity = true;

    }else{
        //QMESSAGEBOX SAYING CANT FOUND CITY HERE
        //OR MAYBE PLAY A SOUND
    }
}

/*
 *
 */
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
        food = 1;//FARM IMPROVES FOOD YIELD BY 1
        break;
    case ROAD:
        break;//ROAD SERVES NO PURPOSE AT THIS TIME
    case PLANTATION:
        food = 1;//PLANTATION INCREASES FOOD YIELD BY 1
        gold = 2;//PLANTATION INCREASES GOLD YIELD BY 2
        break;
    case MINE:
        production = 2;//MINE INCREASES PRODUCTION YIELD BY 2
        break;
    case TRADE_POST:
        gold = 2;//TRADING POST INCREASES GOLD YIELD BY 2
        break;
    default:
        break;
    }
    //If current tile has no improvement then build the improvement
    if((currentTile->GetTileImprovement()) == NONE)
    {
        currentTile->SetTileImprovement(improvement);//set this tile to have the improvement specified
        currentTile->SetYield(gold, production, science, food, culture);//update the yield to reflect new changes
        unit->Use();//use up the unit

    }

    if(unit->GetRemainingUses() <= 0)
    {
        int unitIndex = unit->GetUnitListIndex();//get the correct unit index
        currentCiv->RemoveUnit(unitIndex);//Remove the unit
        unitRemoved = true;//set unit removed flag to true

    }

    return unitRemoved;

}

/*
 * FindUnitAtTile searces the supplied unit list and searches the list until the
 * tile index matches the unit's tile index.
 */
Unit* UnitController::FindUnitAtTile(Tile *tile, QVector<Unit *> unitList)
{
    int tIndex = tile->GetTileIndex();
    foreach(Unit* unit, unitList)
    {
        if(unit->GetTileIndex() == tIndex)
        {
            return unit;
        }
    }

    return new Unit(NO_NATION, WORKER);
}

/*
 * FindCityAtTile functions identically to FindUnitAtTile, only taking a City object in
 * place of a Unit object.
 */
City *UnitController::FindCityAtTile(Tile *tile, QVector<City *> cityList)
{
    int tIndex = tile->GetTileIndex();
    int cIndex;
    foreach(City* city, cityList)
    {
        cIndex = (city->GetCityTile()->GetTileIndex());
        if(cIndex == tIndex)
        {
            return city;
        }
    }

    return new City();
}

/*
 * If a unit has taken damage, then HealUnit will replenish 10 HP
 * per turn untill their health is at max.
 */
void UnitController::HealUnit(Unit *unit)
{
    if((unit->GetHealth() + 10) > 100)
        unit->SetHealth(100);
    else
        unit->SetHealth(unit->GetHealth() + 10);
}

/*
 * Returns TRUE if the passed tile
 * is not controlled by NO_NATION and
 * not controlled by the civilization that
 * is passed in the WarData.
 * Also returns TRUE if the passed tile is not
 * occupied by NO_NATION (-1) and is not occupied
 * by the civListIndex passed in WarData
 */
bool UnitController::AtPeaceWith(Tile *target, WarData wDat)
{
    if(target->GetControllingCiv() != NO_NATION)
    {
        if(target->GetControllingCiv() != wDat.warringCiv)
        {
            return true;
        }
    }
    else if(target->GetOccupyingCivListIndex() != -1)
    {
        if(target->GetOccupyingCivListIndex() != wDat.warCivIndex)
        {
            return true;
        }
        else if(target->GetControllingCivListIndex() != wDat.warCivIndex)
        {
            return true;
        }
    }

    // This should only return false for non-controlled tiles
    // i.e. NO_NATION, -1
    return false;
}

/*
 * GetDistance calculates the distance, in pixels between two tiles.
 * This distance is an approximate value and does not follow true
 * mathmatical formulas for calculating distance.
 */
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

/*
 * Retrace is the final piece of the A* pathfinding algorithm. This part starts by setting the current
 * tile to the end tile, and then adds the current tile to the path. The current tile is updated to the
 * parent of that tile and continues until the start tile has been reached. Since this list is in reverse
 * order, we set to index variables, one set to 0 and the other set to path size - 1, and swap the tiles
 * at each index. After each swap, the first index is incremented and the second index is decremented by one.
 * This continues until the first index is equal to the second index. Once the path has been correctly
 * organized, it is loaded into the unit.
 */
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
        path.removeLast();
    }

    //This sets the path the unit needs to take.
    unit->SetPath(path);
    unit->SetUnitTargetTileIndex(path.last()->GetTileIndex());
    unit->RequiresOrders = false;
}
