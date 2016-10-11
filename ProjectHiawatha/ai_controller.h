#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "ai_strategic.h"
#include "civilization.h"
#include <QVector>
#include <QObject>
class Civilization;

class AI_Controller
{
public:
    AI_Controller();
    AI_Controller(Civilization *civ);

    void AddAIToList(AI_Strategic *ai);

    void turnStarted(int aiIndex);

private:
    Civilization *aiCiv;
    QVector<AI_Strategic*> aiList;

};

#endif // AI_CONTROLLER_H
