#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "yield.h"

#include <QPixmap>
#include <QVector>



class Map
{
public:
    Map();

    void InitHexMap();
    void InitTerrain();
    void InitYield();

    int GetBoardSize();
    QPoint GetHexTilePoint(int boardIndex, int ptIndex);
    TileType GetTileTypeAt(int index);
    Tile* GetTileAt(int index);
    QPixmap *GetTilePixmap(int index);

private:
    QVector<Tile*> board;
    QVector<QPixmap*> terrain;
    QVector<Yield*> tileYield;

    //May be changed later
    int mapSizeX;
    int mapSizeY;

    void GenerateMap();
    void GenerateMapEdge();
    void CleanMap();
    void GenerateBiomes();
};

#endif // MAP_H
