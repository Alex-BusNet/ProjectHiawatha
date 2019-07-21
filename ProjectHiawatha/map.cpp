#include "map.h"
#include <QDebug>
#include <map>
#include <random>
#include <QTime>
#include <ctime>
#include "biome.h"
#include "unittype.h"
#include <QJsonArray>
#include <queue>

Map::Map()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 20; //mapSizeX is doubled for map creation. i.e. a map size of 20 tiles will yield 40 columns.
    mapSizeY = 24;

    //Map Sizes according to Civ V:
    // Duel: 40x24 -- 2 Players
    // Tiny: 56x36 -- 4 Players
    // Small: 66x42 -- 6 Players
    // Standard: 80x52 -- 8 Players
    // Large: 104x64 -- 10 Players
    // Huge: 128x80 -- 12 Players
}

Map::Map(int mapSizeX, int mapSizeY)
{
    this->mapSizeX = mapSizeX * 2;
    this->mapSizeY = mapSizeY;


    // The Ocean scale factor is used to determine
    // how many columns should be created
    // at the center and edges of the map
    switch(mapSizeX)
    {
    case 20:
        this->oceanScaleFactor = 3;
        break;
    case 28:
        this->oceanScaleFactor = 4;
        break;
    case 33:
        this->oceanScaleFactor = 5;
        break;
    case 40:
        this->oceanScaleFactor = 6;
        break;
    case 52:
        this->oceanScaleFactor = 7;
        break;
    case 64:
        this->oceanScaleFactor = 8;
        break;
    default:
        this->oceanScaleFactor = 3;
        break;
    }
}

Map::~Map()
{
    if(!terrain.isEmpty())
    {
        foreach(QPixmap* p, terrain)
        {
            if(p != NULL)
                delete p;
        }
    }

    if(!board.isEmpty())
    {
        foreach(MapData* md, board)
        {
            if(md->tile != NULL)
                delete md->tile;
        }
    }

}

void Map::InitHexMap()
{
    GenerateTiles();
    GenerateBiomes();
    GenerateMapEdge();
    GenerateMap();
    GenerateResources();
    CleanMap();
    InitTerrain();
}

void Map::InitTerrain()
{
    for(int i = 0; i < board.size(); i++)
    {
        QPixmap *pix = new QPixmap(this->GetTileAt(i)->GetTileTexture());
        terrain.push_back(pix);
    }
}

Tile *Map::GetTileAt(int index)
{
    return this->board.at(index)->tile;
}

int Map::GetBoardSize()
{
    return this->board.size();
}

// boardIndex = the index value where the tile is stored in board
// ptIndex = a value 0-6 for the desired point on the hexagon
QPoint Map::GetHexTilePoint(int boardIndex, int ptIndex)
{
    return this->GetTileAt(boardIndex)->GetHexPoint(ptIndex);
}

TileType Map::GetTileTypeAt(int index)
{
    return this->GetTileAt(index)->GetTileType();
}

QPixmap *Map::GetTilePixmap(int index)
{
    return this->terrain.at(index);
}

int Map::GetMapSizeX()
{
    return this->mapSizeX;
}

/*
 * GetNeighbors() will return a list of the 6 tiles
 * adjacent to the passed tile.
 */
QList<Tile *> Map::GetNeighbors(Tile *node)
{
    QList<Tile*> neighbors;

    for(int x = -2; x <= 2; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            if(((x == -1 || x == 1) && y == 0) || (x == -2 && y != 0) || (x == 2 && y != 0) || (x == 0))
            {
                continue;
            }

            int checkX = node->GetTileID().column + x;
            int checkY = node->GetTileID().row + y;

            if(checkX >= 0 && checkX < (mapSizeX * 2) && checkY >= 0 && checkY < mapSizeY)
            {
                neighbors.push_back(this->GetTileFromCoord(checkX, checkY));
//                neighbors.push_back(mapSizeboard.at((checkX / 2) + (X * checkY)));
            }
        }
    }

    return neighbors;
}

/*
 * GetNeighborsRange() is a modified version of GetNeighbors
 * that returns a list of all tiles within a specified range
 * of the passed tile. This function operates the same as
 * GetNeighbors() when range = 1;
 */
