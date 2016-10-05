#ifndef BUILDING_H
#define BUILDING_H

#include <QWidget>
#include <string.h>
#include <QString>

class Building
{
public:
    Building();
    Building(QString Name, QString buildDescription, int cost, int current, QString bonusKind, int bonusValue, int maintanenceCost, bool unlockedBuild);
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
