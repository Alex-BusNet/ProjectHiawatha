#ifndef BUILDING_H
#define BUILDING_H

#include <QWidget>
#include <string.h>
#include <QString>
#include <QJsonObject>

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

    void WriteBuildingSaveData(QJsonObject &obj) const;
    void ReadBuildingSaveData(const QJsonObject &obj);
private:
    QString name;
    QString description;
    int productionCost;
    int currentProduction;
    int bonusType;// 0 = gold, 1 = production, 2 = science, 3 = food, 4 = culture, 5 = defense
    //Bonus is the value added to the yield once the building is finished
    int bonus;
    int buildingMaintenanceCost;
    int unlocked;
    //TechIndex indicates the index of the technology required to unlock the building
    int techIndex;


};

#endif // BUILDING_H
