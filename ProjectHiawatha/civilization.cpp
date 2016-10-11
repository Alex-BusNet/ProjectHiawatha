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
    this->totalCivYield = new Yield(0, 0, 0, 0, 0);

//    if(isAI)
//    {
        /*
         *
         * Will need to set some sort of flag
         * here so we can tell if the selected civ is an AI
         *
         */
//    }
}





//Accessors and Mutators
Nation Civilization::getCiv()
{
    return this->name;
}

Civilization *Civilization::GetCivObject()
{
    return this;
}

void Civilization::UpdateCivYield()
{
    qDebug() << "   Civ controls" << CityList.size() << "cities";

    int newGold = 0, newProd = 0, newSci = 0, newFood = 0, newCul = 0;

    foreach(City *city, CityList)
    {
        newGold += city->getCityYield()->GetGoldYield();
        newProd += city->getCityYield()->GetProductionYield();
        newSci += city->getCityYield()->GetScienceYield();
        newFood += city->getCityYield()->GetFoodYield();
        newCul += city->getCityYield()->GetCultureYield();
    }
    this->totalCivYield->ChangeYield(newGold, newProd, newSci, newFood, newCul);
}

Yield *Civilization::getCivYield()
{
    return this->totalCivYield;
}

QVector<City *> Civilization::GetCityList()
{
    return this->CityList;
}

QVector<Unit *> Civilization::GetUnitList()
{
    return this->UnitList;
}

void Civilization::StartAITurn(int aiIndex, bool isPlayer)
{
    if(!isPlayer)
    {
        //This will tell the AI_Strategic or AI_Controller
        // to start its turn;
        return;
    }
    else
    {
        return;
    }
}

void Civilization::AddCity(City *city)
{
    this->CityList.push_back(city);
}

void Civilization::AddUnit(Unit *unit)
{
    this->UnitList.push_back(unit);
}

void Civilization::SetUnitList(QVector<Unit *> list)
{
    UnitList = list;
}

void Civilization::SetCityList(QVector<City *> list)
{
    CityList = list;
}

void Civilization::SetCivObj(Civilization *civ)
{
    this->CityList = civ->GetCityList();
    this->UnitList = civ->GetUnitList();
}

City* Civilization::GetCityAt(int index)
{
    if(index < this->CityList.size())
    {
        return this->CityList.at(index);
    }
    else
    {
        //If the index is too large, return the capital
        return this->CityList.at(0);
    }
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





