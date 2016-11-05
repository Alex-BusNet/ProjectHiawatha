#ifndef CITY_H
#define CITY_H

#include <QVector>
#include <QWidget>
#include <QString>
#include <QPoint>
#include <QPolygon>
#include <queue>
#include "nation.h"
#include "tile.h"
#include "yield.h"
#include "datatypes.h"

class Tile;
class Unit;

class City
{
public:
    City();

    enum focus {GOLD_FOCUS, PRODUCTION_FOCUS, SCIENCE_FOCUS, FOOD_FOCUS, CULTURE_FOCUS};

    //Accessor and Mutators
    void UpdateCityYield(Yield yield);

    void SetCityAsCaptial();
    void SetName(QString name);
    void SetCityTile(Tile *tile);
    void SetControllingCiv(Nation owner);
    void SetCityIndex(int index);
    void UpdateCityYield();
    void SetCityHealthBarIndex(int index);

    void GarrisonWorker(Unit *worker);
    void GarrisonMilitary(Unit *military);
    void AddControlledTile(Tile* tile);

    void DefineCityBorders(bool redefine);
    void SetCityBordersIndex(int index);

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
    int GetCityHealthBarIndex();
    int GetCityBordersIndex();
    bool IsCityCaptial();

    void setCapitolConnection(bool flag);
    bool getCapitolConnection();

    void SetCitizenCount(int count);
    int GetCitizenCount();

    bool getHasWorker();

    ~City();

    void SortTileQueue();
    QVector<Tile*> tileQueue;

    Update_t UpdateProgress();
private:
    QVector<Tile*> cityControlledTiles;
    QVector<Unit*> StationedWorkers;
    QVector<Unit*> StationedMilitary;

    QString name;
    Nation controllingCiv;
    Tile *cityTile;

    QVector<Tile*> borderTiles;

    Yield* cityTotalYield;

    bool isCaptial;
    bool hasWorker;
    bool hasGarrison;
    bool hasCapitolConnection;

    int productionYield;
    int scienceYield;
    int goldYield;
    int cityIndex;
    int cityHealthBarIndex;
    int cityBordersIndex;
    int turnsToBorderGrowth;
    int citizens;
    int turnsToNewCitizen;

    int orientation(QPoint p, QPoint q, QPoint r);

    void FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, bool reverseSort);
    QPolygon cityBorder;

};


#endif // CITY_H
