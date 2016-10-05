#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "yield.h"

#include <QPixmap>
#include <QThread>
#include <QVector>

class Map
{
public:
    Map();
    Map(int mapSizeX, int mapSizeY);

    void InitHexMap();
    void InitTerrain();
    void InitYield();

    int GetBoardSize();
    QPoint GetHexTilePoint(int boardIndex, int ptIndex);
    TileType GetTileTypeAt(int index);
    Tile* GetTileAt(int index);
    Tile *GetTileFromCoord(int column, int row);
    Tile* GetTileFromCoord(TileID id);
    QPixmap *GetTilePixmap(int index);

//    void run();
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
