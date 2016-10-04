#ifndef BUILDING_H
#define BUILDING_H

#include <QWidget>
#include <string.h>
#include <QString>

class Building
{
public:
    Building();
    Building(QString Name, QString description, int cost, int current, QString bonusType, int bonusValue, int maintanenceCost, bool unlocked);
    ~Building();
    QString getName();
    QString getDescription();
    int getProductionCost();
    int getCurrentProduction();
    QString getbonusType();
    int getBonusValue();
    int getBuildingMaintenanceCost();
    bool isUnlocked();

private:
    QString name;
    QString description;
    int productionCost;
    int currentProduction;
    QString bonusType;
    int bonus;
    int buildingMaintenanceCost;
    bool unlocked;


};

#endif // BUILDING_H
