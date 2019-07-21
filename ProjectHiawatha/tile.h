#ifndef TILE_H
#define TILE_H

#include "datatypes.h"
#include "tileType.h"
#include "tileimprovement.h"
#include "biome.h"
#include "yield.h"
#include "civilization.h"
#include "nation.h"
#include "city.h"
#include <QPen>
#include <QPixmap>
#include <QPolygonF>
#include <QRect>
#include <stdint.h>
#include "resources.h"


class City;
class Unit;
class Yield;

class Tile
{
public:
    Tile(int posX, int posY);

    ~Tile();

    bool HasRoad; // Movement Cost calculation flag
    bool IsWorked; // Yield calculation flag
    //bool ContainsUnit(); // Civ spawning flag. Updated to be function - ARP 6/23/2019
    bool HasCity; // Civ spawning flag. Updated to be function - ARP 6/23/2019
    bool Checked; // Map Gen Flag
    bool Selected; // Unit selection Render flag
    bool Walkable; // Map Gen path finding flag
    bool DiscoveredByPlayer; // Fog of War flag
    bool IsSeenByPlayer; // Fog of War and Line of Sight flag
    bool CanAlwaysBeSeen;
    bool CanHaveFarm, CanHaveMine, CanHavePlantation, CanHavePasture, CanHaveTrade;
    bool CanHaveFishBoat, CanHaveOilWell, CanHaveCamp, CanHaveQuarry;

    int SeenByUnits;

    TileType GetTileType();
    Yield* GetYield();
    TileImprovement GetTileImprovement();
    TileID GetTileID();
    Biome GetTileBiome();

    //Resource GetTileResource();
    //void SetTileResource();

    Nation GetControllingCiv();
//    void SetControllingCiv(Nation civ, int civListIndex);

    City* GetGoverningCity();           // Changed from integer type to City* type - ARP 6/23/2019
    int GetGoverningCityID();
    void SetGoverningCity(City* city, int civListIndex);  // Changed from integer type to City* type - ARP 6/23/2019

    void SetTilePen(QPen pen);
    QPen GetTilePen();

    void SetTileType(TileType type);
    void SetYield(int gold, int prod, int sci, int food, int cul);
    void SetTileImprovement(TileImprovement improvement);

    void SetTileID(int x, int column, Tile *tile);
    void SetTileID(TileID id);

    void SetContainer(int container);
    void SetContinent(int continent);
    int GetContainer();
    int GetContinent();
    void SetMoveCost(int cost);
    int GetMoveCost();


    void SetWorkerButtons();
    unsigned int GetWorkerButtons();

    void SetTileIndex(int index);
    int GetTileIndex();
    QString GetTileIDString();
    QString GetTileTypeString();

    QPoint *GetHexPoints();
    QPoint GetHexPoint(int index);
    QPoint GetCenter();
    QPoint GetTextCenter();
    QPointF GetTexturePoint();
    QPoint GetItemTexturePoint();
    QPoint GetCityLabelPoint();
    QPoint GetTileImprovementPoint();
    QPoint GetResourceIconPoint();

    QPolygon GetTilePolygon();
    QRect GetTileRect();

    int GetHexPosX();
    int GetHexPosY();

    QPixmap GetTileTexture();
    void SetTileTexture(TileType type);
    void SetTileBiome(Biome biome);

    void SetHexPos(int x, int y);
    void SetHexPoly();

    void PrintHexPoints();

    int gCost, hCost;
    int fCost();
    TileID parent;

    //void SetOccupyingUnit(Unit* unit);
    void SetControllingCivListIndex(int index);

    //Unit* GetOccupyingUnit();
    //int GetOccupyingCivListIndex();
    int GetControllingCivListIndex();

    void SetResource(Strategic strat, Luxury lux);
    int GetStratResource();
    int GetLuxResource();

    void WriteTileSaveData(QJsonObject &obj) const;
    void ReadTileSaveData(const QJsonObject &obj);

private:
    TileType type;

    Yield* yield;

    TileImprovement improvement;

    TileID tileID;

    QPixmap tileTexture;

    Biome biome;

    Nation owner;

    QPen outlinePen;

    City* governingCity;       // Changed from integer type to City* type - ARP 6/23/2019
    int continent, container;

    int moveCost;
//    int occupyingCivListIndex;
    int controllingCivListIndex;
    int tileIndex;
    unsigned int workerButtons;

    //Unit* occupyingUnit;        // Replaces the 'occupyingCivListIndex' variable - ARP 6/23/2019
    Strategic stratResource;
    Luxury luxResource;

    //==Hex tiles==
    QPoint points[7];
    QPoint center;
    QPoint textCenter;
    QPointF texturePoint;
    QPoint itemTexturePoint;
    QPoint cityLabelPoint;
    QPoint resourceIconPoint;
    QPoint tileImprovementIconPoint;

    int hexPosX;
    int hexPosY;
    int hexVertOffset;
    int hexHorOffset;
    int hexRowOffset;

    QPolygon poly;
    QRect cirlceRect;
};

#endif // TILE_H