QList<Tile *> Map::GetNeighborsRange(Tile *node, int range)
{
    QList<Tile*> neighbors;

    int maxTiles = 1;
    for(int i=1;i<(range+1);i++){
        maxTiles+=(6*i);
    }//Calculates how many tiles need returned

    int xMin = (-2) * range;
    int xMax = 2 * range;
    int yMin = (-1) * range;
    int yMax = 1 * range;
    Tile* boardTile;

    for(; xMin <= xMax; xMin++)
    {
        for(; yMin <= yMax; yMin++)
        {
            if((((xMin % 2 == -1) || (xMin % 2 == 1)) && (abs(yMin) != yMax)) || ((xMin % 2 == 0) && (abs(yMin) > yMax)))
            {
                continue;
            }

            int checkX = node->GetTileID().column + xMin;
            int checkY = node->GetTileID().row + yMin;

            if(checkX % 2 == 1 && checkY % 2 == 0)
                checkX--;

            if(checkX >= 0 && checkX < (mapSizeX * 2) && checkY >= 0 && checkY < mapSizeY)
            {
                int boardIndex = (checkX / 2) + (mapSizeX * checkY);

                if(checkX < node->GetTileID().column)
                {
                    if(abs(yMin) != yMax)
                    {
                        if(node->GetTileID().column- (range + (range - (abs(yMin) % range))) > (checkX + 1))
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(node->GetTileID().column - (range + (abs(yMin) % range)) > checkX)
                        {
                            continue;
                        }
                    }
                }
                else if(checkX > node->GetTileID().column)
                {
                    if(abs(yMin) != yMax)
                    {
                        if(node->GetTileID().column + (range + (range - (abs(yMin) % range))) < checkX)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(node->GetTileID().column + (range + (abs(yMin) % range)) < checkX)
                        {
                            continue;
                        }
                    }
                }

                bool addtile=true;
                for(int i=0;i<neighbors.length();i++){
                    if(neighbors.at(i)->GetTileIndex()==boardIndex){
                        addtile=false;
                    }
                }
                if(addtile&&(neighbors.length()<maxTiles)){
                    boardTile = this->GetTileAt(boardIndex); //board.at(boardIndex);
                    neighbors.push_back(boardTile);
                }
            }
        }

        yMin = (-1) * range;
    }

    return neighbors;
}


/*
 * GetMaximumExpansionTiles() returns a list of all tiles that
 * a city can expand to. This function is run only when a city
 * is founded. It is filtered through every turn after a city
 * has been for any tile that has been claimed by other cities.
 */
QList<Tile *> Map::GetMaximumExpasionTiles(Tile *cityCenter)
{
    //MEB radius: 5 tiles from node
    QList<Tile*> neighbors;

    int range = 5;
    int xMin = (-2) * range;
    int xMax = 2 * range;
    int yMin = (-1) * range;
    int yMax = 1 * range;
    Tile* boardTile;

    for(; xMin <= xMax; xMin++)
    {
        for(; yMin <= yMax; yMin++)
        {
            // skip iteration if:
            //  abs(yMin) != yMax
            //      --<<AND>>--
            //  checkX < nodeX + range adjustment
            //  || checkX > nodeX + range adjustment
            if((((xMin % 2 == -1) || (xMin % 2 == 1)) && (abs(yMin) != yMax)) || ((xMin % 2 == 0) && (abs(yMin) == yMax)) || ((xMin == 0) && (abs(yMin) != yMax)))
            {
                continue;
            }

            int checkX = cityCenter->GetTileID().column + xMin;
            int checkY = cityCenter->GetTileID().row + yMin;

            if(checkX % 2 == 1 && checkY % 2 == 0)
                checkX--;

            if(checkX >= 0 && checkX < (mapSizeX * 2) && checkY >= 0 && checkY < mapSizeX)
            {
                int boardIndex = (checkX / 2) + (mapSizeX * checkY);

                if(checkX < cityCenter->GetTileID().column)
                {
                    if(abs(yMin) != yMax)
                    {
                        if(cityCenter->GetTileID().column - (range + (range - (abs(yMin) % range))) > (checkX + 1))
                        {
                            continue;
                        }
                        else if(cityCenter->GetTileID().column -(range + (range - (abs(yMin) % range))) < (checkX))
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(cityCenter->GetTileID().column - (range + (abs(yMin) % range)) > checkX)
                        {
                            continue;
                        }
                    }
                }
                else if(checkX > cityCenter->GetTileID().column)
                {
                    if(abs(yMin) != yMax)
                    {
                        if(cityCenter->GetTileID().column + (range + (range - (abs(yMin) % range))) < checkX)
                        {
                            continue;
                        }
                        else if(cityCenter->GetTileID().column + (range + (range - (abs(yMin) % range))) > checkX + 1)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(cityCenter->GetTileID().column + (range + (abs(yMin) % range)) < checkX)
                        {
                            continue;
                        }
                    }
                }

                boardTile = this->GetTileAt(boardIndex); //board.at(boardIndex);
                neighbors.push_back(boardTile);
            }

        }

        yMin = (-1) * range;
    }

    return neighbors;
}

