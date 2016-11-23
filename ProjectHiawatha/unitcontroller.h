#ifndef UNITCONTROLLER_H
#define UNITCONTROLLER_H

#include "map.h"
#include <QList>
#include "tile.h"
#include "unit.h"
#include "city.h"
#include "civilization.h"
#include "gamescene.h"

class UnitController
{
public:
    UnitController();

    void FindPath(Tile *startTile, Tile* endTile, Map *map, Unit *unit);
    void MoveUnit(Unit *unit, Map *map, int civListIndex);

    void Attack(Unit* attacker, Unit* target, bool attackFromWater);
    void RangeAttack(Unit* attacker, Unit* target);
    void AttackCity(Unit* attacker, City* city);

    void FoundCity(Unit* unit, Tile* CurrentTile, Civilization* currentCiv);
    bool BuildImprovement(Unit* unit, Tile* currentTile, Civilization* currentCiv, TileImprovement improvement);

    void HealUnit(Unit* unit);

    Unit *FindUnitAtTile(Tile* tile, Map *map, QVector<Unit *> unitList);
    City *FindCityAtTile(Tile *tile, Map *map, QVector<City*> cityList);

    QString NationName(Nation nation)
    {
        switch(nation)
        {
        case America:
            return QString("America");
        case Germany:
            return QString("Germany");
        case India:
            return QString("India");
        case China:
            return QString("China");
        case Mongolia:
            return QString("Mongolia");
        case Aztec:
            return QString("Aztec");
        case France:
            return QString("France");
        case Iroquois:
            return QString("Iroquois");
        case Greece:
            return QString("Greece");
        case Rome:
            return QString("Rome");
        case England:
            return QString("England");
        case Arabia:
            return QString("Arabia");
        case Persia:
            return QString("Persia");
        case Russia:
            return QString("Russia");
        case Japan:
            return QString("Japan");
        case Egypt:
            return QString("Egypt");
        default:
            return QString("No Nation");
        }
    }

private:
    int GetDistance(Tile* a, Tile *b);
    void RetracePath(Tile *start, Tile *end, Map *map, Unit *unit);

};

#endif // UNITCONTROLLER_H
