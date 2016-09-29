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





//Accessors and Mutators
void Civilization::setCiv(int nation)
{
    this->nation=nation;
}

int Civilization::getCiv()
{
    return this->nation;
}

void Civilization::setLeader(int leader)
{
    this->leader=leader;
}

int Civilization::getLeader()
{
    return this->leader;
}


