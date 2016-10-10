//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include "unit.h"

#include <yield.h>
#include <city.h>
#include <nation.h>
#include <stdint.h>
#include <QVector>
#include <civcolors.h>


class City;

class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name);

    //Accessors and Mutators
    void UpdateCivYield();
    void AddCity(City* city);
    void AddUnit(Unit* unit);

    City *GetCityAt(int index);
    Unit* GetUnitAt(int index);
    Nation getCiv();
    Yield getCivYield();

    QVector<City*> GetCityList();
    QVector<Unit*> GetUnitList();

private:
    Nation name;
    //Enum for leader?
    //civcolors


    QVector<Unit*> UnitList; //Needs a Units object type
        //Will contain all of the civ's units

    QVector<City*> Citylist; //Needs a City object type
        //Will contain all of the civ's Cities


    Yield totalCivYield;
        //Will contain the totals for the civ's yields (uses city->cityyield)
};

#endif // CIVILIZATION_H
