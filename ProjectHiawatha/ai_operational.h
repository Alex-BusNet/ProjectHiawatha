#ifndef AI_OPERATIONAL_H
#define AI_OPERATIONAL_H

#include "civilization.h"
#include "ai_tactical.h"
class AI_Operational
{
public:
    AI_Operational();

    //Used Constructor
    AI_Operational(int midGoal, Civilization *civ);

private:
    void threatScan(QVector<Tile*> highThreats, QVector<Tile*> midThreats, QVector<Tile*> lowThreats);
    void theaterAtWar(Civilization *civ, City *cityTarget);
    void theaterPrep(Civilization *civ, QVector<Tile*> troopLocations);
    void cityLocation(Civilization *civ);

     QVector<Tile*> cityLocations;
     City *cityTarget;
     QVector<Tile*> troopLocations;

     QVector<Tile*> lowThreats;
     QVector<Tile*> midThreats;
     QVector<Tile*> highThreats;
};

#endif // AI_OPERATIONAL_H