/*
 * listContains() and setContains() perform the same operation
 * the only difference being the passed parameters. These functions
 * are only used by the pathfinder for determining if a tile has
 * already been searched.
 */
bool Map::listContains(QList<Tile *> list, Tile *tile)
{
    foreach(Tile* h, list)
    {
        if((h->GetTileID().column == tile->GetTileID().column) && (h->GetTileID().row == tile->GetTileID().row))
            return true;
    }

    return false;
}

bool Map::setContains(QSet<Tile *> set, Tile *tile)
{
    foreach(Tile* h, set)
    {
        if((h->GetTileID().column == tile->GetTileID().column) && (h->GetTileID().row == tile->GetTileID().row))
            return true;
    }

    return false;
}

Tile* Map::GetTileFromCoord(int column, int row)
{
    return this->board.at((column / 2) + (mapSizeX * row))->tile;
}

Tile *Map::GetTileFromCoord(TileID id)
{
    return this->board.at((id.column / 2) + (mapSizeX * id.row))->tile;
}

OccupantData Map::GetODFromTileAt(int index)
{
    return board.at(index)->od;
}

OccupantData Map::GetODFromCoord(int column, int row)
{
    return this->board.at((column/2) + (mapSizeX * row))->od;
}

MapData *Map::GetDataAt(int index)
{
    return this->board.at(index);
}

MapData* Map::GetDataFromCoord(int column, int row)
{
    return this->board.at((column / 2) + (mapSizeX * row));
}

void Map::GenerateMap()
{
    int val;

    //These weights are used to tell the Mersenne Twister
    // how often to generate a particular number.
    // These numbers can be greater than one.
    double weights[] =
    {
        0.05,    //Water = 0
        0.5,     //Grass = 1
        0.15,    //Desert = 2
        0.05,    //Mountain = 3
        0.15,    //Hill = 4
        0.1,     //Forest = 5
        0.1      //Plains = 6
    };
    // Seed the RNG
    qsrand(QTime::currentTime().msec());
    // Create a Mersenne Twister using the random_device
    std::mt19937 gen(qrand());
    // Add the probability weights to the Mersenne Twister
    std::discrete_distribution<> d(std::begin(weights), std::end(weights));

    for(int i = 0; i < board.size(); i++)
    {
        val = d(gen);
        Tile* t = this->GetTileAt(i);

        if(val == 0)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() == 0))
            {
                t->SetTileType(WATER);
                t->SetTileTexture(WATER);
                t->GetYield()->ChangeYield(1, 0, 0, 1, 0);
               t->Walkable = true;
            }
        }
        else if (val == 1)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(GRASS);
                t->SetTileTexture(GRASS);
                t->GetYield()->ChangeYield(0, 0, 0, 2, 0);
                t->SetMoveCost(1);
            }
        }
        else if (val == 2)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(DESERT);
                t->SetTileTexture(DESERT);
                t->SetMoveCost(1);
            }
        }
        else if(val == 3)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(MOUNTAIN);
                t->SetTileTexture(MOUNTAIN);
                t->Walkable = false;
            }
        }
        else if(val == 4)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(HILL);
                t->SetTileTexture(HILL);
                t->SetMoveCost(2);
            }
        }
        else if(val == 5)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(FOREST);
                t->SetTileTexture(FOREST);
                t->GetYield()->ChangeYield(0, 1, 0, 1, 0);
                t->SetMoveCost(2);
            }
        }
        else if(val == 6)
        {
            if((t->GetTileBiome() != POLE) && (t->GetTileBiome() != OCEAN) && (t->GetContinent() != 0))
            {
                t->SetTileType(PLAINS_TILE);
                t->SetTileTexture(PLAINS_TILE);
                t->GetYield()->ChangeYield(0, 1, 0, 1, 0);
                t->SetMoveCost(1);
            }
        }

        t->SetControllingCivListIndex(-1);
    }
}

