#ifndef UNITCONTROLLER_H
#define UNITCONTROLLER_H

#include "map.h"
#include <QList>
#include "tile.h"
#include "unit.h"
#include "city.h";
#include "civilization.h"
#include "gamescene.h"

class UnitController
{
public:
    UnitController();

    void FindPath(Tile *startTile, Tile* endTile, Map *map, GameScene *scene, Unit *unit);
    void MoveUnit(Unit *unit, Map *map, int civListIndex);

    void Attack(Unit* attacker, Unit* target, bool attackFromWater);
    void RangeAttack(Unit* attacker, Unit* target);
    void AttackCity(Unit* attacker, City* city);

    void FoundCity(Unit* unit, Tile* CurrentTile, Civilization* currentCiv);
    void BuildImprovement(Unit* unit, Tile* currentTile, Civilization* currentCiv, TileImprovement improvement);

    void HealUnit(Unit* unit);

    Unit *FindUnitAtTile(Tile* tile, Map *map, QVector<Unit *> unitList);
    City *FindCityAtTile(Tile *tile, Map *map, QVector<City*> cityList);

private:
    int GetDistance(Tile* a, Tile *b);
    void RetracePath(Tile *start, Tile *end, Map *map, Unit *unit);

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
        default:
            return QString("No Nation");
        }
    }
};

#endif // UNITCONTROLLER_H
