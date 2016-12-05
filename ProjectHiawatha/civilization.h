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

    void SetUnitList(QVector<Unit*> list);
    void SetCityList(QVector<City*> list);
    void SetCityIndex(int index);
    void SetCivObj(Civilization *civ);
    void SetHappiness(int happiness);

    void SetLeaderName(QString name);
    QString GetLeaderName();

    City *GetCityAt(int index);
    Unit* GetUnitAt(int index);
    Nation getCiv();
    Civilization* GetCivObject();
    Yield* getCivYield();

    int getHappiness();
    int GetTotalGold();
    int GetTotalScience();
    int GetTotalCulture();

    QVector<City*> GetCityList();
    QVector<Unit*> GetUnitList();

    Update_t UpdateProgress();

    QVector<Technology*> GetTechList();

    QVector<QString> GetInitialCityList();
    QString GetNextCityName();

    //AI stuff
    void StartAITurn(int aiIndex, bool isPlayer);
    bool isCivAI();

//    void startTurn(int aiIndex);
    void loadTechs(QString filename);
    void setAccumulatedScience(int science);
    void resetAccumulatedScience();
    void SetCaptialsControlled(int cc);
    void IncrementCapitalsControlled();
    void SetAtWar(Nation enemy, int enemyCivListIndex);
    void MakePeace();

    int getCityIndex();
    int getAccumulatedScience(void);
    int getTechIndex();
    int GetCapitalsControlled();
    int GetCivListIndexAtWar();
    Nation GetNationAtWar();

    Technology* getCurrentTech();
    Technology *getNextTech();

    void setCurrentTech(Technology* tech);
    void setTechIndex();
    void setNextTech(Technology* tech);
    void loadCities(QString filename);

    //AI Funcs
    void setLowThreats(QVector<Unit *> lowThreats);
    void setMidThreats(QVector<Unit *> midThreats);
    void setHighThreats(QVector<Unit *> highThreats);
    void clearThreats();

    QVector<Unit *> getLowThreats();
    QVector<Unit *> getMidThreats();
    QVector<Unit *> getHighThreats();


    void setCivIndex(int index);
    int getCivIndex();
    void setCityFounding(AIQueueData data);
    QQueue<AIQueueData> getCityFounding();

    AIQueueData dequeue();
    bool isAtWar();
    bool isEmpty();
    int queueSize();
    bool cityFounded;
    bool alive;

private:
    Nation name;

    QVector<Unit*> UnitList; //Needs a Units object type
    QVector<City*> currentCityList; //Needs a City object type
    QVector<QString> initialCityList;
    QVector <Technology*> techList;
    QString LeaderName;


    int cityIndex;

    Yield* totalCivYield;
        //Will contain the totals for the civ's yields (uses city->cityyield)


    void UpdateYield();

    Technology* currentTech;
    Technology* nextTech;

    int happiness;
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