void Map::GenerateMapEdge()
{
    for(int i = 0; i < board.size(); i++)
    {
        Tile* t = this->GetTileAt(i);
        if(t->GetTileBiome() == POLE)
        {
            t->SetTileTexture(ICE);
            t->SetTileType(ICE);
            t->Walkable = false;
        }

        if(t->GetTileBiome() == OCEAN)
        {
            t->SetTileTexture(WATER);
            t->SetTileType(WATER);
            t->Walkable = true;
        }
    }
}

/*
 * The CleanMap function is used to generate a few small islands
 * around the map to give the map a more 'random' feel instead of
 * two landmasses generated within well defined bounds.
 */
void Map::CleanMap()
{
    srand(time(0));
    int index ;
    for(int i  = 0; i < this->oceanScaleFactor; i++)
    {
newloc:
        index = rand() % board.size();

        if(board.at(index)->tile->GetTileType() == WATER)
        {
            foreach(Tile* t, GetNeighborsRange(board.at(index)->tile, this->oceanScaleFactor / 2))
            {
                if((t->GetTileType() == WATER))
                {
                    t->SetContinent(3);
                }
            }
        }
        else
            goto newloc;
    }
}

/*
 * GetTileQueue() uses the passed city's list of controlled
 * tiles to search for the tiles adjacent to the city's borders
 * that are not controlled by another city. This queue is used
 * to keep track of what tile a city will expand to next.
 */
void Map::GetTileQueue(City *city)
{
    QList<Tile*> surroundingTiles;

    city->borderQueue.clear();

    foreach(Tile* tile, city->GetControlledTiles())
    {
        surroundingTiles = GetNeighbors(tile);

        int surroundCount = 0;

        foreach(Tile* sTile, surroundingTiles)
        {
            if(sTile->GetGoverningCityID() != city->GetCityID())
            {
                if(city->borderQueue.size() > 1)
                {
                    if(!city->borderQueue.contains(sTile))
                    {
                        city->borderQueue.push_back(sTile);
                    }
                }
                else
                {
                    city->borderQueue.push_back(sTile);
                }
            }

            if((sTile->GetControllingCiv() != NO_NATION) ||
                    ((sTile->GetTileID().column == city->GetCityTile()->GetTileID().column) &&
                     (sTile->GetTileID().row == city->GetCityTile()->GetTileID().row)) ||
                    (sTile->GetTileType() == ICE))
            {
                surroundingTiles.removeAt(surroundCount);
            }
            else
            {                
                if(city->tileQueue.size() > 1)
                {
                    if(!city->tileQueue.contains(sTile))
                    {
                        city->tileQueue.push_back(sTile);
                    }
                }
                else
                {
                    city->tileQueue.push_back(sTile);
                }
            }

            surroundCount++;
        }
    }
}

void Map::SetOccupantDataAt(int index, OccupantData newOd)
{
    this->board.at(index)->od = newOd;
}

