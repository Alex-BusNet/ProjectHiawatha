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
    //void threatScan(Vector* Highthreats, Vector* Midthreats, Vector* Lowthreats);
    void theaterAtWar(Civilization *civ);
    void theaterPrep(Civilization *civ);
    void cityLocation(Civilization *civ);

     QVector<Tile*> cityLocations;
     City target;
     QVector<Tile*> troopLocations;

     QVector<Tile*> LowThreats;
     QVector<Tile*> MidThreats;
     QVector<Tile*> HighThreats;
};

#endif // AI_OPERATIONAL_H
