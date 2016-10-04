#ifndef RENDERER_H
#define RENDERER_H

#include "gameview.h"
#include "map.h"
#include "tile.h"
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPainter>
#include <QVector>
#include <qlabel.h>

class Renderer
{
public:
    enum ItemGroup {MAP, TERRAIN, CITY_IMPROVEMENTS, OUTLINES, UNITS, FOG_OF_WAR, GUI_IMAGES, GUI_TEXT };

    Renderer();

    void DrawHexScene(Map *map, GameView *scene, bool redraw);
    void UpdateScene(Map *map, QGraphicsView *view);
    void DrawGuiImages(QGraphicsScene *scene);

    void DrawCityBorders(Map *map);
//    void DrawCities(QVector<City*> cities, QGraphicsScene *scene);
//    void DrawUnits(QVector<Unit*> units, QGraphicsScene *scene);

    void DrawTestUnits(Map *map, GameView *view);
    void DrawTestCities(Map *map, GameView *view);
    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);
    void DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget*> wVect, QGraphicsScene *view);
    QString SetYieldDisplay(Map *map);

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

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsPixmapItem*> cityPixmap;

    QPolygon borders;

    //May be changed later
    int mapSizeX;
    int mapSizeY;

    QGraphicsRectItem *YieldDisplay;

};

#endif // RENDERER_H