void Map::DefineCityBordersNew(City *city)
{
    QVector<QLine*> lines;

    //Search run GetNeighbors on the tileQueue to find where the city's border tiles are
    foreach(Tile* tile, city->borderQueue)
    {
        foreach(Tile* neighbor, GetNeighbors(tile))
        {
            //A city will never be on the edge of its borders.
            if(neighbor->HasCity)
            {
                continue;
            }

            if(neighbor->GetGoverningCityID() == city->GetCityID())
            {
                //Generate a new line that is the bordering edge of the tileQueue and controlled tile
                QLine* l = new QLine();
                bool first = true, done = false;
                foreach(QPoint pt, tile->GetTilePolygon())
                {
                    foreach(QPoint nPt, neighbor->GetTilePolygon())
                    {
                        if((pt == nPt) && first)
                        {
                            l->setP1(nPt);
                            first = false;
                        }
                        else if((pt == nPt) && !first && (nPt != l->p1()))
                        {
                            l->setP2(nPt);
                            done = true;
                            break;
                        }
                    }

                    if(done)
                    {
                        first = true;
                        break;
                    }
                }
                lines.push_back(l);
            }
        }
    }

    QLine* f = lines.first();

    //Look for the left most point
    foreach(QLine* line, lines)
    {
        if(line->p1().x() < f->p1().x())
        {
            f = line;
        }
        else if(line->p2().x() < f->p1().x())
        {
            f = line;
        }
        else if(line->p1().x() < f->p2().x())
        {
            f = line;
        }
        else if(line->p2().x() < f->p2().x())
        {
            f = line;
        }
    }

    QPolygon* newBorders = new QPolygon();
    QPoint lPt = f->p2();
    QLine l = *(lines.first());

    bool done = false, found = false;
    do
    {
        for(int i = 0; i < lines.size(); i++)
        {
            if(lines.at(i)->p1() == lPt)
            {
                newBorders->push_back(lines.at(i)->p1());
                lines.at(i)->setP1(QPoint(0,0));
                l = *(lines.at(i));
                lPt = lines.at(i)->p2();
                found = true;
            }
            else if(lines.at(i)->p2() == lPt)
            {
                newBorders->push_back(lines.at(i)->p2());
                lines.at(i)->setP2(QPoint(0,0));
                l = *(lines.at(i));
                lPt = lines.at(i)->p1();
                found = true;
            }

            if(found)
            {
                found = false;
                if((l.p1() == QPoint(0,0)) && (l.p2() == QPoint(0,0)))
                {
                    done = true;
                    break;
                }
            }
        }
    }
    while(!done);

    city->SetCityBorders(*newBorders);
}

/*
 * CreateCity() is the function that actually generates the city object that
 * is added to each civ upon its initial spawn and every city founding thereafter
 */
City* Map::CreateCity(int cityTileIndex, Civilization *founder, bool isCapital)
{
    City* city = new City(founder->getCivIndex());
    QList<Tile*> initialTiles = GetNeighbors(this->GetTileAt(cityTileIndex));

    foreach(Tile* tile, initialTiles)
    {
        if(tile->GetControllingCiv() != NO_NATION)
        {
            return city;
        }
    }

    //We are assuming here that CreateCity is only called
    // When a city is founded not when ownership is changed.
    if(isCapital)
        city->SetCityAsCapital(true, true);
    else
        city->SetCityAsCapital(false, false);

    city->SetCityTile(this->GetTileAt(cityTileIndex));
    city->SetControllingCiv(founder->getCiv());
    city->GetCityTile()->SetYield(5,5,5,5,5);

    if(founder->isCivAI())
        city->GetCityTile()->CanAlwaysBeSeen = false;

    city->SetCityIndex(founder->GetCityList().size());
    city->SetName(founder->GetNextCityName());
    city->SetCitizenCount(1);
    city->SetCityID(100*founder->getCivIndex() + founder->GetCityList().size());

    foreach(Tile* tile, initialTiles)
    {
        city->AddControlledTile(tile);
    }

    city->UpdateCityYield();

    city->InitializeCity();

    return city;
}

void Map::WriteMapSaveData(QJsonObject &obj) const
{
    obj["mapsizex"] = mapSizeX;
    obj["mapsizey"] = mapSizeY;
    obj["oceanscalefactor"] = oceanScaleFactor;

    QJsonArray tileArray;
    foreach(MapData* md, board)
    {
        Tile* t = md->tile;
        QJsonObject tileObj;
        t->WriteTileSaveData(tileObj);
        tileArray.append(tileObj);
    }

    obj["tiledata"] = tileArray;
}

void Map::ReadMapSaveData(QJsonObject &obj)
{
    mapSizeX = obj["mapsizex"].toInt();
    mapSizeY = obj["mapsizey"].toInt();
    oceanScaleFactor = obj["oceanscalefactor"].toInt();

    GenerateTiles();

    QJsonArray tArray = obj["tiledata"].toArray();
    for(int i = 0; i < board.size(); i++)
    {
        board.at(i)->tile->ReadTileSaveData(tArray.at(i).toObject());
    }

    InitTerrain();
}

