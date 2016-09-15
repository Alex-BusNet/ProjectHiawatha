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

    Tile* GetTileAt(int index);
    int GetBoardSize();
    QPixmap *GetTilePixmap(int index);

private:
    QVector<Tile*> board;
    QVector<QPixmap*> terrain;
    QVector<Yield*> tileYield;

    //May be changed later
    int mapSizeX;
    int mapSizeY;

    void GenerateMap();
    void CleanMap();
};

#endif // MAP_H
