#include "building.h"

Building::Building()
{

}

Building::Building(QString Name, QString buildDescription, int cost, int current, QString bonusKind, int bonusValue, int maintanenceCost, bool unlockedBuild)
{
    name = Name;
    description = buildDescription;
    productionCost = cost;
    currentProduction = current;
    bonusType = bonusKind;
    bonus = bonusValue;
    buildingMaintenanceCost = maintanenceCost;
    unlocked = unlockedBuild;

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