void Map::GenerateTiles()
{
    Tile *tile;
    //Flattop: x = 12, y =0;
    //Pointtop: x = 0, y = 12;
    int posX = 0;
    int posY = 12;
//    float rowOffset = 37;
    bool odd = false;
    int column = 0, row = 0;


    for(int i = 0; i < (mapSizeY); i++)
    {
        if((i % 2) != 0)
        {
            odd = true;
            column = 1;
        }
        else
        {
            odd = false;
            column = 0;
        }


        for(int j = 0; j < (mapSizeX); j++)
        {
            tile = new Tile(posX, posY);

            tile->SetTileID(row, column, tile);
            tile->SetTileIndex(board.size());

            board.push_back(new MapData{tile,  DEFAULT_OCCUPANT});

            //flat:
//            posX += 74;
            //point:
            posX += 44;

            column += 2;
        }

        if(!odd)
        {
            //Flat: posX = 12 + offset
            //Point: posX = 22;
            posX = 22; // + rowOffset;
        }
        else
        {
            //Flat: posX = 12;
            //point: posX = 0;
            posX = 0;
        }
        row++;
        //Flat: posY += 22;
        //Point: posY += 37;
        posY += 37;
    }
}

/*
 * SpawnCivs() is the main function that runs during initalization.
 * this function generates the starting capital for each civ as well
 * as the starting Worked and Warrior units.
 */
void Map::SpawnCivs(QVector<Civilization*> civs)
{
    City *city;
    Unit *unit;
    srand(time(0));
    int index, lastIndex = 0;

    for(int i = 0; i < civs.size(); i++)
    {
newrand:
        index = (static_cast<double>(rand()) / RAND_MAX) * (board.size());
        Tile* t = this->GetTileAt(index);
        OccupantData tod = this->GetODFromTileAt(index);
        if((t->GetTileType() == ICE) || (t->GetTileType() == WATER) || (t->GetTileType() == MOUNTAIN) || (tod.OccupantNation != NO_NATION))
        {
            goto newrand;
        }

        if((tod.OccupantNation == NO_NATION) && !t->HasCity)
        {
            if(lastIndex == index)
            {
                goto newrand;
            }

            lastIndex = index;

            t->SetControllingCivListIndex(i);

            city = this->CreateCity(index, civs.at(i), true);

            if(!city->IsInitialized())
            {
                if(t->GetControllingCivListIndex() == -1)
                    t->SetControllingCivListIndex(-1);

                goto newrand;
            }

            for(int j = 0; j < i; j++)
            {
                //since this function only runs when spawning civs for the first time,
                // we can get away with only checking the city at index 0.
                //  This may change when Save/Load is added
                if(city->GetMaximumExpansionBorder().boundingRect().intersects(civs.at(j)->GetCityAt(0)->GetMaximumExpansionBorder().boundingRect()))
                {
                    civs.at(i)->SetCityIndex(0);
                    foreach(Tile* tile, city->GetControlledTiles())
                    {
                        tile->SetControllingCivListIndex(-1);
                        if(tile->IsWorked)
                        {
                            tile->IsWorked = false;
                        }
                    }

                    delete city;
                    t->SetControllingCivListIndex(-1);
                    goto newrand;
                }
            }

            for(int k = 0; k < board.size(); k++)
            {
                if(board.at(k)->tile->GetTileType() == ICE || board.at(k)->tile->GetTileBiome() == OCEAN)
                {
                    if(city->GetMinimumSettleDistance().boundingRect().intersects(board.at(k)->tile->GetTilePolygon().boundingRect()))
                    {
                        civs.at(i)->SetCityIndex(0);
                        foreach(Tile* tile, city->GetControlledTiles())
                        {
                            tile->SetControllingCivListIndex(-1);
                            if(tile->IsWorked)
                            {
                                tile->IsWorked = false;
                            }
                        }

                        delete city;
                        t->SetControllingCivListIndex(-1);
                        goto newrand;
                    }
                }
            }

            t->HasCity = true;
            t->SetGoverningCity(city, i);

            QList<Tile*> cityMEB = this->GetNeighborsRange(t, 4);
            city->SetMaximumExpansionBorderTiles(cityMEB);
            city->FilterMEBList();

            this->GetTileQueue(city);
            this->DefineCityBordersNew(city);
            city->SortTileQueue();

            city->SortControlledTiles();
            city->GetControlledTiles().first()->IsWorked = true;

            civs.at(i)->AddCity(city);

            unit = new Unit(civs.at(i)->getCiv(), WORKER, i);
            unit->RequiresOrders = true;

            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if((this->GetDataAt(tile->GetTileIndex())->od.OccupantNation == NO_NATION) && !tile->HasCity)
                    {
                        unit->SetPosition(tile);
                        break;
                    }
                }
            }

            unit->SetUnitListIndex(0);
            civs.at(i)->AddUnit(unit);

            unit = new Unit(civs.at(i)->getCiv(), WARRIOR, i);
            unit->RequiresOrders = true;
            unit->SetStrength(6);
            unit->SetMovementPoints(2);

            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if((this->GetDataAt(tile->GetTileIndex())->od.OccupantNation == NO_NATION) && !tile->HasCity)
                    {
                        unit->SetPosition(tile);
                        break;
                    }
                }
            }

            unit->SetUnitListIndex(1);
            civs.at(i)->AddUnit(unit);
            civs.at(i)->UpdateCivYield();
        }
    }
