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

    enum Focus {GOLD_FOCUS, PRODUCTION_FOCUS, SCIENCE_FOCUS, FOOD_FOCUS, CULTURE_FOCUS};

    //Accessor and Mutators
    void UpdateCityYield(Yield yield);

    void SetCityAsCaptial();
    void SetName(QString name);
    void SetCityTile(Tile *tile);
    void SetControllingCiv(Nation owner);
    void SetCityIndex(int index);
    void UpdateCityYield();
    void SetCityHealthBarIndex(int index);
    void SetCityProductionBarIndex(int index);
    void SetCityGrowthBarIndex(int index);
    void SetCityHealth(int health);
    void SetCityMaxHealth(int health);

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
    int GetCityGrowthBarIndex();
    int GetCityProductionBarIndex();
    int GetCityHealth();
    int GetMaxHealth();
    bool IsCityCaptial();

    void setCapitolConnection(bool flag);
    bool getCapitolConnection();

    void SetCitizenCount(int count);
    int GetCitizenCount();
    int GetFoodNeededToGrow();
    int GetTurnsToNewCitizen();
    int GetFoodSurplus();
    int getCurrentProductionCost();
    void setCurrentProductionCost(int cost);
    int getAccumulatedProduction();
    void setAccumulatedProduction(int total);
    void resetAccumulatedProduction();
    void setProductionName(QString name);
    void setProductionIndex(int index);
    void setProductionFinished(bool isFinished);
    bool getProductionFinished();
    void setIsUnit(bool isUnit);
    bool getIsUnit();
    int getProductionIndex();
    Unit* getProducedUnit();
    void setProducedUnit(Unit* unit);
    QString getProductionName();
    bool getHasWorker();
    void    loadUnits(QString filename);
    ~City();
    QVector<Unit*> getInitialUnitList();
    void SortControlledTiles();
    void SortTileQueue();
    QVector<Tile*> tileQueue;

    Update_t UpdateProgress();

    void SetCityFocus(Focus focus);
    Focus GetCityFocus();

private:
    enum SortOrder {LRBT, LRTB, RLTB, RLBT};
    QVector<Tile*> cityControlledTiles;
    QVector<Unit*> StationedWorkers;
    QVector<Unit*> StationedMilitary;
    QVector<Unit*> initialUnitList;
    Unit*   producedUnit;
    QString name;
    QString currentProductionName;
    Nation controllingCiv;
    Tile *cityTile;
    Focus cityFocus;

    QVector<Tile*> borderTiles;

    Yield* cityTotalYield;

    bool isCaptial;
    bool hasWorker;
    bool hasGarrison;
    bool hasCapitolConnection;
    bool productionFinished;
    bool productionUnit;

    int productionYield;
    int scienceYield;
    int goldYield;

    int cityIndex;
    int cityHealthBarIndex;
    int cityBordersIndex;
    int cityProductionBarIndex;
    int cityGrowthBarIndex;

    int growthCost;
    int foodSurplus;
    int citizens;
    int turnsToBorderGrowth;
    int turnsToNewCitizen;

    int currentProductionCost;
    int accumulatedProduction;
    int productionIndex;

    int cityHealth, maxHealth;

    int orientation(QPoint p, QPoint q, QPoint r);

    void FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, SortOrder sortOrder);
    QPolygon cityBorder;

};


#endif // CITY_H
