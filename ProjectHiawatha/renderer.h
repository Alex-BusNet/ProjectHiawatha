#ifndef RENDERER_H
#define RENDERER_H

#include "gameview.h"
#include "map.h"
#include "tile.h"
#include "unit.h"
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

    void DrawHexScene(Map *map, GameView *scene);
    void UpdateScene(Map *map, GameScene *scene);
    void DrawGuiImages(QGraphicsScene *scene);

    void DrawCityBorders(Map *map);
    void DrawDebugCityBorders(Map *map, GameScene *scene);
    void DrawCities(QVector<City*> cities, QGraphicsScene *scene);
    void DrawUnits(QVector<Unit*> units, QGraphicsScene *scene);

    void DrawTestUnits(Map *map, GameView *view);
    void DrawTestCities(Map *map, GameView *view);
    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);
    void DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget*> wVect, QGraphicsScene *view);
    QString SetYieldDisplay(Map *map);
    QString SetYieldDisplay(Yield *yield);

private:
    QVector<City*> cities;
    QVector<Unit*> units;

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPolygonItem*> mapBorders;
    QVector<QGraphicsPolygonItem*> cityBorders;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsPixmapItem*> cityPixmap;

    QPolygon borders;
    CivColors *cc;

    QGraphicsRectItem *YieldDisplay;

    void SetOutlinePen(Nation owner);

};

#endif // RENDERER_H
