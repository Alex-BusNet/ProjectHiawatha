#ifndef TILE_H
#define TILE_H
#include "tileType.h"
#include "tileimprovement.h"
#include "biome.h"
#include "yield.h"
#include "civilization.h"
#include "nation.h"
#include "city.h"
#include "unit.h"
#include <QPen>
#include <QPixmap>
#include <QPolygonF>
#include <QRect>
#include <stdint.h>

typedef struct {int column; int row;} TileID;


class City;

class Tile
{
public:
    Tile(int posX, int posY);

    ~Tile();

    bool HasRoad;
    bool IsWorked;
    bool ContainsUnit;
    bool HasCity;
    bool DiscoveredByPlayer; // Fog of War flag
    bool Checked;
    bool Selected;

    Unit GetUnit();
    void SetUnit(Unit unit);

    TileType GetTileType();
    Yield GetYield();
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
    void SetYield(Yield yield);
    void SetTileImprovement(TileImprovement improvement);
    void SetTileID(int x, int column, Tile *tile);
    void SetTileID(TileID id);

    int GetSizeX();
    int GetSizeY();
    int GetPosX();
    int GetPosY();
    QString GetTileIDString();

    void SetPosX(int x);
    void SetPosY(int y);

    QPoint *GetHexPoints();
    QPoint GetHexPoint(int index);
    QPoint GetCenter();
    QPoint GetTextCenter();
    QPointF GetTexturePoint();

    QPolygon GetTilePolygon();
    QRect GetTileRect();

    int GetHexPosX();
    int GetHexPosY();
    float GetHexHorOffset();
    float GetHexVertOffset();
    float GetHexRowOffset();

    QPixmap GetTileTexture();
    void SetTileTexture(TileType type);
    void SetTileBiome(Biome biome);

    void SetHexPos(int x, int y);
    void SetHexPoly();

    void PrintHexPoints();

private:
    TileType type;
    Yield yield;
    TileImprovement improvement;
    TileID tileID;
    QPixmap tileTexture;
    Biome biome;
    Nation owner;
    //Resource resource;
    QPen outlinePen;
    City *city;

    //==Hex tiles==
    QPoint points[7];
    QPoint center;
    QPoint textCenter;
    QPoint texturePoint;

    int hexPosX;
    int hexPosY;
    int hexVertOffset;
    int hexHorOffset;
    int hexRowOffset;

    QPolygon poly;
};

#endif // TILE_H
