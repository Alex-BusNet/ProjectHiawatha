#include "yield.h"
#include <QDebug>

Yield::Yield()
{
    Yield{0, 0, 0, 0, 0};
}

Yield::Yield(int gold, int prod, int research, int food, int culture)
{
    this->totalYield.gold = gold;
    this->totalYield.production = prod;
    this->totalYield.research = research;
    this->totalYield.food = food;
    this->totalYield.culture = culture;
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
            this->totalYield.gold += increaseBy;
            break;
        case PRODUCTION:
            this->totalYield.production += increaseBy;
            break;
        case RESEARCH:
            this->totalYield.research += increaseBy;
            break;
        case FOOD:
            this->totalYield.food += increaseBy;
            break;
        case CULTURE:
            this->totalYield.culture += increaseBy;
            break;
        default:
            break;
    }
}

/*
 *  Overloaded function of ChangeYield(Yield::Yield yieldType, int newValue)
 *
 *  This function takes in 5 integers and
 *  adds them to the totalYield struct of the parent
 *  object (tile, city, and civ).
 *
 *  Passing NEGATIVE values in the array will result in
 *  Yield being REMOVED from the parent object's totalYield
 *
 */
void Yield::ChangeYield(int gold, int prod, int sci, int food, int cul)
{
    this->totalYield.gold += gold;
    this->totalYield.production += prod;
    this->totalYield.research += sci;
    this->totalYield.food += food;
    this->totalYield.culture += cul;
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
            return this->totalYield.gold;
            break;
        case PRODUCTION:
            return this->totalYield.production;
            break;
        case RESEARCH:
            return this->totalYield.research;
            break;
        case FOOD:
            return this->totalYield.food;
            break;
        case CULTURE:
            return this->totalYield.culture;
            break;
        default:
            return -1;
            break;
    }
}

int Yield::GetGoldYield()
{
    return this->totalYield.gold;
}

int Yield::GetProductionYield()
{
    return this->totalYield.production;
}

int Yield::GetScienceYield()
{
    return this->totalYield.research;
}

int Yield::GetFoodYield()
{
    return this->totalYield.food;
}

int Yield::GetCultureYield()
{
    return this->totalYield.culture;
}

void Yield::WriteYieldSaveData(QJsonObject &obj) const
{
    obj["gold"] = totalYield.gold;
    obj["production"] = totalYield.production;
    obj["science"] = totalYield.research;
    obj["food"] = totalYield.food;
    obj["culture"] = totalYield.culture;
}

void Yield::ReadYieldSaveData(const QJsonObject &obj)
{
    totalYield.gold = obj["gold"].toInt();
    totalYield.production = obj["production"].toInt();
    totalYield.research = obj["science"].toInt();
    totalYield.food = obj["food"].toInt();
    totalYield.culture = obj["culture"].toInt();
}
