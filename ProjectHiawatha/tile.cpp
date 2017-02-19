#include "tile.h"
#include <QBitmap>
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
    this->yield = new Yield(0,0,0,0,0);
    this->improvement = NONE;
    this->biome = EMPTY;
    this->stratResource = NO_STRATEGIC;
    this->luxResource = NO_LUXURY;

    IsWorked = false;
    HasRoad = false;
    HasCity = false;
    ContainsUnit = false;
    Selected = false;
    DiscoveredByPlayer = false;
    IsSeenByPlayer = false;
    CanAlwaysBeSeen = false;

    //Tile Improvement flags
    CanHaveFarm = false;
    CanHaveMine = false;
    CanHavePasture = false;
    CanHavePlantation = false;
    CanHaveTrade = false;
    CanHaveFishBoat = false;
    CanHaveCamp = false;
    CanHaveOilWell = false;
    CanHaveQuarry = false;

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
    itemTexturePoint = QPoint(_posX + (13 * drawScale), _posY - (3 * drawScale));
    cityLabelPoint = QPoint(_posX + (12 * drawScale), _posY - (10 * drawScale));
    resourceIconPoint = QPoint(_posX + (7 * drawScale), _posY + (17 * drawScale));
    tileImprovementIconPoint = QPoint(_posX + (30 * drawScale), _posY + (20 * drawScale));

    this->poly << this->points[0]
            << this->points[1]
            << this->points[2]
            << this->points[3]
            << this->points[4]
            << this->points[5]
            << this->points[6];

    this->tileTexture = QPixmap("Assets/Textures/Scaled/water.png");

    this->cirlceRect = QRect(_posX + (2 * drawScale), _posY - (8 * drawScale), 80, 80);

    Checked = false;
    owner = NO_NATION;
    this->governingCity = -1;
    outlinePen.setColor(QColor(255, 255, 255, 0));
    occupyingCivListIndex = -1;
    controllingCivListIndex = -1;
    continent = 0;
    container = 0;

    gCost = 0;
    hCost = 0;

    Walkable = true;
}

Tile::~Tile()
{
    if(yield != NULL)
        delete yield;
}

TileType Tile::GetTileType()
{
    return this->type;
}

Yield* Tile::GetYield()
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

void Tile::SetControllingCiv(Nation civ, int civListIndex)
{
    this->owner = civ;
    this->controllingCivListIndex = civListIndex;
    if(controllingCivListIndex == 0)
        CanAlwaysBeSeen = true;

    if(civListIndex == -1)
    {
        this->CanAlwaysBeSeen = false;
        this->DiscoveredByPlayer = false;
        this->IsSeenByPlayer = false;
        this->governingCity = -1;
    }
}

int Tile::GetGoverningCity()
{
    return this->governingCity;
}

