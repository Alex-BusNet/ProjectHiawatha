//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include "ai_controller.h"
#include "unit.h"

#include <yield.h>
#include <city.h>
#include <nation.h>
#include <stdint.h>
#include <QVector>
#include <civcolors.h>

class City;
class AI_Controller;

class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name);

    //Accessors and Mutators
    void UpdateCivYield();
    void AddCity(City* city);
    void AddUnit(Unit* unit);

    void SetUnitList(QVector<Unit*> list);
    void SetCityList(QVector<City*> list);
    void SetCivObj(Civilization *civ);

    City *GetCityAt(int index);
    Unit* GetUnitAt(int index);
    Nation getCiv();
    Civilization* GetCivObject();
    Yield* getCivYield();

    QVector<City*> GetCityList();
    QVector<Unit*> GetUnitList();

    void StartAITurn(int aiIndex, bool isPlayer);

//    void startTurn(int aiIndex);

private:
    Nation name;
    //Enum for leader?
    //civcolors

    AI_Controller *aiCtrl;

    QVector<Unit*> UnitList; //Needs a Units object type
        //Will contain all of the civ's units

    QVector<City*> CityList; //Needs a City object type
        //Will contain all of the civ's Cities


    Yield* totalCivYield;
        //Will contain the totals for the civ's yields (uses city->cityyield)

    void UpdateYield();

};

#endif // CIVILIZATION_H
