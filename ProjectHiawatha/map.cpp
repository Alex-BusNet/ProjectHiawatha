#include "map.h"
#include <QDebug>
#include <map>
#include <random>
#include <QTime>
#include <ctime>
#include "biome.h"
#include "unittype.h"
#include <queue>

Map::Map()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 64; //mapSizeX is doubled for map creation. i.e. a map size of 40 tiles will yield 80 columns.
    mapSizeY = 80;

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
    this->mapSizeX = mapSizeX;
    this->mapSizeY = mapSizeY;

    switch(this->mapSizeX)
    {
    case 20:
        this->oceanScaleFactor = 2;
        break;
    case 28:
        this->oceanScaleFactor = 3;
        break;
    case 33:
        this->oceanScaleFactor = 4;
        break;
    case 40:
        this->oceanScaleFactor = 5;
        break;
    case 52:
        this->oceanScaleFactor = 6;
        break;
    case 64:
        this->oceanScaleFactor = 7;
        break;
    default:
        this->oceanScaleFactor = 3;
        break;
    }

    qDebug() << "Ocean scale factor:" << this->oceanScaleFactor;
}

void Map::InitHexMap()
{
    qDebug() << "InitHexMap() called";

    Tile *tile;
    //Flattop: x = 12, y =0;
    //Pointtop: x = 0, y = 12;
    int posX = 0;
    int posY = 12;
//    float rowOffset = 37;
    bool odd = false;
    int column = 0, row = 0;

    qDebug() << "MapSizeX: " << mapSizeX << " MapSizeY: " << mapSizeY;

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

            board.push_back(tile);

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
        QPixmap *pix = new QPixmap(board.at(i)->GetTileTexture());
        terrain.push_back(pix);
    }
}

Tile *Map::GetTileAt(int index)
{
    return this->board.at(index);
}

int Map::GetBoardSize()
{
    return this->board.size();
}

// boardIndex = the index value where the tile is stored in board
// ptIndex = a value 0-6 for the desired point on the hexagon
QPoint Map::GetHexTilePoint(int boardIndex, int ptIndex)
{
    return this->board.at(boardIndex)->GetHexPoint(ptIndex);
}

TileType Map::GetTileTypeAt(int index)
{
    return this->board.at(index)->GetTileType();
}

QPixmap *Map::GetTilePixmap(int index)
{
    return this->terrain.at(index);
}

int Map::GetMapSizeX()
{
    return this->mapSizeX;
}

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
                neighbors.push_back(board.at((checkX / 2) + (mapSizeX * checkY)));
            }
        }
    }

    return neighbors;
}

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
    return this->board.at((column / 2) + (mapSizeX * row));
}

Tile *Map::GetTileFromCoord(TileID id)
{
    return this->board.at((id.column / 2) + (mapSizeX * id.row));
}

void Map::GenerateMap()
{
    int dbl;

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
    // Add the probability weigths to the Mersenne Twister
    std::discrete_distribution<> d(std::begin(weights), std::end(weights));

    for(int i = 0; i < board.size(); i++)
    {
        dbl = d(gen);

        if(dbl == 0)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(WATER);
                board.at(i)->SetTileTexture(WATER);
                board.at(i)->GetYield()->ChangeYield(1, 0, 0, 1, 0);
                board.at(i)->Walkable = false;
            }
        }
        else if (dbl == 1)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(GRASS);
                board.at(i)->SetTileTexture(GRASS);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 2, 0);
                board.at(i)->SetMoveCost(1);
            }
        }
        else if (dbl == 2)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(DESERT);
                board.at(i)->SetTileTexture(DESERT);
                board.at(i)->SetMoveCost(1);
            }
        }
        else if(dbl == 3)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(MOUNTAIN);
                board.at(i)->SetTileTexture(MOUNTAIN);
                board.at(i)->Walkable = false;
            }
        }
        else if(dbl == 4)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(HILL);
                board.at(i)->SetTileTexture(HILL);
                board.at(i)->SetMoveCost(2);
            }
        }
        else if(dbl == 5)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(FOREST);
                board.at(i)->SetTileTexture(FOREST);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 1, 0);
                board.at(i)->SetMoveCost(2);
            }
        }
        else if(dbl == 6)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(PLAINS_TILE);
                board.at(i)->SetTileTexture(PLAINS_TILE);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 1, 0);
                board.at(i)->SetMoveCost(1);
            }
        }

        board.at(i)->SetCivListIndex(-1);
    }
}

void Map::GenerateMapEdge()
{
    for(int i = 0; i < board.size(); i++)
    {
        if(board.at(i)->GetTileBiome() == Biome::POLE)
        {
            board.at(i)->SetTileTexture(ICE);
            board.at(i)->SetTileType(ICE);
            board.at(i)->Walkable = false;
        }
        if(board.at(i)->GetTileBiome() == OCEAN)
        {
            board.at(i)->SetTileTexture(WATER);
            board.at(i)->SetTileType(WATER);
            board.at(i)->Walkable = true;
        }
    }
}

