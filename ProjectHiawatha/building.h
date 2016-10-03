#ifndef BUILDING_H
#define BUILDING_H

#include <QWidget>
#include <string.h>
#include <QString>

class Building
{
public:
    Building();
    ~Building();
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
