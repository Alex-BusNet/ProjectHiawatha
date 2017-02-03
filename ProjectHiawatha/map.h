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
    ~Map();

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

    int GetMapSizeX();

    QList<Tile*> GetNeighbors(Tile *node);
    QList<Tile*> GetNeighborsRange(Tile *node, int range);
    QList<Tile*> GetMaximumExpasionTiles(Tile *cityCenter);
    bool listContains(QList<Tile*> list, Tile *tile);
    bool setContains(QSet<Tile*> set, Tile *tile);
//    void run();
    void SpawnCivs(QVector<Civilization*> civs);
    void GetTileQueue(City *city);

    void DefineCityBordersNew(City* city);

    City *CreateCity(int cityTileIndex, Civilization *founder, bool isCapital);

    void WriteMapSaveData(QJsonObject &obj) const;

private:
    QVector<Tile*> board;
    QVector<QPixmap*> terrain;

    //May be changed later
    int mapSizeX;
    int mapSizeY;
    int oceanScaleFactor;

    void GenerateMap();
    void GenerateMapEdge();
    void GenerateBiomes();
    void GenerateResources();
    void CleanMap();

};

#endif // MAP_H
