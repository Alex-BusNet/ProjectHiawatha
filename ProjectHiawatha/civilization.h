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

class City;
class AI_Controller;

class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name, bool isAI);

    //Accessors and Mutators
    void UpdateCivYield();
    void AddCity(City* city);
    void AddUnit(Unit* unit);

    void RemoveCity(int cityIndex);
    void RemoveUnit(int unitIndex);

    void SetUnitList(QVector<Unit*> list);
    void SetCityList(QVector<City*> list);
    void SetCivObj(Civilization *civ);
    void SetHappiness(int happiness);

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

    int getCityIndex();
    //AI stuff
    void StartAITurn(int aiIndex, bool isPlayer);
    bool isCivAI();

//    void startTurn(int aiIndex);
    void loadTechs(QString filename);
    void setAccumulatedScience(int science);
    int getAccumulatedScience(void);
    void resetAccumulatedScience();
    int getTechIndex();
    void setTechIndex();
    Technology* getCurrentTech();
    Technology *getNextTech();
    void setCurrentTech(Technology* tech);
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

private:
    Nation name;
    //Enum for leader?
    //civcolors

    QVector<Unit*> UnitList; //Needs a Units object type
        //Will contain all of the civ's units

    QVector<City*> currentCityList; //Needs a City object type
        //Will contain all of the civ's Cities

    QVector<QString> initialCityList;



    int cityIndex;

    Yield* totalCivYield;
        //Will contain the totals for the civ's yields (uses city->cityyield)
    QVector <Technology*> techList;

    void UpdateYield();

    Technology* currentTech;
    Technology* nextTech;

    int happiness;
    int techIndex;
    int totalScience, totalGold, totalCulture;
    int accumulatedScience;

    //AI stuff
    bool isAIPlayer;
    QVector<Unit*> lowThreats;
    QVector<Unit*> midThreats;
    QVector<Unit*> highThreats;
};

#endif // CIVILIZATION_H
