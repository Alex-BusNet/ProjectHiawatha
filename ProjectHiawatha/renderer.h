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
#include <QPolygon>

class Renderer
{
public:
    enum ItemGroup {MAP, TERRAIN, CITY_IMPROVEMENTS, OUTLINES, UNITS, FOG_OF_WAR, GUI_IMAGES, GUI_TEXT };

    Renderer(int mapSizeX);

    void DrawHexScene(Map *map, GameView *scene);
    void UpdateScene(Map *map, GameScene *scene);
    void UpdateUnits(Map *map, GameView *view, Unit *unit);
    void DrawGuiImages(QGraphicsScene *scene);

    void DrawCityBorders(Map *map, QVector<City *> cities, GameScene *scene);
    void DrawCityBorders(QVector<City *> cities, GameScene *scene, Nation owner);
    void DrawDebugCityBorders(Map *map, GameScene *scene);
    void LoadCities(QVector<City*> cities, Map *map, GameView *view);
    void DrawUnits(QVector<Unit*> units, Map *map, GameView *view);

    void DrawTestUnits(Map *map, GameView *view);
    void DrawTestCities(Map *map, GameView *view);
    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);
    void DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget*> wVect, QGraphicsScene *view);
    void DrawUnitPath(GameScene *scene, Unit *unit);
    void ClearUnitPath(GameScene *scene);

    QString SetYieldDisplay(Map *map);
    QString SetYieldDisplay(Yield *yield);

private:
//    QVector<City*> cities;
//    QVector<Unit*> units;

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPolygonItem*> mapBorders;
    QVector<QGraphicsPolygonItem*> cityBorders;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsPixmapItem*> cityPixmap;
    QVector<QGraphicsLineItem*> unitGraphicsPath;

    QPolygon borders;
    CivColors *cc;

    QGraphicsRectItem *YieldDisplay;

    void SetOutlinePen(Nation owner);
    int mapSizeX;

};

#endif // RENDERER_H
