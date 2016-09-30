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
int drawScale = 2;

Tile::Tile(int posX, int posY)
{
    this->type = WATER;
    this->yield = Yield(1,1,0,1,0);
    this->improvement = NONE;
    this->biome = EMPTY;

    IsWorked = false;
    HasRoad = false;
    HasCity = false;
    ContainsUnit = false;
    Selected = false;

    //Size of Hex tiles in px

    hexVertOffset = 22 * drawScale;
    hexHorOffset = 74 * drawScale;
    hexRowOffset = 37 * drawScale;

    posX *= drawScale;
    posY *= drawScale;

    points[0] = QPoint(posX, posY);
    points[1] = QPoint(posX + (25 * drawScale), posY);
    points[2] = QPoint((posX + hexRowOffset), (posY + hexVertOffset));
    points[3] = QPoint(posX + (25 * drawScale), posY + (hexVertOffset * 2));
    points[4] = QPoint(posX, posY + (hexVertOffset * 2));
    points[5] = QPoint(posX - (12 * drawScale), (posY + hexVertOffset));
    points[6] = QPoint(posX, posY);

    center = QPoint(posX + (12 * drawScale), posY + hexVertOffset);
    textCenter = QPoint(posX + 10, posY + hexVertOffset);
    texturePoint = QPoint(posX - (12 * drawScale), posY + (0 * drawScale));

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];

    this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/grass.png");

    Checked = false;
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

Biome Tile::GetTileBiome()
{
    return this->biome;
}

Nation Tile::GetControllingCiv()
{
    this->owner;
}

void Tile::SetControllingCiv(Nation civ)
{
    this->owner = civ;
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

void Tile::SetTileID(TileID id)
{
    this->tileID = id;
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

QString Tile::GetTileIDString()
{
    return QString("%1, %2").arg(this->GetTileID().column).arg(this->GetTileID().row);
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

QPointF Tile::GetTexturePoint()
{
    return this->texturePoint;
}

QPolygon Tile::GetTilePolygon()
{
    return this->poly;
}

QRect Tile::GetTileRect()
{
    return this->rect;
}

int Tile::GetHexPosX()
{
    return this->hexPosX;
}

int Tile::GetHexPosY()
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
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/water.png");
        break;
    case GRASS:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/grass.png");
        break;
    case DESERT:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/desert.png");
        break;
    case MOUNTAIN:
         this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/mountain.png");
        break;
    case ICE:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/ice.png");
        break;
    case COAST:
        break;
    case HILL:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/hill.png");
        break;
    case FOREST:
         this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/forest.png");
        break;
    default:
        this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/water_flat.png");
        break;
    }
}

void Tile::SetTileBiome(Biome biome)
{
    this->biome = biome;
}

void Tile::SetHexPos(int x, int y)
{
    x *= 10;
    y *= 10;
    this->points[0] = QPoint(x, y);
    this->points[1] = QPoint((x + (25 * drawScale)), y);
    this->points[2] = QPoint((x + 370), (y + (22 * drawScale)));
    this->points[3] = QPoint((x + (25 * drawScale)), (y + (44 * drawScale)));
    this->points[4] = QPoint(x, (y + (44 * drawScale)));
    this->points[5] = QPoint((x - (12 * drawScale)), (y + (22 * drawScale)));
    this->points[6] = QPoint(x, y);

    this->center = QPoint(posX + (12 * drawScale), posY + 22 * drawScale);
    this->textCenter = QPoint(posX + 10, posY + 22 * drawScale);
    this->texturePoint = QPoint(posX - (12 * drawScale), posY + (0 * drawScale));
}

void Tile::SetHexPoly()
{
    this->poly.clear();

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];
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


