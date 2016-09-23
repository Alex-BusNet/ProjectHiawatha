#ifndef RENDERER_H
#define RENDERER_H

#include "map.h"
#include "tile.h"
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPainter>
#include <QVector>

class Renderer
{
public:
    Renderer();

    //I'm going to change these functions so that they are
    // more useful and not wasting space.
    void DrawMap(Map *map, QPainter &painter);
    void DrawHex(Map *map, QPainter &painter);
    void DrawHexScene(Map *map, QGraphicsView *view, QGraphicsScene *scene);
    void UpdateScene(QGraphicsView *view);

private:
    //QVector<City*> cities;
    //QVector<Unit*> units;

    QGraphicsItemGroup MapGrid;             //Layer 0
    QGraphicsItemGroup Terrain;             //Layer 1
    QGraphicsItemGroup CitiesImprovements;  //Layer 2
    QGraphicsItemGroup TileOutlines;        //Layer 3
    QGraphicsItemGroup Units;               //Layer 4
    QGraphicsItemGroup FogOfWar;            //Layer 5
    QGraphicsItemGroup GUI;                 //Layer 6

    //May be changed later
    int mapSizeX;
    int mapSizeY;
};

#endif // RENDERER_H
