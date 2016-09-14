#ifndef TILE_H
#define TILE_H
#include "tileType.h"
#include "tileimprovement.h"
#include "yield.h"
#include <QPixmap>
#include <QPolygonF>
#include <QRect>
#include <stdint.h>

typedef struct {int column; int row;} TileID;

class Tile
{
public:

    Tile(TileType type);
    Tile(TileType type, Yield yield);
    Tile(int posX, int posY);
    ~Tile();

    bool HasRoad;
    bool IsWorked;
    bool ContainsUnit;
    bool HasCity;

    //Unit GetUnit();
    //void SetUnit();

    //City GetCity();
    //void SetCity();

    TileType GetTileType();
    Yield GetYield();
    TileImprovement GetTileImprovement();
    TileID GetTileID();

    //Resource GetTileResource();
    //void SetTileResource();

    //Civ GetControllingCiv();
    //void SetControllingCiv();

    //City getGoverningCity();
    //void setGoverningCity();

    void SetTileType(TileType type);
    void SetYield(Yield yield);
    void SetTileImprovement(TileImprovement improvement);
    void SetTileID(int x, int column, Tile *tile);

    int GetSizeX();
    int GetSizeY();
    int GetPosX();
    int GetPosY();

    void SetPosX(int x);
    void SetPosY(int y);

    QPoint *GetHexPoints();
    QPoint GetHexPoint(int index);
    QPoint GetCenter();
    QPoint GetTextCenter();

    QPolygon GetTilePolygon();
    QRect GetTileRect();

    float GetHexPosX();
    float GetHexPosY();
    float GetHexHorOffset();
    float GetHexVertOffset();
    float GetHexRowOffset();

    QPixmap GetTileTexture();
    void SetTileTexture(TileType type);

    void SetHexPos(int x, int y);
    void SetHexPoly();

    void PrintHexPoints();

private:
    //=========================
    // DO NOT USE THIS C'TOR
    //
    Tile();
    //
    //=========================

    TileType type;
    Yield yield;
    TileImprovement improvement;
    TileID tileID;
    QPixmap tileTexture;
    //Resource resource;

    //==Square Tiles==
    int sizeX;
    int sizeY;
    int posX;
    int posY;
    QRect rect;

    //==Hex tiles==
    QPoint points[7];
    QPoint center;
    QPoint textCenter;

    int hexPosX;
    int hexPosY;
    int hexVertOffset;
    int hexHorOffset;
    int hexRowOffset;

    QPolygon poly;

};

#endif // TILE_H
