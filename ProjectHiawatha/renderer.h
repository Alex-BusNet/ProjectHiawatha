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
#include <QQueue>

class Renderer
{
public:
    enum ItemGroup {MAP, TERRAIN, CITY_IMPROVEMENTS, OUTLINES, UNITS, FOG_OF_WAR, GUI_IMAGES, GUI_TEXT };

    Renderer(int mapSizeX);

    void UpdateScene(Map *map, GameView *view, QQueue<SelectData> *data);
    void UpdateUnits(Map *map, GameView *view, Unit *unit, bool unitMoved);
    void UpdateCityBorders(City* city, GameView *view, Nation owner);

    void DrawHexScene(Map map, GameView *scene);
    void DrawCityBorders(City *city, GameView *view, Nation owner);
    void DrawUnits(QVector<Unit*> units, Map *map, GameView *view);

    void LoadCities(QVector<City*> cities, GameView *view);

    void SetTileWorkedIcon(Tile* tile, GameView *view);
    void SetUnitNeedsOrders(int tile, bool needsOrders);
    void SetFortifyIcon(int tile, bool unfortify);
    void SetTileImprovement(TileImprovement ti, Tile *tile, GameView *view);
    void UpdateCityGrowthBar(City* city, GameView *view);
    void UpdateCityProductionBar(City* city, GameView *view);
    void UpdateCityHealthBar(City* city, GameView *view);

    void AddCityLabel(City* city, GameView *view);
    void AddCity(City* city, GameView *view, bool conqueredCity);
    void AddUnit(Unit* unit, Map *map, GameView *view);

    void RemoveUnit(Unit *unit, GameView *view);
    void RemoveCity(City* city, GameView *view);

    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);

    void AddCityHealthBars(City* city, GameView *view);
private:

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPolygonItem*> mapBorders;
    QVector<QGraphicsPixmapItem*> resourcePixmap;
    QVector<QGraphicsEllipseItem*> tileCircles;

    QVector<QGraphicsPixmapItem*> tileWorked;
    QVector<QGraphicsProxyWidget*> ordersIcon;
    QVector<QGraphicsPixmapItem*> fortifiedIcon;
    QVector<QGraphicsPixmapItem*> tileImprovementIcons;

    QVector<QGraphicsPolygonItem*> cityBorders;

    QVector<QGraphicsPixmapItem*> cityPixmap;
    QVector<QGraphicsTextItem*> cityLabels;
    QVector<QGraphicsRectItem*> cityHealthBars;
    QVector<QGraphicsRectItem*> cityProductionBars;
    QVector<QGraphicsRectItem*> cityGrowthBars;
    QVector<QGraphicsPixmapItem*> cityBarOutlines;
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

    QPixmap *ironPix, *horsePix, *uraniumPix, *aluminumPix, *coalPix, *oilPix;
    QPixmap *wheatPix, *cattlePix, *deerPix, *fishPix, *whalePix, *bananaPix, *goldResourcePix;
    QPixmap *gemsPix, *marblePix, *ivoryPix, *dyesPix, *spicesPix, *silkPix, *sugarPix, *cottonPix, *pearlsPix;
    QPixmap *incencePix, *winePix, *silverPix, *fursPix, *sheepPix;
    QPixmap *tileWorkedIcon, *tileUnworked, *fortified;
    QPixmap *mine, *tradePost, *plantation, *farm, *none;

};

#endif // RENDERER_H
