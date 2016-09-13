#ifndef RENDERER_H
#define RENDERER_H

#include "tile.h"
#include <QGraphicsView>
#include <QPainter>
#include <QVector>

class Renderer
{
public:
    Renderer();
    void DrawMap(QPainter &painter);
    void DrawHex(QPainter &painter);

    void InitMap();
    void InitHexMap();

private:
    QVector<Tile*> board;
    //QVector<City*> cities;
    //QVector<Unit*> units;

    //May be changed later
    int mapSizeX;
    int mapSizeY;
};

#endif // RENDERER_H
