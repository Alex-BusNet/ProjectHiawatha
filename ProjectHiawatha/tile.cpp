#include "tile.h"
#include <QDebug>
#include <qmath.h>

//=====================
//     Tile Diagram
//       Flattop:
//     0/6------1
//     /         \
//    /           \
//   5      c      2
//    \           /
//     \         /
//      4-------3
//
//      Pointtop:
//         0/6
//        /   \
//       /     \
//      5       1
//      |       |
//      |   c   |
//      |       |
//      4       2
//       \     /
//        \   /
//          3
//======================
int drawScale = 2;

Tile::Tile(int _posX, int _posY)
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

    _posX *= drawScale;
    _posY *= drawScale;

    //Flattop:
//    points[0] = QPoint(_posX, _posY);
//    points[1] = QPoint(_posX + (25 * drawScale), _posY);
//    points[2] = QPoint((_posX + hexRowOffset), (_posY + hexVertOffset));
//    points[3] = QPoint(_posX + (25 * drawScale), _posY + (hexVertOffset * 2));
//    points[4] = QPoint(_posX, _posY + (hexVertOffset * 2));
//    points[5] = QPoint(_posX - (12 * drawScale), (_posY + hexVertOffset));
//    points[6] = QPoint(_posX, _posY);

//    center = QPoint(_posX + (12 * drawScale), _posY + hexVertOffset);
//    textCenter = QPoint(_posX + 10, _posY + hexVertOffset);
//    texturePoint = QPoint(_posX - (12 * drawScale), _posY + (0 * drawScale));

    //Pointtop:
    points[0] = QPoint(_posX + (22 * drawScale), _posY - (12 * drawScale));
    points[1] = QPoint(_posX + (44 * drawScale), _posY);
    points[2] = QPoint(_posX + (44 * drawScale), _posY + (25 * drawScale));
    points[3] = QPoint(_posX + (22 * drawScale), _posY + (37 * drawScale));
    points[4] = QPoint(_posX, _posY + (25 * drawScale));
    points[5] = QPoint(_posX, _posY);
    points[6] = QPoint(_posX + (22 * drawScale), _posY - (12 * drawScale));

    center = QPoint(_posX + (22 * drawScale), _posY + (12 * drawScale));
    textCenter = QPoint(_posX + (15 * drawScale), _posY + (10 * drawScale));
    texturePoint = QPointF(_posX, _posY - (12 * drawScale));
    itemTexturePoint = QPoint(_posX, _posY - (12 * drawScale));


    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];

    this->tileTexture = QPixmap("../ProjectHiawatha/Assets/Textures/grass.png");

    Checked = false;
    owner = NO_NATION;
    city = new City();
    outlinePen.setColor(QColor(255, 255, 255, 0));

    gCost = 0;
    hCost = 0;

    Walkable = true;
}

Tile::~Tile()
{

}

Unit *Tile::GetUnit()
{
    return this->occupyingUnit;
}

void Tile::SetUnit(Unit *unit)
{
    this->occupyingUnit = unit;
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
    return this->owner;
}

void Tile::SetControllingCiv(Nation civ)
{
    this->owner = civ;
}

City* Tile::GetGoverningCity()
{
    return this->city;
}

void Tile::SetGoverningCity(City *city)
{
    this->city = city;
}

void Tile::SetTilePen(QPen pen)
{
    this->outlinePen.setColor(pen.color());
}

QPen Tile::GetTilePen()
{
    return this->outlinePen;
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

QString Tile::GetTileIDString()
{
    return QString("%1, %2").arg(this->GetTileID().column).arg(this->GetTileID().row);
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

QPoint Tile::GetItemTexturePoint()
{
    return this->itemTexturePoint;
}

QPolygon Tile::GetTilePolygon()
{
    return this->poly;
}

int Tile::GetHexPosX()
{
    return this->hexPosX;
}

int Tile::GetHexPosY()
{
    return this->hexPosY;
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
        //This may or may not be added.
        //It is going to require some logic to get the right texture.
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

    this->center = QPoint(x + (12 * drawScale), y + 22 * drawScale);
    this->textCenter = QPoint(x + 10, y + 22 * drawScale);
    this->texturePoint = QPoint(x - (12 * drawScale), y + (0 * drawScale));
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

int Tile::fCost()
{
    return gCost + hCost;
}