#ifdef DEBUG
    qDebug() << "SpawnCivs complete";
#endif
}

void Map::GenerateBiomes()
{
    //Generate Ice Caps
    for(int i = 0; i < board.size(); i++)
    {
        Tile* t = this->GetTileAt(i);
        if(t->GetTileID().row <= 1)
        {
            t->SetTileBiome(Biome::POLE);
            t->SetContinent(0);
        }
        else if(t->GetTileID().row >= mapSizeY - 2)
        {
            t->SetTileBiome(Biome::POLE);
            t->SetContinent(0);
        }
    }

    //Generate Ocean Borders
    for(int i = 0; i < board.size(); i++)
    {
        Tile* t = this->GetTileAt(i);
        if(t->GetTileID().column <= (this->oceanScaleFactor - 1))
        {
            if(t->GetTileBiome() != POLE)
            {
                t->SetTileBiome(OCEAN);
                t->SetContinent(0);
            }
        }
        else if((t->GetTileID().column > (mapSizeX) - this->oceanScaleFactor) && (t->GetTileID().column < (mapSizeX) + this->oceanScaleFactor))
        {
            if(t->GetTileBiome() != POLE)
            {
                t->SetTileBiome(OCEAN);
                t->SetContinent(0);
            }
        }
        else if(t->GetTileID().column >= (mapSizeX * 2) - this->oceanScaleFactor)
        {
            if(t->GetTileBiome() != POLE)
            {
                t->SetTileBiome(OCEAN);
                t->SetContinent(0);
            }
        }

        if((t->GetTileID().column > this->oceanScaleFactor) && (t->GetTileID().column < (mapSizeX - this->oceanScaleFactor)))
        {
            if(t->GetTileBiome() != POLE && t->GetTileBiome() != OCEAN)
                t->SetContainer(1);
        }
        else if((t->GetTileID().column > (mapSizeX + this->oceanScaleFactor)) && (t->GetTileID().column < (mapSizeX * 2) - this->oceanScaleFactor))
        {
            if(t->GetTileBiome() != POLE && t->GetTileBiome() != OCEAN)
                t->SetContainer(2);
        }
        else
            t->SetContainer(0);
    }

    //Set up continents
    srand(time(0));
    int index, range;

    for(int j = 1; j < 3; j++)
    {
        for(int i = 0; i < oceanScaleFactor; i++)
        {
newindex:
            index = rand() % board.size();
            Tile* t = this->GetTileAt(index);
            if(t->GetContainer() == j)
            {
                range = ((rand() % oceanScaleFactor) + 1) * 2;
                foreach(Tile* ti, GetNeighborsRange(board.at(index)->tile, range))
                {
                    ti->SetContinent(j);
                }
            }
            else
            {
                goto newindex;
            }
        }
    }
}

