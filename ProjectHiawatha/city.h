#ifndef CITY_H
#define CITY_H
#include <tile.h>
#include <QVector>
#include <QWidget>
#include <QString>
#include "nation.h"

class City
{
public:
    City();

    //Accessor and Mutators
    Yield getCityYield();
    void UpdateCityYield(Yield yield);

    ~City();
private:

    //QVector<Tile> citytiles;// - Need tile object, not tile class?
        //contains coordinates of all of the tiles associated with a specific city


    QVector<Yield> cityyield;
    //Uses citytiles to assemble yield totals for city - city class, not here


    QString name;
    Nation Civ;
    int productionYield;
    int scienceYield;
    int goldYield;
};


#endif // CITY_H
