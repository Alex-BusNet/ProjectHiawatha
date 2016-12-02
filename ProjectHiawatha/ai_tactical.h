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
    AI_Tactical();

    AI_Tactical(Civilization *civ, Civilization *player, Map *map, QVector<Tile *> CityToBeFounded, City *cityTarget);

    //2 functions, depending if the civ is attacking or defending
    void Prep(Civilization *civ, Civilization *player, Map *map);
    void AtWar(Civilization *civ, Map *map, City *cityTarget);


private:

    void settlercontrol(Civilization *civ, Map *map, QVector<Tile *> CityToBeFounded);
    void workercontrol(Civilization *civ, Map *map);
    void highThreatProcessing(Civilization *civ, Civilization *player, Map *map);
    void midThreatProcessing(Civilization *civ, Civilization *player, Map *map);
    void lowThreatProcessing(Civilization *civ, Civilization *player, Map *map);


};

#endif // AI_TACTICAL_H
