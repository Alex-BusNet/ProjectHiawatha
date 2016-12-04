#include "ai_controller.h"
#include <QDebug>

AI_Controller::AI_Controller()
{

}

void AI_Controller::AddAIToList(AI_Strategic *ai)
{
    qDebug() << "       Adding AI to Controller";
    aiList.push_back(ai);
    qDebug() << "       Done.";
}

void AI_Controller::turnStarted(Civilization *civ, Civilization *player, Map *map)
{
//    qDebug()<<"AI Controller Called";
    //This calls the correct Strategic AI and starts their turn
    //aiList.at(aiIndex);
   //qDebug() << "AI" << aiIndex << "of" << aiList.size() <<"turn";

    aiStrat = new AI_Strategic(civ, player, map);
    delete aiStrat;
    return;
}
