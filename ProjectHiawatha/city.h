#ifndef CITY_H
#define CITY_H
#include <QVector>
#include <QWidget>
#include <QString>
#include "nation.h"
#include "tile.h"
#include "yield.h"

class Tile;
class Unit;

class City
{
public:
    City();

    //Accessor and Mutators
    void UpdateCityYield(Yield yield);

    void SetCityAsCaptial();
    void SetName(QString name);
    void SetCityTile(Tile *tile);
    void SetControllingCiv(Nation owner);
    void UpdateCityYield();

    void GarrisonWorker(Unit *worker);
    void GarrisonMilitary(Unit *military);
    void AddControlledTile(Tile* tile);

    QString GetName();
    Tile* GetCityTile();
    Yield* getCityYield();
    Nation GetControllingCiv();

    Unit GetUnitAt(int index);
    Unit* GetGarrisonedWorker(int index);
    Unit* GetGarrisonedMilitary(int index);

    bool IsCityCaptial();

    ~City();

private:
    QVector<Tile*> cityControlledTiles;
    QVector<Unit*> StationedWorkers;
    QVector<Unit*> StationedMilitary;

    QString name;
    Nation controllingCiv;
    Tile *cityTile;

    Yield* cityTotalYield;

    bool isCaptial;
    bool hasWorker;
    bool hasGarrison;

    int productionYield;
    int scienceYield;
    int goldYield;
};


#endif // CITY_H
