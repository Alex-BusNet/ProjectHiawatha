//Individual unit Control


#ifndef AI_TACTICAL_H
#define AI_TACTICAL_H
#include "civilization.h"

class AI_Tactical
{
public:
    AI_Tactical();

    AI_Tactical(int midGoal, Civilization *civ,QVector<Tile*> CityToBeFounded, City Target, QVector<Tile*> TroopPositions, QVector<Tile*> Highthreats, QVector<Tile*> Midthreats, QVector<Tile*> Lowthreats);

    //2 functions, depending if the civ is attacking or defending
    void Prep(Civilization *civ, QVector<Tile*> CityToBeFounded,QVector<Tile*> TroopPositions, QVector<Tile*> Highthreats, QVector<Tile*> Midthreats, QVector<Tile*> Lowthreats);
    void AtWar(Civilization *civ, QVector<Tile*> CityToBeFounded, City Target, QVector<Tile*> Highthreats, QVector<Tile*> Midthreats, QVector<Tile*> Lowthreats);


private:

    //settlercontrol(Queue CityToBeFounded);
    //workercontrol();
    //highThreats(Vector Highthreats);
    //midThreats(Vector Midthreats);
    //lowThreats(Vector Lowthreats);


};

#endif // AI_TACTICAL_H
