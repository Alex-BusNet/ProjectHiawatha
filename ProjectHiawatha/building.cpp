#include "building.h"

Building::Building()
{

}

Building::Building(QString Name, QString description, int cost, int current, QString bonusType, int bonusValue, int maintanenceCost, bool unlocked)
{

}

Building::~Building()
{

}

QString Building::getName()
{
    return name;

}

QString Building::getDescription()
{
    return description;

}

int Building::getProductionCost()
{
    return productionCost;
}

int Building::getCurrentProduction()
{
    return currentProduction;

}

QString Building::getbonusType()
{
    return bonusType;
}

int Building::getBonusValue()
{
    return bonus;
}

int Building::getBuildingMaintenanceCost()
{
    return buildingMaintenanceCost;
}

bool Building::isUnlocked()
{
    return unlocked;
}

