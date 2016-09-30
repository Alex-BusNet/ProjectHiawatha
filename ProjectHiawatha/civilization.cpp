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
}





//Accessors and Mutators
void Civilization::setCiv(Nation name)
{
    this->name=name;
}

Nation Civilization::getCiv()
{
    return this->name;
}



