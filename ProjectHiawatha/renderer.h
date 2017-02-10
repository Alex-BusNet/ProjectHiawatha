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
    ~Renderer();

    void UpdateScene(Map *map, GameView *view, QQueue<SelectData> *data);
    void UpdateUnits(Map *map, GameView *view, Unit *unit, bool unitMoved);
    void UpdateCityBorders(City* city, GameView *view, Nation owner);
    void UpdateTileVisibilty(QQueue<ViewData> *data, GameView *view);

    void DiscoverTile(int index, GameView *view);
    void SetTileVisibility(int index, bool viewable, bool toggle);

    void DrawHexScene(Map *map, GameView *scene);
    void DrawCityBorders(City *city, GameView *view, Nation owner);
    void DrawUnits(QVector<Unit*> units, Map *map, GameView *view);

    void LoadCities(QVector<City*> cities, GameView *view);

    void SetTileWorkedIcon(Tile* tile, GameView *view);
    void SetTileImprovement(TileImprovement ti, Tile *tile, GameView *view);
    void SetTileTooltip(int index, Yield tileYield, Nation owner, QString tileID);
    void SetFortifyIcon(int tile, bool unfortify);
    void SetUnitNeedsOrders(int tile, bool needsOrders);

    void UpdateCityGrowthBar(City* city, GameView *view);
    void UpdateCityProductionBar(City* city, GameView *view);
    void UpdateCityHealthBar(City* city, GameView *view);

    void AddCityLabel(City* city, GameView *view);
    void AddCity(City* city, GameView *view, bool conqueredCity);
    void AddUnit(Unit* unit, Map *map, GameView *view);

    void RemoveUnit(Unit *unit, GameView *view);
    void RemoveCity(City* city, GameView *view);

    void DrawGuiText(Map *map, QVector<QGraphicsTextItem *> tVect, GameView *view);
    void DrawGridLines(GameView *view);

    void AddCityHealthBars(City* city, GameView *view);

    void PrepareForDelete(GameView *view);

private:

    QVector<QGraphicsPolygonItem*> tiles;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> fogOfWar;
    QVector<QGraphicsPolygonItem*> mapBorders;
    QVector<QGraphicsPixmapItem*> resourcePixmap;
    QVector<QGraphicsEllipseItem*> tileCircles;

    QVector<QGraphicsPixmapItem*> tileWorked;
    QVector<QGraphicsPixmapItem*> ordersIcon;
    QVector<QGraphicsPixmapItem*> fortifiedIcon;
    QVector<QGraphicsPixmapItem*> tileImprovementIcons;

    QVector<QGraphicsPolygonItem*> cityBorders;

    QVector<QGraphicsPixmapItem*> cityPixmap;
    QVector<QGraphicsTextItem*> cityLabels;
    QVector<QGraphicsRectItem*> cityHealthBars;
    QVector<QGraphicsRectItem*> cityProductionBars;
    QVector<QGraphicsRectItem*> cityGrowthBars;
    QVector<QGraphicsPixmapItem*> cityBarOutlines;
    QVector<QGraphicsTextItem*> cityPopulationLabels;

    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsRectItem*> unitHealthBars;

    QVector<QGraphicsLineItem*> gridLines;
    QVector<QGraphicsTextItem*> gridCoords;

    QPolygon borders;
    CivColors *cc;

    void SetOutlinePen(Nation owner);
    void AddUnitHealthBars(Unit *unit, Map *map, GameView *view);

    void DrawThread1(Map *map);
    void DrawThread2(Map *map);
    void DrawThread3(Map *map);
    void DrawThread4(Map *map);

    void DecThread1();
    void DecThread2();
    void DecThread3();
    void DecThread4();
    void DecThread5();
    void DecThread6();
    void DecThread7();
    void DecThread8();
    void DecThread9();

    int mapSizeX;

    QPixmap *ironPix, *horsePix, *uraniumPix, *aluminumPix, *coalPix, *oilPix;
    QPixmap *wheatPix, *cattlePix, *deerPix, *fishPix, *whalePix, *bananaPix, *goldResourcePix;
    QPixmap *gemsPix, *marblePix, *ivoryPix, *dyesPix, *spicesPix, *silkPix, *sugarPix, *cottonPix, *pearlsPix;
    QPixmap *incencePix, *winePix, *silverPix, *fursPix, *sheepPix;
    QPixmap *tileWorkedIcon, *tileUnworked, *fortified;
    QPixmap *mine, *tradePost, *plantation, *farm, *none;
    QPixmap *clouds, *hidden, *orders;
};

#endif // RENDERER_H
