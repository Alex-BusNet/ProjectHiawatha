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

void Yield::ChangeYield(Yield::YIELD yieldType, int newValue)
{
    switch(yieldType)
    {
        case GOLD:
            this->totalYield[0] = newValue;
            break;
        case PRODUCTION:
            this->totalYield[1] = newValue;
            break;
        case RESEARCH:
            this->totalYield[2] = newValue;
            break;
        case FOOD:
            this->totalYield[3] = newValue;
            break;
        case CULTURE:
            this->totalYield[4] = newValue;
            break;
        default:
            break;
    }
}

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



