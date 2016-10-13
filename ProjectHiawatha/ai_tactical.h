//Individual unit Control


#ifndef AI_TACTICAL_H
#define AI_TACTICAL_H
#include "civilization.h"

class AI_Tactical
{
public:
    AI_Tactical();

    AI_Tactical(int midGoal, Civilization *civ, QVector<Tile*> CityToBeFounded, City *cityTarget, QVector<Tile*> TroopPositions, QVector<Tile*> highThreats, QVector<Tile*> midThreats, QVector<Tile*> lowThreats);

    //2 functions, depending if the civ is attacking or defending
    void Prep(Civilization *civ, QVector<Tile*> TroopPositions);
    void AtWar(Civilization *civ, City *cityTarget);


private:

    void settlercontrol(QVector<Tile *>  CityToBeFounded);
    void workercontrol();
    void highThreatProcessing(QVector<Tile*> highThreats);
    void midThreatProcessing(QVector<Tile*> midThreats);
    void lowThreatProcessing(QVector<Tile*> lowThreats);


};

#endif // AI_TACTICAL_H
