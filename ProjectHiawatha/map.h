#ifndef MAP_H
#define MAP_H

#include "datatypes.h"
#include "tile.h"
#include "yield.h"

#include <QPixmap>
#include <QThread>
#include <QVector>

class Civilization;
class City;

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
    OccupantData GetODFromTileAt(int index);
    OccupantData GetODFromCoord(int column, int row);
    MapData *GetDataAt(int index);
    MapData *GetDataFromCoord(int column, int row);

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
    void SetOccupantDataAt(int index, OccupantData newOd);
    void DefineCityBordersNew(City* city);

    City *CreateCity(int cityTileIndex, Civilization *founder, bool isCapital);

    void WriteMapSaveData(QJsonObject &obj) const;
    void ReadMapSaveData(QJsonObject &obj);

private:
//    QVector<Tile*> board;
    QVector<MapData*> board;
    QVector<QPixmap*> terrain;

    //May be changed later
    int mapSizeX;
    int mapSizeY;
    int oceanScaleFactor;

    void GenerateTiles();
    void GenerateMap();
    void GenerateMapEdge();
    void GenerateBiomes();
    void GenerateResources();
    void CleanMap();

};

#endif // MAP_H
