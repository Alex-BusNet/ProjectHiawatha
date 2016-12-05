#ifndef AI_OPERATIONAL_H
#define AI_OPERATIONAL_H

#include "civilization.h"
#include "ai_tactical.h"
#include "map.h"
#include "gamescene.h"
class AI_Operational
{
public:
    AI_Operational(QVector<Tile *> CityToBeFounded, Civilization *civ, Civilization *player, Map *map);
private:
    void threatScan(Civilization *civ, Civilization *player, Map *map);
    void theaterAtWar(Civilization *civ, Civilization *player);
     City *cityTarget;
     AI_Tactical *aiTact;
};

#endif // AI_OPERATIONAL_H
