#include "map.h"
#include <QDebug>

Map::Map()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 31; //maxOnScreen = 51
    mapSizeY = 39;  //maxOnScreen = 48 (for now)

    //31 tiles X 39 tiles for using the screen in 1200 X 900 px mode;
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

        for(int j = 0; j < (mapSizeX / 2); j++)
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

    GenerateMap();
    GenerateMapEdge();
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

    int GrassTilesRemaining = board.size() * 0.4,
            WaterTilesRemaining = board.size() * 0.3,
            DesertTilesRemaining = board.size() * 0.2,
            MountainTilesRemaining = board.size() * 0.1,
            HillTilesRemaining = GrassTilesRemaining * 0.1,
            ForestTilesRemaining = GrassTilesRemaining * 0.1;

    int TotalGrassTiles = GrassTilesRemaining,
            TotalWaterTiles = WaterTilesRemaining,
            TotalDesertTiles = DesertTilesRemaining,
            TotalMountainTiles = MountainTilesRemaining,
            TotalHillTiles = HillTilesRemaining,
            TotalForestTiles = ForestTilesRemaining;

    double tileProb;

    for(int i = 0; i < board.size(); i++)
    {
newrand:
        dbl = (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * 6;
        if(dbl == 0 && WaterTilesRemaining > 0)
        {
            tileProb = 0.3;
            board.at(i)->SetTileType(WATER);
            board.at(i)->SetTileTexture(WATER);
            WaterTilesRemaining--;
        }
        else if (dbl == 1 && GrassTilesRemaining > 0)
        {
            tileProb = 0.4;
            board.at(i)->SetTileType(GRASS);
            board.at(i)->SetTileTexture(GRASS);
            GrassTilesRemaining--;
        }
        else if (dbl == 2 && DesertTilesRemaining > 0)
        {
            tileProb = 0.2;
            board.at(i)->SetTileType(DESERT);
            board.at(i)->SetTileTexture(DESERT);
            DesertTilesRemaining--;
        }
        else if(dbl == 3 && MountainTilesRemaining > 0)
        {
            tileProb = 0.1;
            board.at(i)->SetTileType(MOUNTAIN);
            board.at(i)->SetTileTexture(MOUNTAIN);
            MountainTilesRemaining--;
        }
        else if(dbl == 4 && HillTilesRemaining > 0)
        {
            tileProb = 0.4 * 0.1;
            board.at(i)->SetTileType(HILL);
            board.at(i)->SetTileTexture(HILL);
            HillTilesRemaining--;
        }
        else if(dbl == 5 && ForestTilesRemaining > 0)
        {
            tileProb = 0.4 * 0.1;
            board.at(i)->SetTileType(FOREST);
            board.at(i)->SetTileTexture(FOREST);
            ForestTilesRemaining--;
        }
        else
        {
            goto newrand;
        }
    }
}

void Map::GenerateMapEdge()
{

}
