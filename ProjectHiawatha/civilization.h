//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include <yield.h>
#include <city.h>
#include <nation.h>
#include <stdint.h>
#include <QVector>
#include <civcolors.h>



class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name);

    //Accessors and Mutators
    Nation getCiv();
    void UpdateCivYield();
    Yield getCivYield();

private:
    Nation name;
    //Enum for leader?
    //civcolors

    //QVector<Unit> UnitList; Needs a Units object type
        //Will contain all of the civ's units

    //QVector<City> citylist; Needs a City object type
        //Will contain all of the civ's Cities


    Yield totalyield;
        //Will contain the totals for the civ's yields (uses city->cityyield)
};

#endif // CIVILIZATION_H
