#include "map.h"
#include <QDebug>
#include <map>
#include <random>
#include <QTime>
#include <ctime>
#include "biome.h"

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
    float posX = 12;
    float posY = 0;
    float rowOffset = 37;
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

            posX += 74;
            column += 2;
        }

        if(!odd)
        {
            posX = 12 + rowOffset;
        }
        else
        {
            posX = 12;
        }
        row++;
        posY += 22;
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

// boardIndex = the idex value where the tile is stored in board
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
            }
        }
        else if (dbl == 1)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(GRASS);
                board.at(i)->SetTileTexture(GRASS);
            }
        }
        else if (dbl == 2)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(DESERT);
                board.at(i)->SetTileTexture(DESERT);
            }
        }
        else if(dbl == 3)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(MOUNTAIN);
                board.at(i)->SetTileTexture(MOUNTAIN);
            }
        }
        else if(dbl == 4)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(HILL);
                board.at(i)->SetTileTexture(HILL);
            }
        }
        else if(dbl == 5)
        {
            if(board.at(i)->GetTileBiome() != POLE && board.at(i)->GetTileBiome() != OCEAN)
            {
                board.at(i)->SetTileType(FOREST);
                board.at(i)->SetTileTexture(FOREST);
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
        }
        if(board.at(i)->GetTileBiome() == OCEAN)
        {
            board.at(i)->SetTileTexture(WATER);
            board.at(i)->SetTileType(WATER);
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
    // and a settle instead of a city to start.
    //
    //=====================

    City *city;
    Unit *unit;
    srand(time(0));
    int index;

    for(int i = 0; i < civs.size(); i++)
    {
newrand:
        index = (static_cast<double>(rand()) / RAND_MAX) * (board.size() / 2);

        if(board.at(index)->GetTileType() == ICE || board.at(index)->GetTileType() == WATER || board.at(index)->GetTileType() == MOUNTAIN)
        {
            goto newrand;
        }

        if(!board.at(index)->ContainsUnit && !board.at(index)->HasCity)
        {
            city = new City();
            city->SetCityAsCaptial();
            city->SetCityTile(board.at(index));
            city->SetControllingCiv(civs.at(i)->getCiv());
            civs.at(i)->AddCity(city);

            board.at(index)->HasCity = true;
            board.at(index)->SetControllingCiv(civs.at(i)->getCiv());
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

