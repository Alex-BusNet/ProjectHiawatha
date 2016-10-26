#include "map.h"
#include <QDebug>
#include <map>
#include <random>
#include <QTime>
#include <ctime>
#include "biome.h"
#include "unittype.h"

Map::Map()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 128; //mapSizeX is doubled for map creation. i.e. a map size of 40 tiles will yield 80 columns.
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
        0.1      //Forest = 5
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
                board.at(i)->Walkable = false;
            }
        }
        else if (dbl == 1)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(GRASS);
                board.at(i)->SetTileTexture(GRASS);
                board.at(i)->GetYield()->ChangeYield(1,1,0,1,0);
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
                board.at(i)->GetYield()->ChangeYield(1,1,0,1,0);
                board.at(i)->SetMoveCost(2);
            }
        }
        else if(dbl == 5)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(FOREST);
                board.at(i)->SetTileTexture(FOREST);
                board.at(i)->GetYield()->ChangeYield(1,1,0,1,0);
                board.at(i)->SetMoveCost(2);
            }
        }
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
    // and a settler instead of a city to start.
    //
    //=====================

    City *city;
    Unit *unit;
    srand(time(0));
    int index;

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
//            qDebug() << "   Adding Capital";
            city = new City();
            QList<Tile*> initialTiles = GetNeighbors(board.at(index));

            foreach(Tile* tile, initialTiles)
            {
                if(tile->GetControllingCiv() != NO_NATION)
                {
                    goto newrand;
                }
            }

            city->SetCityAsCaptial();
            city->SetCityTile(board.at(index));
            city->SetControllingCiv(civs.at(i)->getCiv());
            city->GetCityTile()->SetYield(5,5,5,5,5);

            foreach(Tile* tile, initialTiles)
            {
                tile->SetControllingCiv(civs.at(i)->getCiv());
                city->AddControlledTile(tile);
            }
//            city->AddControlledTile(board.at(index - 2));
//            city->AddControlledTile(board.at(index - 3));
//            city->AddControlledTile(board.at(index - 18));
//            city->AddControlledTile(board.at(index - 23));
//            city->AddControlledTile(board.at(index - 42));
//            city->AddControlledTile(board.at(index - 43));
//            city->AddControlledTile(board.at(index + 2));
            city->UpdateCityYield();
            city->SetCityIndex(0);
            civs.at(i)->AddCity(city);
            city->DefineCityBorders();

            board.at(index)->SetYield(5,5,5,5,5);
            board.at(index)->HasCity = true;
            board.at(index)->SetControllingCiv(civs.at(i)->getCiv());

//            qDebug() << "   Adding starting Unit";

            unit = new Unit(civs.at(i)->getCiv(), WORKER);
            unit->SetOwner(civs.at(i)->getCiv());
            unit->RequiresOrders = true;

            foreach(Tile* tile, city->GetControlledTiles())
            {
                qDebug() << "           Tile type:" << tile->GetTileTypeString() << "  Contains Unit:" << tile->ContainsUnit << "  Has City:" << tile->HasCity;
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if(!tile->ContainsUnit && !tile->HasCity)
                    {
                        unit->SetPositionIndex((tile->GetTileID().column / 2) + (mapSizeX * tile->GetTileID().row));
                        unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
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

            foreach(Tile* tile, city->GetControlledTiles())
            {
                qDebug() << "           Tile type:" << tile->GetTileTypeString() << "  Contains Unit:" << tile->ContainsUnit << "  Has City:" << tile->HasCity;
                if(tile->GetTileType() != MOUNTAIN && tile->GetTileType() != WATER && tile->GetTileType() != ICE)
                {
                    if(!tile->ContainsUnit && !tile->HasCity)
                    {
                        unit->SetPositionIndex((tile->GetTileID().column / 2) + (mapSizeX * tile->GetTileID().row));
                        unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
                        tile->ContainsUnit = true;
                        break;
                    }
                }
            }

            unit->SetUnitListIndex(1);
            civs.at(i)->AddUnit(unit);

            qDebug() << "       UnitPos 1:" << board.at(civs.at(i)->GetUnitAt(0)->GetTileIndex())->GetTileIDString();
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
        if(board.at(i)->GetTileID().column <= 1)
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
            }
        }
        else if((board.at(i)->GetTileID().column > (mapSizeX) - 2) && (board.at(i)->GetTileID().column < (mapSizeX) + 2))
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
            }
        }
        else if(board.at(i)->GetTileID().column >= (mapSizeX * 2) - 2)
        {
            if(board.at(i)->GetTileBiome() != POLE)
            {
                board.at(i)->SetTileBiome(OCEAN);
            }
        }
    }
}

