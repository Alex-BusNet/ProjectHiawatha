#ifndef CITY_H
#define CITY_H

#include <QWidget>
#include <QString>

class City
{
public:
    City();
    ~City();
private:
    QString name;
    QString Civ;
    int productionYield;
    int scienceYield;
    int goldYield;

};

#endif // CITY_H
