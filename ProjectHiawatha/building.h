#ifndef BUILDING_H
#define BUILDING_H

#include <QWidget>
#include <string.h>
#include <QString>

class Building
{
public:
    Building();
    Building(QString Name, QString buildDescription, int cost, int bonusKind, int bonusValue, int maintanenceCost, int unlockedBuild, int techIndex);
    ~Building();
    QString getName() const;
    QString getDescription();
    int getProductionCost();
    int getCurrentProduction();
    int getbonusType();
    int getBonusValue();
    int getBuildingMaintenanceCost();
    int isUnlocked();
    int getTechIndex();
    void setUnlocked(int unlock);
private:
    QString name;
    QString description;
    int productionCost;
    int currentProduction;
    int bonusType;// 0 = gold, 1 = production, 2 = science, 3 = food, 4 = culture, 5 = defense
    int bonus;
    int buildingMaintenanceCost;
    int unlocked;
    int techIndex;


};

#endif // BUILDING_H