void Map::CleanMap()
{
    //=====================
    // Step 1) Search map for isolated desert tiles and change to grass tiles
    // Step 2) Search map for clusters of desert tiles and fill them in to create a desert that is
    //          a minimum of 5 columns by 3 rows in size
    // Step 3) Search map for plains biomes and determine locations to place hills or forests
    // Step 4) Place one hill or forest at indicated location, determine a random direction and place a hill or forest there,
    //          Repeat until max size of hill or forest biome has been reached
    // Step 5) Search map and determine proximity of hills, forests, and deserts to make sure no two biomes
    //          of the same type are too close to each other.
    // Step 6) Change continent edges to water to reduce blocky-ness of continents.
    // Step 7) Populate tiles with proper yields and resources.
    // Step 8) Set Player and AI spawns.
    //=====================

    qDebug() << "Cleaning map";
    Tile* adjacentTiles[3]; // { SW, S, SE }

    for(int i = 0; i < board.size(); i++)
    {
        if(board.at(i)->GetTileType() == DESERT && !(board.at(i)->Checked))
        {
            //Get the tiles surrounding the selected tile
            TileID selectedID = board.at(i)->GetTileID();

            // We only need to check the tiles to the SW, S, and SE of the selected tile
            // because the rows above it have already been checked. Also need to use to Checked flag
            // so we don't turn the whole map into a desolate land.
            adjacentTiles[0] = GetTileFromCoord(selectedID.column - 1, selectedID.row + 1); //SW
            adjacentTiles[1] = GetTileFromCoord(selectedID.column, selectedID.row + 2);     //S
            adjacentTiles[2] = GetTileFromCoord(selectedID.column + 1, selectedID.row + 1); //SE

            for(int j = 0; j < 3; j++)
            {
                if(adjacentTiles[j]->GetTileType() == DESERT)
                {
                    if(j == 0)
                    {
                        //build down and left
                        break;

                    }
                    else if(j == 1)
                    {
                        //build down, centered on selected
                        break;
                    }
                    else if(j == 2)
                    {
                        //build down and right
                        break;
                    }
                }

                adjacentTiles[j]->Checked = true;
            }
        }
    }

}

void Map::GetTileQueue(City *city)
{
    qDebug() << "--Getting Tile Queue";
    QList<Tile*> surroundingTiles;

    foreach(Tile* tile, city->GetControlledTiles())
    {
        surroundingTiles = GetNeighbors(tile);

        int surroundCount = 0;

        foreach(Tile* tile, surroundingTiles)
        {
            if((tile->GetControllingCiv() != NO_NATION) ||
                    ((tile->GetTileID().column == city->GetCityTile()->GetTileID().column) &&
                     (tile->GetTileID().row == city->GetCityTile()->GetTileID().row)) || (tile->GetTileType() == ICE))
            {
//                qDebug() << "---------SurroundingTiles at" << tile->GetTileIDString() << "is already owned";
                surroundingTiles.removeAt(surroundCount);
            }
            else
            {
                if(city->tileQueue.size() > 1)
                {
                    if(!city->tileQueue.contains(tile))
                    {
                        city->tileQueue.push_back(tile);
                    }
//                    else
//                    {
//                        qDebug() << "--------------------Tile already added";
//                    }
                }
                else
                {
                    city->tileQueue.push_back(tile);
                }
            }

            surroundCount++;
        }
    }

//    foreach(Tile* tile, city->tileQueue)
//    {
//        qDebug() << "               tileQueue:" << tile->GetTileIDString();
//    }
    qDebug() << "--Done";
}

City* Map::CreateCity(int cityTileIndex, Civilization *founder, bool isCapital)
{
    City* city = new City();
    QList<Tile*> initialTiles = GetNeighbors(board.at(cityTileIndex));

    foreach(Tile* tile, initialTiles)
    {
        if(tile->GetControllingCiv() != NO_NATION)
        {
            return city;
        }
    }

    if(isCapital)
        city->SetCityAsCapital(true);
    else
        city->SetCityAsCapital(false);

    city->SetCityTile(board.at(cityTileIndex));    
    city->SetControllingCiv(founder->getCiv());
    city->GetCityTile()->SetYield(5,5,5,5,5);

    foreach(Tile* tile, initialTiles)
    {
        tile->SetControllingCiv(founder->getCiv());
        city->AddControlledTile(tile);
    }

    city->UpdateCityYield();
    city->SetCityIndex(founder->GetCityList().size());
    city->SetName(founder->GetNextCityName());
    city->SetCitizenCount(1);
    city->DefineCityBorders(false);
    this->GetTileQueue(city);
    city->SortTileQueue();
    city->SortControlledTiles();
    city->GetControlledTiles().first()->IsWorked = true;

    city->InitializeCity();

    return city;
}

