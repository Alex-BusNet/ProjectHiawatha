//City Control

#ifndef AI_STRATEGIC_H
#define AI_STRATEGIC_H

#include <ai_operational.h>
#include <ai_tactical.h>
#include "civilization.h"
#include "map.h"

class AI_Strategic
{
public:
    AI_Strategic(Civilization *civ, Map *map);//Probably use default constructor
    //shouldn't need passed values

private:
    int midTermGoal(Civilization *civ);
    void cityProduction(int midGoal, Civilization *civ);
};

#endif // AI_STRATEGIC_H