void Map::GenerateResources()
{
    int resource;

    //These weights are used to tell the Mersenne Twister
    // how often to generate a particular number.
    // These numbers can be greater than one.
    double weights[] =
    {
        1.0, // No Strategic
        0.1, // Iron
        0.1, // Horses
        0.1, // Uranium
        0.1, // Aluminum
        0.1, // Coal
        0.1, // Oil
  /* ------------------------*/
        1.0, // No Luxury
        0.1, // Wheat
        0.1, // Cattle
        0.1, // Deer
        0.1, // Fish
        0.1, // Whales
        0.1, // Bananas
        0.1, // Gold
        0.1, // Gems
        0.1, // Marble
        0.1, // Ivory
        0.1, // Dyes
        0.1, // Spices
        0.1, // Silk
        0.1, // Sugar
        0.1, // Cotton
        0.1, // Pearls
        0.1, // Incense
        0.1, // Wine
        0.1, // Silver
        0.1, // Furs
        0.1  // Sheep
    };
    // Seed the RNG
    qsrand(QTime::currentTime().msec());
    // Create a Mersenne Twister using the random_device
    std::mt19937 gen(qrand());
    // Add the probability weigths to the Mersenne Twister
    std::discrete_distribution<> d(std::begin(weights), std::end(weights));

    for(int i = 0; i < board.size(); i++)
    {
        Tile *t = this->GetTileAt(i);
        if((t->GetTileType() == GRASS) || (t->GetTileType() == HILL) || (t->GetTileType() == FOREST) || (t->GetTileType() == PLAINS_TILE) && (t->GetContinent() != 0))
        {
            resource = d(gen);
            switch(resource)
            {
            case NO_STRATEGIC:
                t->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            case IRON:
                t->SetResource(IRON, NO_LUXURY);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case HORSES:
                t->SetResource(HORSES, NO_LUXURY);
                t->CanHavePasture = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case URANIUM:
                t->SetResource(URANIUM, NO_LUXURY);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case ALUMINUM:
                t->SetResource(ALUMINUM, NO_LUXURY);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case COAL:
                t->SetResource(COAL, NO_LUXURY);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case OIL:
                t->SetResource(OIL, NO_LUXURY);
                t->CanHaveOilWell = true;
                t->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case NO_LUXURY:
                t->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            case WHEAT:
                t->SetResource(NO_STRATEGIC, WHEAT);
                t->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case CATTLE:
                t->SetResource(NO_STRATEGIC, CATTLE);
                t->CanHavePasture = true;
                t->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case DEER:
                t->SetResource(NO_STRATEGIC, DEER);
                t->CanHaveCamp = true;
                t->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case FISH:
                if(t->GetTileType() == WATER)
                {
                    t->SetResource(NO_STRATEGIC, FISH);
                    t->CanHaveFishBoat = true;
                    t->GetYield()->ChangeYield(0, 0, 0, 2, 0);
                }
                break;
            case WHALES:
                if(t->GetTileType() == WATER)
                {
                    t->SetResource(NO_STRATEGIC, WHALES);
                    t->CanHaveFishBoat = true;
                    t->GetYield()->ChangeYield(1, 0, 0, 1, 0);
                }
                break;
            case BANANAS:
                t->SetResource(NO_STRATEGIC, BANANAS);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case GOLD_RESOURCE:
                t->SetResource(NO_STRATEGIC, GOLD_RESOURCE);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case GEMS:
                t->SetResource(NO_STRATEGIC, GEMS);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(3, 0, 0, 0, 0);
                break;
            case MARBLE:
                t->SetResource(NO_STRATEGIC, MARBLE);
                t->CanHaveQuarry = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case IVORY:
                t->SetResource(NO_STRATEGIC, IVORY);
                t->CanHaveCamp = true;
                t->GetYield()->ChangeYield(1, 0, 0, 0, 0);
                break;
            case DYES:
                t->SetResource(NO_STRATEGIC, DYES);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SPICES:
                t->SetResource(NO_STRATEGIC, SPICES);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SILK:
                t->SetResource(NO_STRATEGIC, SILK);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SUGAR:
                t->SetResource(NO_STRATEGIC, SUGAR);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case COTTON:
                t->SetResource(NO_STRATEGIC, COTTON);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case PEARLS:
                if(t->GetTileType() == WATER)
                {
                    t->SetResource(NO_STRATEGIC, PEARLS);
                    t->CanHaveFishBoat = true;
                    t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                }
                break;
            case INCENSE:
                t->SetResource(NO_STRATEGIC, INCENSE);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case WINE:
                t->SetResource(NO_STRATEGIC, WINE);
                t->CanHavePlantation = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SILVER:
                t->SetResource(NO_STRATEGIC, SILVER);
                t->CanHaveMine = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case FURS:
                t->SetResource(NO_STRATEGIC, FURS);
                t->CanHaveCamp = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SHEEP:
                t->SetResource(NO_STRATEGIC, SHEEP);
                t->CanHavePasture = true;
                t->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            default:
                t->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            }
        }
        else
        {
            t->SetResource(NO_STRATEGIC, NO_LUXURY);
        }

        t->SetWorkerButtons();
    }
}

