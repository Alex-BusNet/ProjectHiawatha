#ifndef RENDERER_H
#define RENDERER_H

#include "gameview.h"
#include "map.h"
#include "tile.h"
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPainter>
#include <QVector>

class Renderer
{
public:
    enum ItemGroup {MAP, TERRAIN, CITY_IMPROVEMENTS, OUTLINES, UNITS, FOG_OF_WAR, GUI_IMAGES, GUI_TEXT };
    Renderer();

    //I'm going to change these functions so that they are
    // more useful and not wasting space.
    void DrawMap(Map *map, QPainter &painter);
    void DrawHex(Map *map, QPainter &painter);
    void DrawHexScene(Map *map, QVector<QGraphicsPolygonItem *> polyVect, QVector<QGraphicsPixmapItem*> itemVect, GameView *scene);
    void UpdateScene(QGraphicsView *view);
    void DrawGuiImages(QGraphicsScene *scene);
//    void DrawCities(QVector<City*> cities, QGraphicsScene *scene);
//    void DrawUnits(QVector<Unit*> units, QGraphicsScene *scene);
    void DrawTestUnits(Map *map, QVector<QGraphicsPixmapItem*> uVect, GameView *view);
    void DrawTestCities(Map *map, QVector<QGraphicsPixmapItem *> cVect, GameView *view);
    void DrawGuiText(QGraphicsScene *scene);
    void DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget*> wVect, QGraphicsScene *view);

    void AddItemToGroup(QGraphicsItem *item, ItemGroup iGroup);

private:
    //QVector<City*> cities;
    //QVector<Unit*> units;

    QGraphicsItemGroup MapGrid;             //Layer 0
    QGraphicsItemGroup Terrain;             //Layer 1
    QGraphicsItemGroup CitiesImprovements;  //Layer 2
    QGraphicsItemGroup TileOutlines;        //Layer 3
    QGraphicsItemGroup Units;               //Layer 4
    QGraphicsItemGroup FogOfWar;            //Layer 5
    QGraphicsItemGroup GUI_Images;          //Layer 6
    QGraphicsItemGroup GUI_Text;            //Layer 7

    //May be changed later
    int mapSizeX;
    int mapSizeY;

    QGraphicsRectItem *YieldDisplay;

};

#endif // RENDERER_H
