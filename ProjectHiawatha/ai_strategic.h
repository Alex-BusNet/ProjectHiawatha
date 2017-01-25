//City Control
#ifndef AI_STRATEGIC_H
#define AI_STRATEGIC_H

#include <ai_operational.h>
#include <ai_tactical.h>
#include "civilization.h"
#include "map.h"
#include "gamescene.h"

class AI_Strategic
{
public:
    AI_Strategic(Civilization *civ, Civilization *player, Map *map);//Probably use default constructor
private:
    void cityProduction(Civilization *civ, Civilization *player);
    void invasionCheck(Civilization *civ, Civilization *player, Map *map);
    AI_Operational *aiOp;
    void cityLocation(Civilization *civ, Map *map);
    QVector<Tile*> cityLocations;
};

#endif // AI_STRATEGIC_H
