//Each Civ is based here (Player and AI)

/*
 *  Holds all the overlap between player and AI classes
 *      City and units owned / locations
 *      Borders
 *      Data such as Tech progress, Yields, etc will be read through here (stored elsewhere)
 *
 */

#include "civilization.h"
#include <QDebug>

Civilization::Civilization()
{

}

Civilization::Civilization(Nation name)
{
    this->name=name;
    //set color
    //spawn a city, and initialize (based on nation)
    //call updatecityyield and updatecivyield to initialize
    totalCivYield.ChangeYield(0,0,0,0,0);
}





//Accessors and Mutators
Nation Civilization::getCiv()
{
    return this->name;
}



void Civilization::UpdateCivYield()
{
    //Re-tally's the total of the city yields
//    for(int i=0;i<city.length();i++){
//        totalyield+=cityyield;//need to adjust so it cycles through cities - syntax?
//    }
}

Yield Civilization::getCivYield()
{
    return this->totalCivYield;
}

QVector<City *> Civilization::GetCityList()
{
    return this->Citylist;
}

QVector<Unit *> Civilization::GetUnitList()
{
    return this->UnitList;
}

void Civilization::AddCity(City *city)
{
    this->Citylist.push_back(city);
}

void Civilization::AddUnit(Unit *unit)
{
    this->UnitList.push_back(unit);
}

City* Civilization::GetCityAt(int index)
{
    if(index < this->Citylist.size())
        return this->Citylist.at(index);
}

Unit* Civilization::GetUnitAt(int index)
{

    if(index < this->UnitList.size())
    {
        return this->UnitList.at(index);
    }
    else
    {
        qDebug() << "UnitList -- Index out of range";
        return NULL;
    }
}





