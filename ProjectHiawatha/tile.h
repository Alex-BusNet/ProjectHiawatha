#ifndef TILE_H
#define TILE_H

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

typedef struct {int column; int row;} TileID;

//class City;

class Tile
{
public:
    Tile(int posX, int posY);

    ~Tile();

    bool HasRoad; // Movement Cost calculation flag
    bool IsWorked; // Yield calculation flag
    bool ContainsUnit; // Civ spawning flag
    bool HasCity; // Civ spawning flag
    bool DiscoveredByPlayer; // Fog of War flag
    bool Checked; // Map Gen Flag
    bool Selected; // Unit selection Render flag
    bool Walkable; // Map Gen path finding flag

    TileType GetTileType();
    Yield* GetYield();
    TileImprovement GetTileImprovement();
    TileID GetTileID();
    Biome GetTileBiome();

    //Resource GetTileResource();
    //void SetTileResource();

    Nation GetControllingCiv();
    void SetControllingCiv(Nation civ);

    City *GetGoverningCity();
    void SetGoverningCity(City *city);

    void SetTilePen(QPen pen);
    QPen GetTilePen();

    void SetTileType(TileType type);
    void SetYield(int gold, int prod, int sci, int food, int cul);
    void SetTileImprovement(TileImprovement improvement);

    void SetTileID(int x, int column, Tile *tile);
    void SetTileID(TileID id);

    void SetMoveCost(int cost);
    int GetMoveCost();

    QString GetTileIDString();
    QString GetTileTypeString();

    QPoint *GetHexPoints();
    QPoint GetHexPoint(int index);
    QPoint GetCenter();
    QPoint GetTextCenter();
    QPointF GetTexturePoint();
    QPoint GetItemTexturePoint();
    QPoint GetCityLabelPoint();

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

    void SetCivListIndex(int index);
    int GetCivListIndex();

private:
    TileType type;
    Yield* yield;
    TileImprovement improvement;
    TileID tileID;
    QPixmap tileTexture;
    Biome biome;
    Nation owner;
    //Resource resource;
    QPen outlinePen;
    City *city;
    int moveCost;
    int occupyingCivListIndex;

    //==Hex tiles==
    QPoint points[7];
    QPoint center;
    QPoint textCenter;
    QPointF texturePoint;
    QPoint itemTexturePoint;
    QPoint cityLabelPoint;

    int hexPosX;
    int hexPosY;
    int hexVertOffset;
    int hexHorOffset;
    int hexRowOffset;

    QPolygon poly;
};

#endif // TILE_H
