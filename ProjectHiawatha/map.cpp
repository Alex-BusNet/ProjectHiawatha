#include "map.h"
#include <QDebug>
#include <map>
#include <random>
#include <QTime>

Map::Map()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 20; //mapSizeX is doubled for map creation. i.e. a map size of 40 tiles will yield 80 columns.
    mapSizeY = 39;
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
    return terrain.at(index);
}

void Map::GenerateMap()
{
    int dbl;

    //These weights are used to tell the Mersenne Twister
    // how often to generate a particular number.
    // These numbers can be greater than one.
    double weights[] =
    {
        0.2,    //Water = 0
        0.5,    //Grass = 1
        0.2,    //Desert = 2
        0.1,    //Mountain = 3
        0.05,   //Hill = 4
        0.05    //Forest = 5
    };
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
    //          a minimum of 5 columnsby 3 rows in size
    // Step 3) Search map for plains biomes and determine locations to place hills or forests
    // Step 4) Place one hill or forest at indicated location, determine a random direction and place a hill or forest there,
    //          Repeat until max size of hill or forest biome has been reached
    // Step 5) Search map and determine proximity of hills, forests, and deserts to make sure no two biomes
    //          of the same type are too close to each other.
    // Step 6) Populate tiles with proper yields and resources.
    // Step 7) Set Player and AI spawns.
    //=====================
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
