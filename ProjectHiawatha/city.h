#ifndef CITY_H
#define CITY_H

#include <QVector>
#include <QWidget>
#include <QString>
#include <QPoint>
#include <QPolygon>
#include <queue>
#include "nation.h"
#include "building.h"
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
    void SetCityAsCapital(bool capitalStatus, bool originalCapital);
    void SetName(QString name);
    void SetCityTile(Tile *tile);
    void SetControllingCiv(Nation owner);
    void SetCityIndex(int index);

    void UpdateCityYield();
    void UpdateCityStatus();

    void SetCityRenderIndex(int index);
    void SetCityBorders(QPolygon borders);
    void SetCityHealth(float health);
    void SetCityMaxHealth(int health);

    void SetCityStrength(int strength);
    void SetCityBuildingStrength(int strength);
    void SetBaseCityStrength(int strength);

    void GarrisonWorker(Unit *worker);
    void RemoveGarrisonWorker();
    void GarrisonMilitary(Unit *military);
    void RemoveGarrisonMilitary();

    void AddControlledTile(Tile* tile);
    void SetMaximumExpansionBorderTiles(QList<Tile*> tileVect);
    void DefineCityBorders(bool redefine);
    void setCapitolConnection(bool flag);

    void SetCitizenCount(int count);
    void SetStagnant(bool isStagnant);
    void SetGrowthCost(int cost);
    void SetFoodSurplus(int surplus);
    void SetBuildingStrength(int strength);
    void SetFullyExpanded(bool fullExpansion);

    void setCurrentProductionCost(int cost);
    void setAccumulatedProduction(int total);
    void resetAccumulatedProduction();
    void setProductionName(QString name);
    void setProductionIndex(int index);
    void setProductionFinished(bool isFinished);
    void setIsUnit(bool isUnit);
    void setProducedUnit(Unit* unit);

    void loadUnits(QString filename);
    void loadBuildings(QString filename);

    void addBuilding(Building* building);
    void IncrementNumberOfBuildings();

    void SortControlledTiles();
    void SortTileQueue();
    void FilterMEBList();

    void SetCityFocus(Focus focus);
    void InitializeCity();

    QString GetName();
    QString getProductionName();

    Tile* GetCityTile();
    Yield* getCityYield();
    Nation GetControllingCiv();

    QPolygon GetCityBorders();
    QPolygon GetMaximumExpansionBorder();
    QPolygon GetMinimumSettleDistance();

    Unit GetUnitAt(int index);
    Unit* GetGarrisonedWorker();
    Unit* GetGarrisonedMilitary();
    Unit* getProducedUnit();

    QVector<Tile*> GetControlledTiles();
    QList<Tile *> GetMaximumExpansionBorderTiles();
    QVector<Tile*> tileQueue;
    QVector<Unit*> getInitialUnitList();
    QVector<Building*> getInitialBuildingList();
    QVector<Building*> getCurrentBuildingList();

    Update_t UpdateProgress();
    Focus GetCityFocus();

    int GetCityIndex();
    int GetCityRenderIndex();
    int GetCityHealth();
    int GetMaxHealth();
    int GetCityStrength();
    int GetCityBuildingStrength();
    int GetBaseStrength();
    int GetCitizenCount();
    int GetFoodNeededToGrow();
    int GetTurnsToNewCitizen();
    int GetFoodSurplus();
    int getCurrentProductionCost();
    int getAccumulatedProduction();
    int getProductionIndex();
    int getNumberOfBuildings();
    int GetTurnsToBorderGrowth();

    bool IsCityCaptial();
    bool IsOriginalCapital();
    bool getCapitolConnection();
    bool IsInitialized();
    bool IsStagnant();
    bool IsFullyExpanded();
    bool getProductionFinished();
    bool getIsUnit();
    bool getHasWorker();
    bool HasGarrisonUnit();
    bool MSDIntersects(QPolygon targetMSD);

    ~City();

private:
    enum SortOrder {LRBT, LRTB, RLTB, RLBT};

    QVector<Tile*> cityControlledTiles;
    Unit* StationedWorkers;
    Unit* StationedMilitary;
    QVector<Unit*> initialUnitList;
    QVector<Building*> initialBuildingList;
    QVector<Building*> producedBuildings;
    QList<Tile*> maximumExpansionBorderTiles;
    Unit*   producedUnit;
    QString name;
    QString currentProductionName;
    Nation controllingCiv;
    Tile *cityTile;
    Focus cityFocus;

    QVector<Tile*> borderTiles;

    Yield* cityTotalYield;

    bool isCaptial, isOriginalCapital;
    bool hasWorker;
    bool hasGarrison;
    bool hasCapitolConnection;
    bool productionFinished;
    bool productionUnit;
    bool initialized;
    bool stagnant;
    bool fullyExpanded;

    int productionYield;
    int scienceYield;
    int goldYield;

    int cityIndex;
    int cityRenderIndex;

    int growthCost;
    int foodSurplus;
    int citizens;
    int turnsToBorderGrowth;
    int turnsToNewCitizen;

    int currentProductionCost;
    int accumulatedProduction;
    int productionIndex;

    int numberofBuildings;
    int cityStrength, baseStrength, buildingStrength;

    float cityHealth, maxHealth;

    int orientation(QPoint p, QPoint q, QPoint r);

    void FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, SortOrder sortOrder);
    QPolygon cityBorder, maximumExpansionBorder, minimumSettleDistance;

};


#endif // CITY_H
