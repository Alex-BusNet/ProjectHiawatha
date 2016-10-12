#include "ai_controller.h"
#include <QDebug>

AI_Controller::AI_Controller()
{

}

AI_Controller::AI_Controller(Civilization* civ)
{
    this->aiCiv = civ;
}

void AI_Controller::AddAIToList(AI_Strategic *ai)
{
    aiList.push_back(ai);
}

void AI_Controller::turnStarted(int aiIndex)
{
    //This calls the correct Strategic AI and starts their turn
//    aiList.at(aiIndex);
    qDebug() << "AI" << aiIndex + 1 << "turn";
    return;
}
