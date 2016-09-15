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

            if(odd)
            {
                tile->SetTileTexture(WATER);
            }
            else
            {
                tile->SetTileTexture(GRASS);
            }

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

QPixmap *Map::GetTilePixmap(int index)
{
    return terrain.at(index);
}

void Map::GenerateMap()
{
    double randDbl;

    for(int i = 0; i < board.size(); i++)
    {
        randDbl = static_cast<double>(rand() / static_cast<double>(RAND_MAX));
        if (randDbl < 0.3)
        {
            board.at(i)->SetTileTexture(WATER);
        }
        else if(randDbl < 0.6)
        {
            board.at(i)->SetTileTexture(GRASS);
        }
        else
        {
            board.at(i)->SetTileTexture(DESERT);
        }
    }
}

void Map::GenerateMapEdge()
{

}