void Tile::SetGoverningCity(int cityID)
{
    this->governingCity = cityID;
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

void Tile::SetYield(int gold, int prod, int sci, int food, int cul)
{
    this->yield->ChangeYield(gold, prod, sci, food, cul);
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

void Tile::SetContainer(int container)
{
    this->container = container;
}

void Tile::SetContinent(int continent)
{
    this->continent = continent;
}

int Tile::GetContainer()
{
    return this->container;
}

int Tile::GetContinent()
{
    return this->continent;
}

void Tile::SetMoveCost(int cost)
{
    this->moveCost = cost;
}

int Tile::GetMoveCost()
{
    return this->moveCost;
}

void Tile::SetTileIndex(int index)
{
    this->tileIndex = index;
}

int Tile::GetTileIndex()
{
    return this->tileIndex;
}

QString Tile::GetTileIDString()
{
    return QString("%1, %2").arg(this->GetTileID().column).arg(this->GetTileID().row);
}

QString Tile::GetTileTypeString()
{
    switch(this->type)
    {
    case WATER:
        return QString("Water");
        break;
    case GRASS:
        return QString("Grass");
        break;
    case DESERT:
        return QString("Desert");
        break;
    case MOUNTAIN:
        return QString("Mountain");
        break;
    case ICE:
        return QString("Ice");
        break;
    case COAST:
        //This may or may not be added.
        //It is going to require some logic to get the right texture.
        return QString("Coast");
        break;
    case HILL:
        return QString("Hill");
        break;
    case FOREST:
        return QString("Forest");
        break;
    case PLAINS_TILE:
        return QString("Plains");
        break;
    case SNOW:
        return QString("Snow");
        break;
    default:
        return QString(" ");
        break;
    }
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

QPoint Tile::GetCityLabelPoint()
{
    return this->cityLabelPoint;
}

QPoint Tile::GetTileImprovementPoint()
{
    return this->tileImprovementIconPoint;
}

QPoint Tile::GetResourceIconPoint()
{
    return this->resourceIconPoint;
}

QPolygon Tile::GetTilePolygon()
{
    return this->poly;
}

QRect Tile::GetTileRect()
{
    return this->cirlceRect;
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
        this->tileTexture = QPixmap("Assets/Textures/Scaled/water.png");
        CanHaveFishBoat = true;
        break;
    case GRASS:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/grass.png");
        CanHaveFarm = true;
        CanHaveTrade = true;
        break;
    case DESERT:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/desert.png");
        CanHaveFarm = true;
        CanHaveTrade = true;
        break;
    case MOUNTAIN:
         this->tileTexture = QPixmap("Assets/Textures/Scaled/mountain.png");
        break;
    case ICE:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/snow.png");
        break;
    case COAST:
        //This may or may not be added.
        //It is going to require some logic to get the right texture.
        break;
    case HILL:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/hills.png");
        CanHaveFarm = true;
        CanHaveTrade = true;
        CanHaveMine = true;
        break;
    case FOREST:
         this->tileTexture = QPixmap("Assets/Textures/Scaled/forest.png");
        CanHaveFarm = true;
        CanHaveTrade = true;
        break;
    case SNOW:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/snow.png");
        break;
    case PLAINS_TILE:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/plains.png");
        CanHaveFarm = true;
        CanHaveTrade = true;
        break;
    default:
        this->tileTexture = QPixmap("Assets/Textures/Scaled/water.png");
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
    this->points[2] = QPoint((x + 37), (y + (22 * drawScale)));
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

void Tile::SetOccupyingCivListIndex(int index)
{
    this->occupyingCivListIndex = index;
}

void Tile::SetControllingCivListIndex(int index)
{
    this->controllingCivListIndex = index;
}

int Tile::GetOccupyingCivListIndex()
{
    return this->occupyingCivListIndex;
}

int Tile::GetControllingCivListIndex()
{
    return this->controllingCivListIndex;
}

void Tile::SetResource(Strategic strat, Luxury lux)
{
    this->stratResource = strat;
    this->luxResource = lux;
}

int Tile::GetStratResource()
{
    return this->stratResource;
}

int Tile::GetLuxResource()
{
    return this->luxResource;
}

void Tile::WriteTileSaveData(QJsonObject &obj) const
{
    obj["tileIndex"] = tileIndex;
    obj["type"] = type;
    obj["biome"] = biome;
    obj["container"] = container;
    obj["continent"] = continent;
    obj["strategicresource"] = stratResource;
    obj["luxuryresource"] = luxResource;

    QJsonObject yo;
    yield->WriteYieldSaveData(yo);

    obj["yield"] = yo;
    obj["tileworked"] = IsWorked;
    obj["improvement"] = improvement;
    obj["hascity"] = HasCity;
    obj["containsunit"] = ContainsUnit;
    obj["occupyingcivlistindex"] = occupyingCivListIndex;
    obj["controllingcivlistindex"] = controllingCivListIndex;
    obj["governedby"] = governingCity;
    obj["owner"] = owner;
    obj["movecost"] = moveCost;
    obj["canalwaysbeseen"] = CanAlwaysBeSeen;
    obj["isseenbyplayer"] = IsSeenByPlayer;
    obj["discoveredbyplayer"] = DiscoveredByPlayer;
    obj["walkable"] = Walkable;
}

void Tile::ReadTileSaveData(const QJsonObject &obj)
{
    type = static_cast<TileType>(obj["type"].toInt());
    biome = static_cast<Biome>(obj["biome"].toInt());
    container = obj["container"].toInt();
    continent = obj["continent"].toInt();
    stratResource = static_cast<Strategic>(obj["strategicresource"].toInt());
    luxResource = static_cast<Luxury>(obj["luxuryresource"].toInt());

    QJsonObject yo = obj["yield"].toObject();
    yield->ReadYieldSaveData(yo);

    IsWorked = obj["tileworked"].toBool();
    improvement = static_cast<TileImprovement>(obj["improvement"].toInt());
    HasCity = obj["hascity"].toBool();
    ContainsUnit = obj["containsunit"].toBool();
    occupyingCivListIndex = obj["occupyingcivlistindex"].toInt();
    controllingCivListIndex = obj["controllingcivlistindex"].toInt();
    governingCity = obj["governedby"].toInt();
    owner = static_cast<Nation>(obj["owner"].toInt());
    moveCost = obj["movecost"].toInt();
    CanAlwaysBeSeen = obj["canalwaysbeseen"].toBool();
    DiscoveredByPlayer = obj["discoveredbyplayer"].toBool();
    IsSeenByPlayer = obj["isseenbyplayer"].toBool();

    Walkable = obj["walkable"].toBool();

    this->SetTileTexture(type);
}


