//Individual unit Control

#ifndef AI_TACTICAL_H
#define AI_TACTICAL_H
#include "civilization.h"
#include "unitcontroller.h"
#include "unit.h"
#include "tile.h"
#include "map.h"
#include "gamescene.h"

class AI_Tactical
{
public:
    AI_Tactical(Civilization *civ, Map *map, QVector<Tile *> CityToBeFounded, City *cityTarget);
    void AtWar(Civilization *civ, Map *map, City *cityTarget);
private:
    void settlercontrol(Civilization *civ, Map *map, QVector<Tile *> CityToBeFounded);
    void workercontrol(Civilization *civ, Map *map);
    void highThreatProcessing(Civilization *civ, Map *map);
    void lowThreatProcessing(Civilization *civ, Map *map);
    UnitController *uc;
};

#endif // AI_TACTICAL_H
