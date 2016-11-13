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
#include <QGraphicsProxyWidget>
#include <QProgressBar>

class Renderer
{
public:
    enum ItemGroup {MAP, TERRAIN, CITY_IMPROVEMENTS, OUTLINES, UNITS, FOG_OF_WAR, GUI_IMAGES, GUI_TEXT };

    Renderer(int mapSizeX);

    void UpdateScene(Map *map, GameView *view, TileData data, bool attackSelection);
    void UpdateUnits(Map *map, GameView *view, Unit *unit, bool unitMoved);
    void UpdateCityBorders(City* city, GameView *view, Nation owner);

    void DrawHexScene(Map *map, GameView *scene);
    void DrawGuiImages(QGraphicsScene *scene);
    void DrawCityBorders(QVector<City *> cities, GameView *view, Nation owner);
    void DrawUnits(QVector<Unit*> units, Map *map, GameView *view);
    void DrawCityHealthBars(QVector<City*> cities, GameView *scene);

    void LoadCities(QVector<City*> cities, GameView *view);

    void SetTileWorkedIcon(Tile* tile, GameView *view);
    void SetUnitNeedsOrders(Tile *tile, bool movedTo);
    void UpdateCityGrowthBar(City* city, GameView *view);
    void UpdateCityProductionBar(City* city, GameView *view);
    void UpdateCityHealthBar(City* city, GameView *view);

    void AddCityLabel(City* city, GameView *view);
    void AddCity(City* city, GameView *view);
    void AddUnit(Unit* unit, Map *map, GameView *view);

    void RemoveUnit(Unit *unit, GameView *view);

    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);
    void DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget*> wVect, QGraphicsScene *view);

    QString SetYieldDisplay(Map *map);
    QString SetYieldDisplay(Civilization *player);

    void AddCityHealthBars(City* city, GameView *view);
private:
//    QVector<City*> cities;
//    QVector<Unit*> units;

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPolygonItem*> mapBorders;
    QVector<QGraphicsPixmapItem*> resourcePixmap;
    QVector<QGraphicsEllipseItem*> tileCircles;

    QVector<QGraphicsPixmapItem*> tileWorked;
    QVector<QGraphicsProxyWidget*> ordersIcon;

    QVector<QGraphicsPolygonItem*> cityBorders;
    QVector<QGraphicsPixmapItem*> cityPixmap;
    QVector<QGraphicsProxyWidget*> cityLabels;
    QVector<QGraphicsRectItem*> cityHealthBars;
    QVector<QGraphicsRectItem*> cityProductionBars;
    QVector<QGraphicsRectItem*> cityGrowthBars;
    QVector<QGraphicsRectItem*> cityBarOutlines;
    QVector<QGraphicsProxyWidget*> cityPopulationLabels;

    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsLineItem*> unitGraphicsPath;
    QVector<QGraphicsRectItem*> unitHealthBars;

    QPolygon borders;
    CivColors *cc;

    QGraphicsRectItem *YieldDisplay;

    void SetOutlinePen(Nation owner);
    void AddUnitHealthBars(Unit *unit, Map *map, GameView *view);
    int mapSizeX;

};

#endif // RENDERER_H
