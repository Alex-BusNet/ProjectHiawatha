#ifndef AI_OPERATIONAL_H
#define AI_OPERATIONAL_H

#include "civilization.h"
#include "ai_tactical.h"
#include "map.h"
#include "gamescene.h"
class AI_Operational
{
public:
    AI_Operational();

    //Used Constructor
    AI_Operational(int midGoal, QVector<Tile *> CityToBeFounded, Civilization *civ, Civilization *player, Map *map);

private:
    void threatScan(Civilization *civ, Civilization *player, Map *map);
    void theaterAtWar(Civilization *civ, Civilization *player, City *cityTarget, Map *map);
    void theaterPrep(Civilization *civ, Civilization *player, QVector<Tile*> troopLocations);

     City *cityTarget;
     QVector<Tile*> troopLocations;

     AI_Tactical *aiTact;
};

#endif // AI_OPERATIONAL_H
