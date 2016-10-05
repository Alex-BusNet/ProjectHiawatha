#include "yield.h"

Yield::Yield()
{
    Yield(1, 1, 0, 1, 0);
}

Yield::Yield(int gold, int prod, int research, int food, int culture)
{
    this->totalYield[0] = gold;
    this->totalYield[1] = prod;
    this->totalYield[2] = research;
    this->totalYield[3] = food;
    this->totalYield[4] = culture;
}

Yield::~Yield()
{

}

/*
 *
 *  This function takes in a YIELD enum as well as
 *  the amount to increase the yield type by. This then
 *  adds the passed integer to the selected yield type of the parent
 *  object (tile, city, and civ).
 *
 *  Passing NEGATIVE values in the array will result in
 *  Yield being REMOVED from the parent object's totalYield
 *
 */
void Yield::ChangeYield(Yield::YIELD yieldType, int increaseBy)
{
    switch(yieldType)
    {
        case GOLD:
            this->totalYield[0] += increaseBy;
            break;
        case PRODUCTION:
            this->totalYield[1] += increaseBy;
            break;
        case RESEARCH:
            this->totalYield[2] += increaseBy;
            break;
        case FOOD:
            this->totalYield[3] += increaseBy;
            break;
        case CULTURE:
            this->totalYield[4] += increaseBy;
            break;
        default:
            break;
    }
}

/*
 *  Overloaded function of ChangeYield(Yield::Yield yieldType, int newValue)
 *
 *  This function takes in an array of 5 integers and
 *  adds them to the totalYield array of the parent
 *  object (tile, city, and civ).
 *
 *  Passing NEGATIVE values in the array will result in
 *  Yield being REMOVED from the parent object's totalYield
 *
 */
void Yield::ChangeYield(int yieldArr[])
{
    this->totalYield[0] += yieldArr[0];
    this->totalYield[1] += yieldArr[1];
    this->totalYield[2] += yieldArr[2];
    this->totalYield[3] += yieldArr[3];
    this->totalYield[4] += yieldArr[4];
}

/*
 *  Overloaded function of ChangeYield(Yield::Yield yieldType, int newValue)
 *
 *  This function takes in 5 integers and
 *  adds them to the totalYield array of the parent
 *  object (tile, city, and civ).
 *
 *  Passing NEGATIVE values in the array will result in
 *  Yield being REMOVED from the parent object's totalYield
 *
 */
void Yield::ChangeYield(int gold, int prod, int sci, int food, int cul)
{
    this->totalYield[0] += gold;
    this->totalYield[1] += prod;
    this->totalYield[2] += sci;
    this->totalYield[3] += food;
    this->totalYield[4] += cul;
}

/*
 *
 *  This function takes in a yieldType enum
 *  and returns the desired value
 *
 */
int Yield::GetYield(Yield::YIELD yieldType)
{
    switch(yieldType)
    {
        case GOLD:
            return this->totalYield[0];
            break;
        case PRODUCTION:
            return this->totalYield[1];
            break;
        case RESEARCH:
            return this->totalYield[2];
            break;
        case FOOD:
            return this->totalYield[3];
            break;
        case CULTURE:
            return this->totalYield[4];
            break;
        default:
            return -1;
            break;
    }
}
