#ifndef UNITCONTROLLER_H
#define UNITCONTROLLER_H

#include "map.h"
#include <QList>
#include "tile.h"
#include "unit.h"


class UnitController
{
public:
    UnitController();

    void FindPath(Tile *startTile, Tile* endTile, Map *map, GameScene *scene, Unit *unit);
    void MoveUnit(Unit *unit, Map *map, GameScene *scene);

    Unit *FindUnitAtTile(Tile* tile, Map *map, QVector<Unit *> unitList);

private:
    int GetDistance(Tile* a, Tile *b);
    void RetracePath(Tile *start, Tile *end, Map *map, Unit *unit);
};

#endif // UNITCONTROLLER_H
