#ifndef CITY_H
#define CITY_H
#include <QVector>
#include <QWidget>
#include <QString>
#include "nation.h"
#include "tile.h"
#include "yield.h"
#include <QPoint>
#include <QPolygon>

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
    void SetCityIndex(int index);
    void UpdateCityYield();

    void GarrisonWorker(Unit *worker);
    void GarrisonMilitary(Unit *military);
    void AddControlledTile(Tile* tile);

    void DefineCityBorders();

    QString GetName();
    Tile* GetCityTile();
    Yield* getCityYield();
    Nation GetControllingCiv();
    QVector<Tile*> GetControlledTiles();
    QPolygon GetCityBorders();

    Unit GetUnitAt(int index);
    Unit* GetGarrisonedWorker(int index);
    Unit* GetGarrisonedMilitary(int index);

    int GetCityIndex();
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
    int cityIndex;

    void TileToGetNext();
    int orientation(QPoint p, QPoint q, QPoint r);

    void FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, bool reverseSort);
    QPolygon cityBorder;
};


#endif // CITY_H
