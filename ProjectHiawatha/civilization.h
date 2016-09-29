//Each Civ is based here (Player and AI)

#ifndef CIVILIZATION_H
#define CIVILIZATION_H

#include <stdint.h>
#include <QVector>



class Civilization
{
public:
    Civilization();

    //Accessors and Mutators
    void setCiv(int nation);
    int getCiv();
    void setLeader(int leader);
    int getLeader();

private:
    int nation;//Enumerations may be easier than reading strings?
    int leader;//Enumerations? or maybe tie this directly to nation?



    //QVector<Unit> UnitList; Needs a Units object type
        //Will contain all of the civ's units

    //QVector<City> citylist; Needs a City object type
        //Will contain all of the civ's Cities

    //QVector<Yield> totalyield; Needs a yield object type
        //Will contain the totals for the civ's yields
};

#endif // CIVILIZATION_H
