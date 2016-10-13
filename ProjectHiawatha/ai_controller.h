#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "ai_strategic.h"
#include <QVector>
#include <QObject>
#include "civilization.h"

class AI_Controller
{
public:
    AI_Controller();

    void AddAIToList(AI_Strategic *ai);

    void turnStarted(Civilization *civ);

private:
    QVector<AI_Strategic*> aiList;

};

#endif // AI_CONTROLLER_H