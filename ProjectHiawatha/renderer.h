#ifndef RENDERER_H
#define RENDERER_H

#include "map.h"
#include "tile.h"
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

    // To be Deprecated
    void InitMap();

private:
    //QVector<City*> cities;
    //QVector<Unit*> units;

    //May be changed later
    int mapSizeX;
    int mapSizeY;
};

#endif // RENDERER_H
