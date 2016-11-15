#include "building.h"

Building::Building()
{

}

Building::Building(QString Name, QString buildDescription, int cost, int bonusKind, int bonusValue, int maintanenceCost, int unlockedBuild, int techIndex)
{
    this->name = Name;
    this->description = buildDescription;
    this->productionCost = cost;
    this->techIndex = techIndex;
    this->bonusType = bonusKind;
    this->bonus = bonusValue;
    this->buildingMaintenanceCost = maintanenceCost;
    this->unlocked = unlockedBuild;

}

Building::~Building()
{

}

QString Building::getName() const
{
    return this->name;

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

int Building::getbonusType()
{
    return bonusType;
}

int Building::getBonusValue()
{
    return this->bonus;
}

int Building::getBuildingMaintenanceCost()
{
    return buildingMaintenanceCost;
}

int Building::isUnlocked()
{
    return unlocked;
}

int Building::getTechIndex()
{
    return this->techIndex;
}

void Building::setUnlocked(int unlock)
{
    this->unlocked = unlock;
}

