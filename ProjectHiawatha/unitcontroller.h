#ifndef UNITCONTROLLER_H
#define UNITCONTROLLER_H

#include "map.h"
#include <QList>


class UnitController
{
public:
    UnitController();

    void FindPath(Tile *startTile, Tile* endTile, Map *map, GameScene *scene, Unit *unit);
    void MoveUnit(Unit *unit, Map *map, GameScene *scene);

private:
    int GetDistance(Tile* a, Tile *b);
    void RetracePath(Tile *start, Tile *end, Map *map, Unit *unit);
};

#endif // UNITCONTROLLER_H
