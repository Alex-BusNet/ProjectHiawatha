//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include <yield.h>
#include <tile.h>
#include <nation.h>
#include <stdint.h>
#include <QVector>



class Civilization
{
public:
    Civilization();//default, don't use
    Civilization(Nation name);

    //Accessors and Mutators
    void setCiv(Nation name);
    Nation getCiv();


private:
    Nation name;
    //Enum for leader?
    //enum for colors - primary and secondary?

    //QVector<Unit> UnitList; Needs a Units object type
        //Will contain all of the civ's units

    //QVector<City> citylist; Needs a City object type
        //Will contain all of the civ's Cities

   // QVector<Tile> citytiles; - Tile not declared?
        //contains coordinates of all of the tiles associated with a specific city

    QVector<Yield> cityyield;
        //Uses citytiles to assemble yield totals for city

    QVector<Yield> totalyield;
        //Will contain the totals for the civ's yields (uses cityyield)
};

#endif // CIVILIZATION_H
