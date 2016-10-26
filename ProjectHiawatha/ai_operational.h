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
    AI_Operational(int midGoal, Civilization *civ, Civilization *player, Map *map, GameScene *scene);

private:
    void threatScan(Civilization *civ, Civilization *player);
    void theaterAtWar(Civilization *civ, Civilization *player, City *cityTarget);
    void theaterPrep(Civilization *civ, Civilization *player, QVector<Tile*> troopLocations);
    void cityLocation(Civilization *civ);

     QVector<Tile*> cityLocations;
     City *cityTarget;
     QVector<Tile*> troopLocations;


};

#endif // AI_OPERATIONAL_H
