#include "ai_controller.h"
#include <QDebug>

void AI_Controller::AddAIToList(AI_Strategic *ai)
{
    //qDebug() << "       Adding AI to Controller";
    aiList.push_back(ai);
    //qDebug() << "       Done.";
}

void AI_Controller::turnStarted(Civilization *civ, Civilization *player, Map *map)
{
    //This calls the correct Strategic AI and starts their turn
    //aiList.at(aiIndex);
    aiStrat = new AI_Strategic(civ, player, map);
    delete aiStrat;
    return;
}