void Map::SpawnCivs(QVector<Civilization*> civs)
{
    //=====================
    //
    // Future steps will involve spacing civs farther apart
    // so no two civs are on top of each other.
    //
    // There will also be an initial unit spawning and
    // bigger borders for cities.
    //
    // This may change later to where the player gets a warrior
    // and a settler instead of a city, warrior, and worker to start.
    //
    //=====================

    City *city;
    Unit *unit;
    srand(time(0));
    int index, lastIndex;

    for(int i = 0; i < civs.size(); i++)
    {
newrand:
        index = (static_cast<double>(rand()) / RAND_MAX) * (board.size());

        if(board.at(index)->GetTileType() == ICE || board.at(index)->GetTileType() == WATER || board.at(index)->GetTileType() == MOUNTAIN || board.at(index)->ContainsUnit)
        {
            goto newrand;
        }

        if(!board.at(index)->ContainsUnit && !board.at(index)->HasCity)
        {
            lastIndex = index;

            city = this->CreateCity(index, civs.at(i), true);

            if(!city->IsInitialized()){ goto newrand; }

            for(int j = 0; j < i; j++)
            {
                //since this function only runs when spawning civs for the first time,
                // we can get away with only checking the city at index 0.
                if(city->GetMaximumExpansionBorder().boundingRect().intersects(civs.at(j)->GetCityAt(0)->GetMaximumExpansionBorder().boundingRect()))
                {
                    qDebug() << "--Maximum expansion borders intersected, finding new location to spawn.";
                    delete city;
                    civs.at(i)->SetCityIndex(0);
                    foreach(Tile* tile, city->GetControlledTiles())
                    {
                        tile->SetControllingCiv(NO_NATION);
                        if(tile->IsWorked)
                        {
                            tile->IsWorked = false;
                        }
                    }
                    goto newrand;
                }
            }

            for(int k = 0; k < board.size(); k++)
            {
                if(board.at(k)->GetTileType() == ICE || board.at(k)->GetTileBiome() == OCEAN)
                {
                    if(city->GetMinimumSettleDistance().boundingRect().intersects(board.at(k)->GetTilePolygon().boundingRect()))
                    {
                        qDebug() << "--Minimum settle distance intersects ice; finding new location to spawn.";
                        delete city;
                        civs.at(i)->SetCityIndex(0);
                        foreach(Tile* tile, city->GetControlledTiles())
                        {
                            tile->SetControllingCiv(NO_NATION);
                            if(tile->IsWorked)
                            {
                                tile->IsWorked = false;
                            }
                        }
                        goto newrand;
                    }
                }
            }

            board.at(index)->SetCivListIndex(i);
            board.at(index)->HasCity = true;
            board.at(index)->SetControllingCiv(civs.at(i)->getCiv());
            board.at(index)->SetGoverningCity(city);

            civs.at(i)->AddCity(city);

            unit = new Unit(civs.at(i)->getCiv(), WORKER);
            unit->SetOwner(civs.at(i)->getCiv());
            unit->RequiresOrders = true;
            unit->SetName("Worker");

            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if(!tile->ContainsUnit && !tile->HasCity)
                    {
                        unit->SetPositionIndex((tile->GetTileID().column / 2) + (mapSizeX * tile->GetTileID().row));
                        unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
                        tile->SetCivListIndex(i);
                        tile->ContainsUnit = true;
                        break;
                    }
                }
            }

            unit->SetUnitListIndex(0);
            civs.at(i)->AddUnit(unit);

            unit = new Unit(civs.at(i)->getCiv(), WARRIOR);
            unit->SetOwner(civs.at(i)->getCiv());
            unit->RequiresOrders = true;
            unit->SetStrength(6);
            unit->SetMovementPoints(2);
            unit->SetName("Warrior");

            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if(!tile->ContainsUnit && !tile->HasCity)
                    {
                        unit->SetPositionIndex((tile->GetTileID().column / 2) + (mapSizeX * tile->GetTileID().row));
                        unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
                        tile->SetCivListIndex(i);
                        tile->ContainsUnit = true;
                        break;
                    }
                }
            }

            unit->SetUnitListIndex(1);
            civs.at(i)->AddUnit(unit);
            civs.at(i)->UpdateCivYield();

            qDebug() << "       UnitPos 0:" << board.at(civs.at(i)->GetUnitAt(0)->GetTileIndex())->GetTileIDString();
            qDebug() << "       UnitPos 1:" << board.at(civs.at(i)->GetUnitAt(1)->GetTileIndex())->GetTileIDString();
        }
    }
}

