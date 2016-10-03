#ifndef CITY_H
#define CITY_H

#include <QWidget>
#include <QString>
#include "nation.h"

class City
{
public:
    City();
    ~City();
private:
    QString name;
    Nation Civ;
    int productionYield;
    int scienceYield;
    int goldYield;

};

#endif // CITY_H
