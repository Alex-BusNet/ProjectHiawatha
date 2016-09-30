//Each Civ is based here (Player and AI)

/*
 *  Holds all the overlap between player and AI classes
 *      City and units owned / locations
 *      Borders
 *      Data such as Tech progress, Yields, etc will be read through here (stored elsewhere)
 *
 */

#include "civilization.h"

Civilization::Civilization()
{

}

Civilization::Civilization(Nation name)
{
    this->name=name;
    //spawn a city, and initialize (based on nation)
    //call updatecityyield and updatecivyield to initialize
}





//Accessors and Mutators
Nation Civilization::getCiv()
{
    return this->name;
}

//move to city class
//void Civilization::UpdateCityYield(Yield yield)
//{
//    //Re-evaluates the yield for the city
//    for(int i=0; i<city.tiles();i++){
//        cityyield+=tile.yield;//need to adjust so it cycles the tiles - syntax?
//    }
//}

//Yield Civilization::getCityYield()
//{
//    return this->cityyield;
//}

void Civilization::UpdateCivYield()
{
    //Re-tally's the total of the city yields
//    for(int i=0;i<city.length();i++){
//        totalyield+=cityyield;//need to adjust so it cycles through cities - syntax?
//    }
}

Yield Civilization::getCivYield()
{
    return this->totalyield;
}





