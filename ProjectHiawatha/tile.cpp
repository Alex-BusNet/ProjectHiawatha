#include "tile.h"
#include <QDebug>
#include <qmath.h>

//=====================
//    Tile Diagram
//
//     0/6------1
//     /         \
//    /           \
//   5      c      2
//    \           /
//     \         /
//      4-------3
//
//======================

Tile::Tile()
{
    this->type = GRASS;
    this->yield = Yield();
    this->improvement = NONE;

    IsWorked = false;
    HasRoad = false;
    HasCity = false;
    ContainsUnit = false;

    //size of square tiles in px
    sizeX = 20;
    sizeY = 20;

    posX = 0;
    posY = 0;

    rect = QRect(posX, posY, sizeX, sizeY);

    //Size of Hex tiles in px
    hexPosX = 12;
    hexPosY = 0.0;
    hexVertOffset = 22;
    hexHorOffset = 74;
    hexRowOffset = 37;

    points[0] = QPoint(hexPosX, hexPosY);
    points[1] = QPoint((hexPosX + 25), hexPosY);
    points[2] = QPoint((hexPosX + 37), (hexPosY + 22));
    points[3] = QPoint((hexPosX + 25), (hexPosY + 44));
    points[4] = QPoint(hexPosX, (hexPosY + 44));
    points[5] = QPoint((hexPosX - 12), (hexPosY + 22));
    points[6] = QPoint(hexPosX, hexPosY);

    center = QPoint(hexPosX + 12, hexPosY + 22);
    textCenter = QPoint(hexPosX + 1, hexPosY + 22);

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];

    this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/grass.png");
}

Tile::Tile(TileType type)
{
    qDebug() << "Tile(TileType) c'tor called";
    this->type = type;
    this->yield = Yield();
    this->improvement = NONE;

    IsWorked = false;
    HasRoad = false;
    HasCity = false;
    ContainsUnit = false;

    //size of square tiles in px
    sizeX = 20;
    sizeY = 20;

    posX = 0;
    posY = 0;

    rect = QRect(posX, posY, sizeX, sizeY);

    //Size of Hex tiles in px
    hexPosX = 12;
    hexPosY = 0.0;
    hexVertOffset = 22;
    hexHorOffset = 74;
    hexRowOffset = 37;

    points[0] = QPoint(hexPosX, hexPosY);
    points[1] = QPoint((hexPosX + 25), hexPosY);
    points[2] = QPoint((hexPosX + 37), (hexPosY + 22));
    points[3] = QPoint((hexPosX + 25), (hexPosY + 44));
    points[4] = QPoint(hexPosX, (hexPosY + 44));
    points[5] = QPoint((hexPosX - 12), (hexPosY + 22));
    points[6] = QPoint(hexPosX, hexPosY);

    center = QPoint(hexPosX + 12, hexPosY + 22);
    textCenter = QPoint(hexPosX + 1, hexPosY + 22);

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];
}

Tile::Tile(TileType type, Yield yield)
{
    qDebug() << "Tile(TileType, Yield) c'tor called";
    this->type = type;
    this->yield = yield;
    this->improvement = NONE;

    IsWorked = false;
    HasRoad = false;
    HasCity = false;
    ContainsUnit = false;

    //size of square tiles in px
    sizeX = 20;
    sizeY = 20;

    posX = 0;
    posY = 0;

    rect = QRect(posX, posY, sizeX, sizeY);

    //Size of Hex tiles in px
    hexPosX = 12;
    hexPosY = 0.0;
    hexVertOffset = 22;
    hexHorOffset = 74;
    hexRowOffset = 37;

    points[0] = QPoint(hexPosX, hexPosY);
    points[1] = QPoint((hexPosX + 25), hexPosY);
    points[2] = QPoint((hexPosX + 37), (hexPosY + 22));
    points[3] = QPoint((hexPosX + 25), (hexPosY + 44));
    points[4] = QPoint(hexPosX, (hexPosY + 44));
    points[5] = QPoint((hexPosX - 12), (hexPosY + 22));
    points[6] = QPoint(hexPosX, hexPosY);

    center = QPoint(hexPosX + 12, hexPosY + 22);
    textCenter = QPoint(hexPosX + 1, hexPosY + 22);

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];
}

Tile::~Tile()
{

}

TileType Tile::GetTileType()
{
    return this->type;
}

Yield Tile::GetYield()
{
    return this->yield;
}

TileImprovement Tile::GetTileImprovement()
{
    return this->improvement;
}

TileID Tile::GetTileID()
{
    return this->tileID;
}

void Tile::SetTileType(TileType type)
{
    this->type = type;
}

void Tile::SetYield(Yield yield)
{
    this->yield = yield;
}

void Tile::SetTileImprovement(TileImprovement improvement)
{
    this->improvement = improvement;
}

void Tile::SetTileID(int row, int column, Tile *tile)
{
    tile->tileID.row = row;
    tile->tileID.column = column;
}

int Tile::GetSizeX()
{
    return this->sizeX;
}

int Tile::GetSizeY()
{
    return this->sizeY;
}

int Tile::GetPosX()
{
    return this->posX;
}

int Tile::GetPosY()
{
    return this->posY;
}

void Tile::SetPosX(int x)
{
    this->posX = x;
}

void Tile::SetPosY(int y)
{
    this->posY = y;
}

QPoint* Tile::GetHexPoints()
{
    return this->points;
}

QPoint Tile::GetHexPoint(int index)
{
    return this->points[index];
}

QPoint Tile::GetCenter()
{
    return this->center;
}

QPoint Tile::GetTextCenter()
{
    return this->textCenter;
}

QPolygon Tile::GetTilePolygon()
{
    return this->poly;
}

QRect Tile::GetTileRect()
{
    return this->rect;
}

float Tile::GetHexPosX()
{
    return this->hexPosX;
}

float Tile::GetHexPosY()
{
    return this->hexPosY;
}

float Tile::GetHexHorOffset()
{
    return this->hexHorOffset;
}

float Tile::GetHexVertOffset()
{
    return this->hexVertOffset;
}

float Tile::GetHexRowOffset()
{
    return this->hexRowOffset;
}

QPixmap Tile::GetTileTexture()
{
    return this->tileTexture;
}

void Tile::SetTileTexture(TileType type)
{
    switch(type)
    {
    case WATER:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/TexturesTerrainHex.dds");
        break;
    case GRASS:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/grass.png");
        break;
    default:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/TexturesTerrainHex.dds");
        break;
    }
}

void Tile::SetHexPos(int x, int y)
{
    this->points[0] = QPoint(x, y);
    this->points[1] = QPoint((x + 25), y);
    this->points[2] = QPoint((x + 37), (y + 22));
    this->points[3] = QPoint((x + 25), (y + 44));
    this->points[4] = QPoint(x, (y + 44));
    this->points[5] = QPoint((x - 12), (y + 22));
    this->points[6] = QPoint(x, y);

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];

    this->center = QPoint(x + 12, y + 22);
    this->textCenter = QPoint(x + 1, y + 20);

}

void Tile::PrintHexPoints()
{
    qDebug() << "0: " << this->points[0];
    qDebug() << "1: " << this->points[1];
    qDebug() << "2: " << this->points[2];
    qDebug() << "3: " << this->points[3];
    qDebug() << "4: " << this->points[4];
    qDebug() << "5: " << this->points[5];
    qDebug() << "6: " << this->points[6];
    qDebug() << "";
}


