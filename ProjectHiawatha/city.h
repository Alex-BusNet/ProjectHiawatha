#ifndef CITY_H
#define CITY_H
#include <tile.h>
#include <QVector>

class City
{
public:
    City();

    //Accessor and Mutators
    Yield getCityYield();
    void UpdateCityYield(Yield yield);

private:

    //QVector<Tile> citytiles;// - Need tile object, not tile class?
        //contains coordinates of all of the tiles associated with a specific city


    QVector<Yield> cityyield;
    //Uses citytiles to assemble yield totals for city - city class, not here
};


#endif // CITY_H
