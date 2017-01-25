#include "ai_controller.h"
#include <QDebug>
//#define DEBUG
void AI_Controller::AddAIToList(AI_Strategic *ai)
{
#ifdef DEBUG
     qDebug() << "       Adding AI to Controller";
#endif
    aiList.push_back(ai);
#ifdef DEBUG
     qDebug() << "       Done.";
#endif
}

void AI_Controller::turnStarted(Civilization *civ, Civilization *player, Map *map)
{
    //This calls the correct Strategic AI and starts their turn
    //aiList.at(aiIndex);
    aiStrat = new AI_Strategic(civ, player, map);
    delete aiStrat;
    return;
}