void Map::GenerateBiomes()
{
    //Generate Ice Caps
    for(int i = 0; i < board.size(); i++)
    {
        if(board.at(i)->GetTileID().row <= 1)
        {
            board.at(i)->SetTileBiome(Biome::POLE);
        }
        else if(board.at(i)->GetTileID().row >= mapSizeY - 2)
        {
            board.at(i)->SetTileBiome(Biome::POLE);
        }
    }

    //Generate Ocean Borders
    for(int i = 0; i < board.size(); i++)
    {
        if(board.at(i)->GetTileID().column <= (this->oceanScaleFactor - 1))
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
            }
        }
        else if((board.at(i)->GetTileID().column > (mapSizeX) - this->oceanScaleFactor) && (board.at(i)->GetTileID().column < (mapSizeX) + this->oceanScaleFactor))
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
            }
        }
        else if(board.at(i)->GetTileID().column >= (mapSizeX * 2) - this->oceanScaleFactor)
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
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
        0.0, // Fish
        0.0, // Whales
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
        0.0, // Pearls
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
        resource = d(gen);
//        qDebug() << "--Board Tile Type:" << board.at(i)->GetTileTypeString();
        if((board.at(i)->GetTileType() == GRASS) || (board.at(i)->GetTileType() == HILL) || (board.at(i)->GetTileType() == FOREST))
        {
//            qDebug() << "   Resource:" << resource;
            /// Need to set this so they also update the tile yields.
            switch(resource)
            {
            case NO_STRATEGIC:
                board.at(i)->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            case IRON:
                board.at(i)->SetResource(IRON, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case HORSES:
                board.at(i)->SetResource(HORSES, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case URANIUM:
                board.at(i)->SetResource(URANIUM, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case ALUMINUM:
                board.at(i)->SetResource(ALUMINUM, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case COAL:
                board.at(i)->SetResource(COAL, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case OIL:
                board.at(i)->SetResource(OIL, NO_LUXURY);
                board.at(i)->GetYield()->ChangeYield(0, 1, 0, 0, 0);
                break;
            case NO_LUXURY:
                board.at(i)->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            case WHEAT:
                board.at(i)->SetResource(NO_STRATEGIC, WHEAT);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case CATTLE:
                board.at(i)->SetResource(NO_STRATEGIC, CATTLE);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case DEER:
                board.at(i)->SetResource(NO_STRATEGIC, DEER);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case FISH:
                board.at(i)->SetResource(NO_STRATEGIC, FISH);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 2, 0);
                break;
            case WHALES:
                board.at(i)->SetResource(NO_STRATEGIC, WHALES);
                board.at(i)->GetYield()->ChangeYield(1, 0, 0, 1, 0);
                break;
            case BANANAS:
                board.at(i)->SetResource(NO_STRATEGIC, BANANAS);
                board.at(i)->GetYield()->ChangeYield(0, 0, 0, 1, 0);
                break;
            case GOLD_RESOURCE:
                board.at(i)->SetResource(NO_STRATEGIC, GOLD_RESOURCE);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case GEMS:
                board.at(i)->SetResource(NO_STRATEGIC, GEMS);
                board.at(i)->GetYield()->ChangeYield(3, 0, 0, 0, 0);
                break;
            case MARBLE:
                board.at(i)->SetResource(NO_STRATEGIC, MARBLE);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case IVORY:
                board.at(i)->SetResource(NO_STRATEGIC, IVORY);
                board.at(i)->GetYield()->ChangeYield(1, 0, 0, 0, 0);
                break;
            case DYES:
                board.at(i)->SetResource(NO_STRATEGIC, DYES);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SPICES:
                board.at(i)->SetResource(NO_STRATEGIC, SPICES);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SILK:
                board.at(i)->SetResource(NO_STRATEGIC, SILK);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SUGAR:
                board.at(i)->SetResource(NO_STRATEGIC, SUGAR);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case COTTON:
                board.at(i)->SetResource(NO_STRATEGIC, COTTON);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case PEARLS:
                board.at(i)->SetResource(NO_STRATEGIC, PEARLS);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case INCENSE:
                board.at(i)->SetResource(NO_STRATEGIC, INCENSE);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case WINE:
                board.at(i)->SetResource(NO_STRATEGIC, WINE);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SILVER:
                board.at(i)->SetResource(NO_STRATEGIC, SILVER);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case FURS:
                board.at(i)->SetResource(NO_STRATEGIC, FURS);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            case SHEEP:
                board.at(i)->SetResource(NO_STRATEGIC, SHEEP);
                board.at(i)->GetYield()->ChangeYield(2, 0, 0, 0, 0);
                break;
            default:
                board.at(i)->SetResource(NO_STRATEGIC, NO_LUXURY);
                break;
            }
        }
        else
        {
            board.at(i)->SetResource(NO_STRATEGIC, NO_LUXURY);
        }
    }
}

