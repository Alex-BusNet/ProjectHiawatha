//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include "unit.h"
#include "technology.h"
#include <yield.h>
#include <city.h>
#include <nation.h>
#include <stdint.h>
#include <QVector>
#include <civcolors.h>
#include "datatypes.h"
#include "qqueue.h"

class City;
class AI_Controller;

typedef struct {ActionState action; Unit *unit; } AIQueueData;

class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name, bool isAI, QString leaderName);

    //Accessors and Mutators
    void UpdateCivYield();
    void AddCity(City* city);
    void AddUnit(Unit* unit);
    void RemoveCity(int cityIndex);
    void RemoveUnit(int unitIndex);
    void SetCityIndex(int index);
    void SetLeaderName(QString name);    
    void loadTechs(QString filename);
    void setAccumulatedScience(int science);
    void resetAccumulatedScience();
    void SetCaptialsControlled(int cc);
    void IncrementCapitalsControlled();
    void SetAtWar(Nation enemy, int enemyCivListIndex);
    void setCurrentTech(Technology* tech);
    void setTechIndex();
    void setNextTech(Technology* tech);
    void loadCities(QString filename);
    void setCivIndex(int index);
    void MakePeace();

    QString GetLeaderName();

    City *GetCityAt(int index);
    Unit* GetUnitAt(int index);
    Nation getCiv();
    Civilization* GetCivObject();
    Yield* getCivYield();

    int GetTotalGold();
    int GetTotalScience();
    int GetTotalCulture();
    int getCityIndex();
    int getAccumulatedScience(void);
    int getTechIndex();
    int GetCapitalsControlled();
    int GetCivListIndexAtWar();
    int getCivIndex();

    QVector<City*> GetCityList();
    QVector<Unit*> GetUnitList();
    QVector<Technology*> GetTechList();
    QVector<QString> GetInitialCityList();

    Update_t UpdateProgress();
    QString GetNextCityName();

    Nation GetNationAtWar();

    Technology* getCurrentTech();
    Technology *getNextTech();

    bool alive;

    //AI Funcs
    void StartAITurn(int aiIndex, bool isPlayer);
    void setLowThreats(QVector<Unit *> lowThreats);
    void setMidThreats(QVector<Unit *> midThreats);
    void setHighThreats(QVector<Unit *> highThreats);
    void setCityFounding(AIQueueData data);
    void clearThreats();

    QVector<Unit *> getLowThreats();
    QVector<Unit *> getMidThreats();
    QVector<Unit *> getHighThreats();

    QQueue<AIQueueData> getCityFounding();
    AIQueueData dequeue();

    bool isAtWar();
    bool isEmpty();
    bool isCivAI();
    bool cityFounded;

    int queueSize();

private:
    Nation name;

    QVector<Unit*> UnitList;
    QVector<City*> currentCityList;
    QVector<QString> initialCityList;
    QVector <Technology*> techList;
    QString LeaderName;

    int cityIndex;

    Yield* totalCivYield;

    void UpdateYield();

    Technology* currentTech;
    Technology* nextTech;

    //Keeps track of which tech in the list the civ is currently on
    int techIndex;
    int totalScience, totalGold, totalCulture;
    int accumulatedScience;
    int capitalsControlled;

    int atWarWithCivListIndex;
    Nation atWarWithNation;
    bool atWar;

    //AI stuff
    bool isAIPlayer;
    QVector<Unit*> lowThreats;
    QVector<Unit*> midThreats;
    QVector<Unit*> highThreats;
    QQueue<AIQueueData> cityFounding;

    int civIndex;
};

#endif // CIVILIZATION_H
