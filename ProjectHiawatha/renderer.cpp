#include "renderer.h"
#include "civcolors.h"
#include <QPen>
#include <QDebug>
#include <QProgressBar>
#include <QFuture>
#include <QtConcurrent>
#include <chrono>
#include "resources.h"
#include "unitcontroller.h"

//======================================
//  Render Layers
//
// Layer 0: Map Grip (initialized)
// Layer 1: Terrain
// Layer 2: Cities / Tile Improvements
// Layer 3: Tile Outlines (Game View)
// Layer 4: Units
// Layer 5: GUI Images
// Layer 6: Gui Text
// Layer 7: Fog of War
//
// Use setZValue() to change each item's render layer.
//=======================================

QPen outlinePen(QColor(255, 255, 255, 0));
QBrush brush(Qt::black);

Renderer::Renderer(int mapSizeX)
{
    #ifdef __APPLE__
        QDir bin(QCoreApplication::applicationDirPath());
        bin.cdUp();
        bin.cdUp();
        bin.cdUp();
        QDir::setCurrent(bin.absolutePath());
    #endif
    this->mapSizeX = mapSizeX;
    cc = new CivColors();
    outlinePen.setColor(cc->NO_NATION_PRIMARY);

    //Fog of War Images
    clouds = new QPixmap("Assets/Textures/Scaled/clouds.png");
    hidden = new QPixmap("Assets/Textures/Scaled/fogOfWar.png");

    //Strategic Resource Images
    ironPix = new QPixmap("Assets/Resources/iron.png");
    horsePix = new QPixmap("Assets/Resources/horses.png");
    uraniumPix = new QPixmap("Assets/Resources/uranium.png");
    aluminumPix = new QPixmap("Assets/Resources/aluminum.png");
    coalPix = new QPixmap("Assets/Resources/coal.png");
    oilPix = new QPixmap("Assets/Resources/oil.png");

    //Luxury Resource images
    wheatPix = new QPixmap("Assets/Resources/wheat.png");
    cattlePix = new QPixmap("Assets/Resources/cattle.png");
    deerPix = new QPixmap("Assets/Resources/deer.png");
    fishPix = new QPixmap("Assets/Resources/fish.png");
    whalePix = new QPixmap("Assets/Resources/whales.png");
    bananaPix = new QPixmap("Assets/Resources/bananas.png");
    goldResourcePix = new QPixmap("Assets/Resources/gold.png");
    gemsPix = new QPixmap("Assets/Resources/gems.png");
    marblePix = new QPixmap("Assets/Resources/marble.png");
    ivoryPix = new QPixmap("Assets/Resources/ivory.png");
    dyesPix = new QPixmap("Assets/Resources/dyes.png");
    spicesPix = new QPixmap("Assets/Resources/spices.png");
    silkPix = new QPixmap("Assets/Resources/silk.png");
    sugarPix = new QPixmap("Assets/Resources/sugar.png");
    cottonPix = new QPixmap("Assets/Resources/cotton.png");
    pearlsPix = new QPixmap("Assets/Resources/pearls.png");
    incencePix = new QPixmap("Assets/Resources/incense.png");
    winePix = new QPixmap("Assets/Resources/wine.png");
    silverPix = new QPixmap("Assets/Resources/silver.png");
    fursPix = new QPixmap("Assets/Resources/furs.png");
    sheepPix = new QPixmap("Assets/Resources/sheep.png");

    //Tile Icons
    tileWorkedIcon = new QPixmap("Assets/Citizens/worked.png");
    tileUnworked = new QPixmap("Assets/Citizens/unworked.png");
    fortified = new QPixmap("Assets/Icons/fortified.png");
    orders = new QPixmap("Assets/Icons/requiresOrders.png");

    //Tile improvement icons
    mine = new QPixmap("Assets/Resources/mine.png");
    plantation = new QPixmap("Assets/Resources/plantation.png");
    farm = new QPixmap("Assets/Resources/farm.png");
    tradePost = new QPixmap("Assets/Resources/trade_post.png");
    none = new QPixmap("Assets/Resources/noImprovement.png");
    camp = new QPixmap("Assets/Resources/camp.png");
    pasture = new QPixmap("Assets/Resources/pasture.png");
    fishingboat = new QPixmap("Assets/Resources/fishingboat.png");
    quarry = new QPixmap("Assets/Resources/quarry.png");
    oilwell = new QPixmap("Assets/Resources/oilwell.png");
}

Renderer::~Renderer()
{
#ifdef __APPLE__
    foreach(QGraphicsPolygonItem* t, tiles)
    {
        if(t != NULL)
            delete t;
    }

    foreach(QGraphicsPixmapItem* p, tilePixmap)
    {
        if(p != NULL)
            delete p;
    }

    foreach(QGraphicsPixmapItem* p, fogOfWar)
    {
        if(p != NULL)
            delete p;
    }

    foreach(QGraphicsPixmapItem* oi, ordersIcon)
    {
        if(oi != NULL)
            delete oi;
    }

    foreach(QGraphicsPolygonItem* mb, mapBorders)
    {
        if(mb != NULL)
            delete mb;
    }

    foreach(QGraphicsPixmapItem* p, resourcePixmap)
    {
        if(p != NULL)
            delete p;
    }

    foreach(QGraphicsEllipseItem* e, tileCircles)
    {
        if(e != NULL)
            delete e;
    }

    foreach(QGraphicsPixmapItem* tw, tileWorked)
    {
        if(tw != NULL)
            delete tw;
    }

    foreach(QGraphicsPixmapItem* fi, fortifiedIcon)
    {
        if(fi != NULL)
            delete fi;
    }

    foreach(QGraphicsPixmapItem* tii, tileImprovementIcons)
    {
        if(tii != NULL)
            delete tii;
    }

    foreach(QGraphicsPolygonItem* cb, cityBorders)
    {
        if(cb != NULL)
            delete cb;
    }

    foreach(QGraphicsPixmapItem* cp, cityPixmap)
    {
        if(cp != NULL)
            delete cp;
    }

    foreach(QGraphicsTextItem* cl, cityLabels)
    {
        if(cl != NULL)
            delete cl;
    }

    foreach(QGraphicsRectItem* chb, cityHealthBars)
    {
        if(chb != NULL)
            delete chb;
    }

    foreach(QGraphicsRectItem* cpb, cityProductionBars)
    {
        if(cpb != NULL)
            delete cpb;
    }

    foreach(QGraphicsRectItem* cgb, cityGrowthBars)
    {
        if(cgb != NULL)
            delete cgb;
    }

    foreach(QGraphicsPixmapItem* cbo, cityBarOutlines)
    {
        if(cbo != NULL)
            delete cbo;
    }

    foreach(QGraphicsTextItem* cpl, cityPopulationLabels)
    {
        if(cpl != NULL)
            delete cpl;
    }

    foreach(QGraphicsPixmapItem* up, unitPixmap)
    {
        if(up != NULL)
            delete up;
    }

    foreach(QGraphicsRectItem* uhb, unitHealthBars)
    {
        if(uhb != NULL)
            delete uhb;
    }

    foreach(QGraphicsLineItem* gl, gridLines)
    {
        if(gl != NULL)
            delete gl;
    }

    foreach(QGraphicsTextItem* gc, gridCoords)
    {
        if(gc != NULL)
            delete gc;
    }
#else    
    QFutureSynchronizer<void> synch;
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread9));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread8));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread7));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread6));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread5));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread4));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread3));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread2));
    synch.addFuture(QtConcurrent::run(this, Renderer::DecThread1));
#endif

    if(cc != NULL)
        delete cc;

    delete ironPix;
    delete horsePix;
    delete uraniumPix;
    delete aluminumPix;
    delete coalPix;
    delete oilPix;
    delete wheatPix;
    delete cattlePix;
    delete deerPix;
    delete fishPix;
    delete whalePix;
    delete bananaPix;
    delete goldResourcePix;
    delete gemsPix;
    delete marblePix;
    delete ivoryPix;
    delete dyesPix;
    delete spicesPix;
    delete silkPix;
    delete sugarPix;
    delete cottonPix;
    delete pearlsPix;
    delete incencePix;
    delete winePix;
    delete silverPix;
    delete fursPix;
    delete sheepPix;
    delete tileWorkedIcon;
    delete tileUnworked;
    delete fortified;
    delete mine;
    delete tradePost;
    delete plantation;
    delete oilwell;
    delete quarry;
    delete fishingboat;
    delete pasture;
    delete camp;
    delete farm;
    delete none;
    delete clouds;
    delete hidden;
    delete orders;

    #ifndef __APPLE__
        synch.waitForFinished();
    #endif
}

/*
 * DrawHexScene runs during init and is used to set up the
 * rendered version of the data map in the GameView
 */
void Renderer::DrawHexScene(Map *map, GameView *view)
{
    QPen circlePen(Qt::transparent);
#ifdef __APPLE__
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(map->GetTileAt(i)->GetTileID().row % 2 != 0)
        {
            map->GetTileAt(i)->SetTilePen(QPen(cc->NO_NATION_SECONDARY));
        }
        else
        {
            map->GetTileAt(i)->SetTilePen(outlinePen);
        }

        tiles.push_back(view->addPolygon(map->GetTileAt(i)->GetTilePolygon()));
        tiles.at(i)->setPen(map->GetTileAt(i)->GetTilePen());
        tiles.at(i)->setZValue(2);
        tiles.at(i)->setOpacity(0.5);

        tileCircles.push_back(view->addEllipse(map->GetTileAt(i)->GetTileRect(), circlePen));
        tileCircles.last()->setZValue(0);

        tilePixmap.push_back(view->addPixmap((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
        tilePixmap.at(i)->setZValue(-1);

        fogOfWar.push_back(view->addPixmap(*clouds));
        fogOfWar.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
        fogOfWar.at(i)->setZValue(7);

        if(map->GetTileAt(i)->GetStratResource() != NO_STRATEGIC)
        {
            switch(map->GetTileAt(i)->GetStratResource())
            {
            case IRON:
                resourcePixmap.push_back(view->addPixmap(*ironPix));
                break;
            case HORSES:
                resourcePixmap.push_back(view->addPixmap(*horsePix));
                break;
            case URANIUM:
                resourcePixmap.push_back(view->addPixmap(*uraniumPix));
                break;
            case ALUMINUM:
                resourcePixmap.push_back(view->addPixmap(*aluminumPix));
                break;
            case COAL:
                resourcePixmap.push_back(view->addPixmap(*coalPix));
                break;
            case OIL:
                resourcePixmap.push_back(view->addPixmap(*oilPix));
                break;
            }

            resourcePixmap.last()->setScale(0.5f);
            resourcePixmap.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint());
            resourcePixmap.last()->setZValue(1);
        }
        else if(map->GetTileAt(i)->GetLuxResource() != NO_LUXURY)
        {
            switch(map->GetTileAt(i)->GetLuxResource())
            {
            case WHEAT:
                resourcePixmap.push_back(view->addPixmap(*wheatPix));
                break;
            case CATTLE:
                resourcePixmap.push_back(view->addPixmap(*cattlePix));
                break;
            case DEER:
                resourcePixmap.push_back(view->addPixmap(*deerPix));
                break;
            case FISH:
                resourcePixmap.push_back(view->addPixmap(*fishPix));
                break;
            case WHALES:
                resourcePixmap.push_back(view->addPixmap(*whalePix));
                break;
            case BANANAS:
                resourcePixmap.push_back(view->addPixmap(*bananaPix));
                break;
            case GOLD_RESOURCE:
                resourcePixmap.push_back(view->addPixmap(*goldResourcePix));
                break;
            case GEMS:
                resourcePixmap.push_back(view->addPixmap(*gemsPix));
                break;
            case MARBLE:
                resourcePixmap.push_back(view->addPixmap(*marblePix));
                break;
            case IVORY:
                resourcePixmap.push_back(view->addPixmap(*ivoryPix));
                break;
            case DYES:
                resourcePixmap.push_back(view->addPixmap(*dyesPix));
                break;
            case SPICES:
                resourcePixmap.push_back(view->addPixmap(*spicesPix));
                break;
            case SILK:
                resourcePixmap.push_back(view->addPixmap(*silkPix));
                break;
            case SUGAR:
                resourcePixmap.push_back(view->addPixmap(*sugarPix));
                break;
            case COTTON:
                resourcePixmap.push_back(view->addPixmap(*cottonPix));
                break;
            case PEARLS:
                resourcePixmap.push_back(view->addPixmap(*pearlsPix));
                break;
            case INCENSE:
                resourcePixmap.push_back(view->addPixmap(*incencePix));
                break;
            case WINE:
                resourcePixmap.push_back(view->addPixmap(*winePix));
                break;
            case SILVER:
                resourcePixmap.push_back(view->addPixmap(*silverPix));
                break;
            case FURS:
                resourcePixmap.push_back(view->addPixmap(*fursPix));
                break;
            case SHEEP:
                resourcePixmap.push_back(view->addPixmap(*sheepPix));
                break;
            }

            resourcePixmap.last()->setScale(0.5f);
            resourcePixmap.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint());
            resourcePixmap.last()->setZValue(1);
        }

        if(map->GetTileAt(i)->GetControllingCivListIndex() == 0)
        {
            if(map->GetTileAt(i)->IsWorked)
            {
                tileWorked.push_back(view->addPixmap(*tileWorkedIcon));
            }
            else
            {
                tileWorked.push_back(view->addPixmap(*tileUnworked));
            }

            tileWorked.last()->setScale(0.6f);
            tileWorked.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 23, map->GetTileAt(i)->GetResourceIconPoint().y() + 10);
            tileWorked.last()->setZValue(2);
        }
        else
        {
            tileWorked.push_back(view->addPixmap(*tileUnworked));
            tileWorked.last()->setOpacity(0);
            tileWorked.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 23, map->GetTileAt(i)->GetResourceIconPoint().y() + 10);
        }

        //Every tile created has a requires orders icon, fortified icon
        // and a tile improvement icon slot. The tile improvement icon is
        // changed as the Player or AI builds builds different improvements.
        // The requires orders icon and fortified icons are turned on and off by changing
        // their opacity levels when told to do so by the manager.
#ifdef __APPLE__
QDir bin(QCoreApplication::applicationDirPath());
bin.cdUp();
bin.cdUp();
bin.cdUp();
QDir::setCurrent(bin.absolutePath());
#endif
        QPixmap *orders = new QPixmap("Assets/Icons/requiresOrders.png");
        ordersIcon.push_back(view->addPixmap(*orders));
        ordersIcon.last()->setOpacity(0);
        ordersIcon.last()->setZValue(3);
        ordersIcon.last()->setPos(map->GetTileAt(i)->GetItemTexturePoint().x() + 30, map->GetTileAt(i)->GetItemTexturePoint().y());

        fortifiedIcon.push_back(view->addPixmap(*fortified));
        fortifiedIcon.last()->setScale(0.3f);
        fortifiedIcon.last()->setOpacity(0);
        fortifiedIcon.last()->setZValue(3);
        fortifiedIcon.last()->setPos(map->GetTileAt(i)->GetItemTexturePoint().x(), map->GetTileAt(i)->GetItemTexturePoint().y());

        tileImprovementIcons.push_back(view->addPixmap(*none));
        tileImprovementIcons.last()->setScale(0.5f);
        tileImprovementIcons.last()->setOpacity(0);
        tileImprovementIcons.last()->setZValue(2);
        tileImprovementIcons.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 43, map->GetTileAt(i)->GetResourceIconPoint().y());
    }
#else

    QFutureSynchronizer<void> synch;
    synch.addFuture(QtConcurrent::run(this, Renderer::DrawThread1, map));
    synch.addFuture(QtConcurrent::run(this, Renderer::DrawThread2, map));
    synch.addFuture(QtConcurrent::run(this, Renderer::DrawThread3, map));
    synch.addFuture(QtConcurrent::run(this, Renderer::DrawThread4, map));

    synch.waitForFinished();

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    begin = std::chrono::steady_clock::now();

    foreach(QGraphicsPolygonItem *pi, tiles)
    {
        view->addItem(pi);
    }

    end = std::chrono::steady_clock::now();
    qDebug() << "   Tile polygons added in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

    begin = std::chrono::steady_clock::now();
    foreach(QGraphicsEllipseItem *ei, tileCircles)
    {
        ei->setPen(circlePen);
        view->addItem(ei);
    }

    end = std::chrono::steady_clock::now();
    qDebug() << "   Tile circles added in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

    begin = std::chrono::steady_clock::now();

    foreach(QGraphicsPixmapItem *pi, fogOfWar)
    {
        view->addItem(pi);
    }

    end = std::chrono::steady_clock::now();
    qDebug() << "   Fog of War added in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

    begin = std::chrono::steady_clock::now();

    foreach(QGraphicsPixmapItem *pi, resourcePixmap)
    {
       view->addItem(pi);
    }

    end = std::chrono::steady_clock::now();
    qDebug() << "   Resource pixmaps added in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";

#endif
    qDebug() << "DrawHexScene finished";
}

void Renderer::UpdateScene(Map *map, GameView *view, QQueue<SelectData> *data)
{
    if(data->isEmpty())
        return;

    int index;
    SelectData selDat;
    while(!data->isEmpty())
    {
        selDat = data->dequeue();
        index = selDat.index;

        // Prevent the renderer trying to retrieve a
        // tileCircle that is out of range.
        if(index >= map->GetBoardSize())
        {
            return;
        }

        // The tile has been selected by the player
        if(selDat.player && !selDat.target)
        {
            outlinePen.setColor(Qt::yellow);
            outlinePen.setWidth(2);
            map->GetTileAt(index)->Selected = true;
        }
        // The tile is contains a unit that can be targeted
        else if(!selDat.player && selDat.target)
        {
            outlinePen.setColor(Qt::red);
            outlinePen.setWidth(2);
            map->GetTileAt(index)->Selected = false;
        }
        // Return the tile to it's default state.
        else if(!selDat.player && !selDat.target)
        {
            outlinePen.setColor(Qt::transparent);
            outlinePen.setWidth(1);
            map->GetTileAt(index)->Selected = false;
        }

        // Update the tileCircle vector and GameView with the
        // new circle information.
        tileCircles.at(index)->setPen(outlinePen);
        tileCircles.at(index)->setZValue(0);
    }
}

void Renderer::UpdateUnits(Map *map, GameView *view, Unit *unit, bool unitMoved)
{
    unitPixmap.at(unit->GetPixmapIndex())->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint());
    unitPixmap.at(unit->GetPixmapIndex())->setZValue(3);

    // If the unit has moved, or their health has changed,
    // update the unit's health bar.
    if((unit->GetHealth() / unit->GetMaxHealth()) != 1 || unitMoved)
    {
        view->removeItem(unitHealthBars.at(unit->GetHealthBarIndex()));

        unitHealthBars.replace(unit->GetHealthBarIndex(), view->addRect(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().x(),
                                                                        map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().y() + unit->GetUnitIcon()->height() + 1,
                                                                        ceil(35 * (static_cast<double>(unit->GetHealth()) / unit->GetMaxHealth())), 5, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green))));
        unitHealthBars.at(unit->GetHealthBarIndex())->setZValue(5);
    }
}

/*
 * UpdateCityBorders() is used when a city expands or has it's ownership changed.
 */
void Renderer::UpdateCityBorders(City *city, GameView *view, Nation owner)
{
    qDebug() << "   UpdateCityBorders()";
    SetOutlinePen(owner);

    view->removeItem(cityBorders.at(city->GetCityRenderIndex()));
    cityBorders.replace(city->GetCityRenderIndex(), view->addPolygon(city->GetCityBorders(), outlinePen));
    cityBorders.at(city->GetCityRenderIndex())->setZValue(2);

    if(city->GetCityID() < 100)
    {
        outlinePen.setColor(Qt::green);
        foreach(Tile* t, city->borderQueue)
        {
            tileCircles.at(t->GetTileIndex())->setPen(outlinePen);
        }
        outlinePen.setColor(Qt::transparent);
        foreach(Tile* t, city->GetControlledTiles())
        {
            tileCircles.at(t->GetTileIndex())->setPen(outlinePen);
        }
    }
}

void Renderer::UpdateTileVisibilty(QQueue<ViewData> *data, GameView *view)
{
    if(data->isEmpty())
        return;

    ViewData i;
    while(!data->isEmpty())
    {
        i = data->dequeue();

        switch(i.state)
        {
        case DISCOVERED:
            this->DiscoverTile(i.index, view);
            break;
        case VISIBLE:
            this->SetTileVisibility(i.index, true, false);
            break;
        case HIDDEN:
            this->SetTileVisibility(i.index, false, false);
            break;
        default:
            break;
        }
    }
}

void Renderer::DiscoverTile(int index, GameView *view)
{
    QPointF pos = fogOfWar.at(index)->pos();
    view->removeItem(fogOfWar.at(index));
    fogOfWar.replace(index, view->addPixmap(*hidden));
    fogOfWar.at(index)->setPos(pos);
    fogOfWar.at(index)->setOpacity(0);
    fogOfWar.at(index)->setZValue(7);

    if(!view->items().contains(tilePixmap.at(index)))
        view->addItem(tilePixmap.at(index));

    if(!view->items().contains(ordersIcon.at(index)))
        view->addItem(ordersIcon.at(index));

    if(!view->items().contains(fortifiedIcon.at(index)))
        view->addItem(fortifiedIcon.at(index));

    if(!view->items().contains(tileWorked.at(index)))
        view->addItem(tileWorked.at(index));

    if(!view->items().contains(tileImprovementIcons.at(index)))
        view->addItem(tileImprovementIcons.at(index));
}

void Renderer::SetTileVisibility(int index, bool viewable, bool toggle)
{
    if(viewable)
    {
        fogOfWar.at(index)->setOpacity(0);
    }
    else if(toggle)
    {
        fogOfWar.at(index)->setOpacity(1);
    }
    else
    {
        fogOfWar.at(index)->setOpacity(0.5);
    }
}

void Renderer::DrawGuiText(Map *map, QVector<QGraphicsTextItem*> tVect, GameView *view)
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tVect.push_back(view->addText(QString("%1,%2\n%3").arg(map->GetTileAt(i)->GetTileID().column).arg(map->GetTileAt(i)->GetTileID().row).arg(i)));
        tVect.at(i)->setPos(map->GetTileAt(i)->GetTextCenter());
        tVect.at(i)->setZValue(5);
        if(map->GetTileAt(i)->GetTileType() == ICE)
        {
            tVect.at(i)->setDefaultTextColor(Qt::red);
        }
        else
        {
            tVect.at(i)->setDefaultTextColor(Qt::yellow);
        }
    }

}

void Renderer::DrawGridLines(GameView *view)
{
    int rowLines, colLines, width, height;

    rowLines = view->GetScene()->height() / 74;
    colLines = view->GetScene()->width() / 44;
    QPen line(Qt::red);

    width = view->GetScene()->sceneRect().width();
    height = view->GetScene()->sceneRect().height();

    for(int i = 0; i < rowLines + 1; i++)
    {
        gridLines.push_back(view->addLine(0, 74 * i, width, 74 * i, line));
    }

    for(int i = 0; i < colLines; i++)
    {
        gridLines.push_back(view->addLine(44 * i, 0, 44 * i, height, line));
    }

    for(int i = 0; i < rowLines; i++)
    {
        for(int j = 0; j < colLines; j++)
        {
            gridCoords.push_back(view->addText(QString("%1,%2").arg(j).arg(i)));
            gridCoords.last()->setPos((44 * j) + 10, (74 * i) + 15);
            gridCoords.last()->setZValue(6);
            gridCoords.last()->setDefaultTextColor(Qt::red);
            gridCoords.last()->setScale(0.5);
        }
    }
}

/*
 * SetOutlinePen is an internal function used to change the
 * color of the pen used to draw all non-image items in the gameView
 */
void Renderer::SetOutlinePen(Nation owner)
{
    switch(owner)
    {
    case NO_NATION:
        outlinePen.setColor(cc->NO_NATION_PRIMARY);
        break;
    case America:
        outlinePen.setColor(cc->AMERICA_PRIMARY);
        break;
    case Germany:
        outlinePen.setColor(cc->GERMANY_PRIMARY);
        break;
    case India:
        outlinePen.setColor(cc->INDIA_PRIMARY);
        break;
    case China:
        outlinePen.setColor(cc->CHINA_PRIMARY);
        break;
    case Mongolia:
        outlinePen.setColor(cc->MONGOLIA_PRIMARY);
        break;
    case Aztec:
        outlinePen.setColor(cc->AZTEC_PRIMARY);
        break;
    case France:
        outlinePen.setColor(cc->FRANCE_PRIMARY);
        break;
    case Iroquois:
        outlinePen.setColor(cc->IROQUOIS_PRIMARY);
        break;
    case Greece:
        outlinePen.setColor(cc->GREECE_PRIMARY);
        break;
    case Rome:
        outlinePen.setColor(cc->ROME_PRIMARY);
        break;
    case England:
        outlinePen.setColor(cc->ENGLAND_PRIMARY);
        break;
    case Arabia:
        outlinePen.setColor(cc->ARABIA_PRIMARY);
        break;
    case Persia:
        outlinePen.setColor(cc->PERSIA_PRIMARY);
        break;
    case Russia:
        outlinePen.setColor(cc->RUSSIA_PRIMARY);
        break;
    case Japan:
        outlinePen.setColor(cc->JAPAN_PRIMARY);
        break;
    case Egypt:
        outlinePen.setColor(cc->EGYPT_PRIMARY);
        break;
    default:
        outlinePen.setColor(cc->NO_NATION_PRIMARY);
        break;
    }

    outlinePen.setWidth(5);
}

/*
 * AddUnitHealthBars is an internal function
 * that is a sub-function of AddUnit.
 */
void Renderer::AddUnitHealthBars(Unit *unit, Map *map, GameView *view)
{
    QRect *health = new QRect(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().x(),
                              map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().y() + unit->GetUnitIcon()->height() + 1,
                              35, 5);

    unit->SetHealthBarIndex(unitHealthBars.size());
    unitHealthBars.push_back(view->addRect(health, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green))));
    unitHealthBars.last()->setZValue(6);


    if(map->GetTileAt(unit->GetTileIndex())->GetControllingCivListIndex() == 0)
    {
        this->SetUnitNeedsOrders(unit->GetTileIndex(), true);
    }
}

void Renderer::DrawThread1(Map *map)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(map->GetTileAt(i)->GetTileID().row % 2 != 0)
        {
            map->GetTileAt(i)->SetTilePen(QPen(cc->NO_NATION_SECONDARY));
        }
        else
        {
            map->GetTileAt(i)->SetTilePen(outlinePen);
        }

        tiles.push_back(new QGraphicsPolygonItem(map->GetTileAt(i)->GetTilePolygon()));
        tiles.at(i)->setPen(map->GetTileAt(i)->GetTilePen());
        tiles.at(i)->setZValue(2);
        tiles.at(i)->setOpacity(0.5);

        tilePixmap.push_back(new QGraphicsPixmapItem((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
        tilePixmap.at(i)->setZValue(-1);
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    qDebug() << "   Render thread 1 completed in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";
}

void Renderer::DrawThread2(Map *map)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        fogOfWar.push_back(new QGraphicsPixmapItem(*clouds));
        fogOfWar.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
        fogOfWar.at(i)->setZValue(7);

        ordersIcon.push_back(new QGraphicsPixmapItem(*orders));
        ordersIcon.last()->setOpacity(0);
        ordersIcon.last()->setZValue(3);
        ordersIcon.last()->setPos(map->GetTileAt(i)->GetItemTexturePoint().x() + 30, map->GetTileAt(i)->GetItemTexturePoint().y());
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    qDebug() << "   Render thread 2 completed in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";
}

void Renderer::DrawThread3(Map *map)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tileCircles.push_back(new QGraphicsEllipseItem(map->GetTileAt(i)->GetTileRect()));
        tileCircles.last()->setZValue(0);

        if(map->GetTileAt(i)->GetStratResource() != NO_STRATEGIC)
        {
            switch(map->GetTileAt(i)->GetStratResource())
            {
            case IRON:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*ironPix));
                break;
            case HORSES:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*horsePix));
                break;
            case URANIUM:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*uraniumPix));
                break;
            case ALUMINUM:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*aluminumPix));
                break;
            case COAL:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*coalPix));
                break;
            case OIL:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*oilPix));
                break;
            }

            resourcePixmap.last()->setScale(0.5f);
            resourcePixmap.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint());
            resourcePixmap.last()->setZValue(1);
        }
        else if(map->GetTileAt(i)->GetLuxResource() != NO_LUXURY)
        {
            switch(map->GetTileAt(i)->GetLuxResource())
            {
            case WHEAT:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*wheatPix));
                break;
            case CATTLE:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*cattlePix));
                break;
            case DEER:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*deerPix));
                break;
            case FISH:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*fishPix));
                break;
            case WHALES:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*whalePix));
                break;
            case BANANAS:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*bananaPix));
                break;
            case GOLD_RESOURCE:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*goldResourcePix));
                break;
            case GEMS:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*gemsPix));
                break;
            case MARBLE:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*marblePix));
                break;
            case IVORY:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*ivoryPix));
                break;
            case DYES:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*dyesPix));
                break;
            case SPICES:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*spicesPix));
                break;
            case SILK:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*silkPix));
                break;
            case SUGAR:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*sugarPix));
                break;
            case COTTON:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*cottonPix));
                break;
            case PEARLS:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*pearlsPix));
                break;
            case INCENSE:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*incencePix));
                break;
            case WINE:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*winePix));
                break;
            case SILVER:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*silverPix));
                break;
            case FURS:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*fursPix));
                break;
            case SHEEP:
                resourcePixmap.push_back(new QGraphicsPixmapItem(*sheepPix));
                break;
            }

            resourcePixmap.last()->setScale(0.5f);
            resourcePixmap.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint());
            resourcePixmap.last()->setZValue(1);
        }

        if(map->GetTileAt(i)->GetControllingCivListIndex() == 0)
        {
            if(map->GetTileAt(i)->IsWorked)
            {
                tileWorked.push_back(new QGraphicsPixmapItem(*tileWorkedIcon));
            }
            else
            {
                if(!map->GetTileAt(i)->HasCity)
                    tileWorked.push_back(new QGraphicsPixmapItem(*tileUnworked));
            }

            tileWorked.last()->setScale(0.6f);
            tileWorked.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 23, map->GetTileAt(i)->GetResourceIconPoint().y() + 10);
            tileWorked.last()->setZValue(2);
        }
        else
        {
            tileWorked.push_back(new QGraphicsPixmapItem(*tileUnworked));
            tileWorked.last()->setOpacity(0);
            tileWorked.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 23, map->GetTileAt(i)->GetResourceIconPoint().y() + 10);
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    qDebug() << "   Render thread 3 completed in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";
}

void Renderer::DrawThread4(Map *map)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        fortifiedIcon.push_back(new QGraphicsPixmapItem(*fortified));
        fortifiedIcon.last()->setScale(0.3f);
        fortifiedIcon.last()->setOpacity(0);
        fortifiedIcon.last()->setZValue(3);
        fortifiedIcon.last()->setPos(map->GetTileAt(i)->GetItemTexturePoint().x(), map->GetTileAt(i)->GetItemTexturePoint().y());

        tileImprovementIcons.push_back(new QGraphicsPixmapItem(*none));
        tileImprovementIcons.last()->setScale(0.5f);
        tileImprovementIcons.last()->setOpacity(0);
        tileImprovementIcons.last()->setZValue(2);
        tileImprovementIcons.last()->setPos(map->GetTileAt(i)->GetResourceIconPoint().x() + 43, map->GetTileAt(i)->GetResourceIconPoint().y());
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    qDebug() << "   Render thread 4 completed in:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";
}

void Renderer::DecThread1()
{
    if(!tiles.isEmpty())
    {
        tiles.detach();
        foreach(QGraphicsPolygonItem* t, tiles)
        {
            if(t != NULL)
                delete t;
        }
    }

    if(!tilePixmap.isEmpty())
    {
        tilePixmap.detach();
        foreach(QGraphicsPixmapItem* tp, tilePixmap)
        {
            if(tp != NULL)
                delete tp;
        }
    }
}

void Renderer::DecThread2()
{
    if(!fogOfWar.isEmpty())
    {
        fogOfWar.detach();
        foreach(QGraphicsPixmapItem* fow, fogOfWar)
        {
            if(fow != NULL)
                delete fow;
        }
    }

    if(!ordersIcon.isEmpty())
    {
        ordersIcon.detach();
        foreach(QGraphicsPixmapItem* oi, ordersIcon)
        {
            if(oi != NULL)
                delete oi;
        }
    }
}

void Renderer::DecThread3()
{
    if(!resourcePixmap.isEmpty())
    {
        resourcePixmap.detach();
        foreach(QGraphicsPixmapItem* rp, resourcePixmap)
        {
            if(rp != NULL)
                delete rp;
        }
    }

    if(!tileCircles.isEmpty())
    {
        tileCircles.detach();
        foreach(QGraphicsEllipseItem* e, tileCircles)
        {
            if(e != NULL)
                delete e;
        }
    }

    if(!tileWorked.isEmpty())
    {
        tileWorked.detach();
        foreach(QGraphicsPixmapItem* tw, tileWorked)
        {
            if(tw != NULL)
                delete tw;
        }
    }
}

void Renderer::DecThread4()
{
    if(!fortifiedIcon.isEmpty())
    {
        fortifiedIcon.detach();
        foreach(QGraphicsPixmapItem* fi, fortifiedIcon)
        {
            if(fi != NULL)
                delete fi;
        }
    }

    if(!tileImprovementIcons.isEmpty())
    {
        tileImprovementIcons.detach();
        foreach(QGraphicsPixmapItem* tii, tileImprovementIcons)
        {
            if(tii != NULL)
                delete tii;
        }
    }
}

void Renderer::DecThread5()
{
    if(!cityBorders.isEmpty())
    {
        cityBorders.detach();
        foreach(QGraphicsPolygonItem* cb, cityBorders)
        {
            if(cb != NULL)
                delete cb;
        }
    }

    if(!cityLabels.isEmpty())
    {
        cityLabels.detach();
        foreach(QGraphicsTextItem* cl, cityLabels)
        {
            if(cl != NULL)
                delete cl;
        }
    }
}

void Renderer::DecThread6()
{
    if(!cityPixmap.isEmpty())
    {
        cityPixmap.detach();
        foreach(QGraphicsPixmapItem* cp, cityPixmap)
        {
            if(cp != NULL)
                delete cp;
        }
    }

    if(!unitPixmap.isEmpty())
    {
        unitPixmap.detach();
        foreach(QGraphicsPixmapItem* up, unitPixmap)
        {
            if(up != NULL)
                delete up;
        }
    }
}

void Renderer::DecThread7()
{
    if(!cityHealthBars.isEmpty())
    {
        cityHealthBars.detach();
        foreach(QGraphicsRectItem* chb, cityHealthBars)
        {
            if(chb != NULL)
                delete chb;
        }
    }

    if(!unitHealthBars.isEmpty())
    {
        unitHealthBars.detach();
        foreach(QGraphicsRectItem* uhb, unitHealthBars)
        {
            if(uhb != NULL)
                 delete uhb;
        }
    }
}

void Renderer::DecThread8()
{
    if(!cityProductionBars.isEmpty())
    {
        cityProductionBars.detach();
        foreach(QGraphicsRectItem* cpb, cityProductionBars)
        {
            if(cpb != NULL)
                delete cpb;
        }
    }

    if(!cityGrowthBars.isEmpty())
    {
        cityGrowthBars.detach();
        foreach(QGraphicsRectItem* cgb, cityGrowthBars)
        {
            if(cgb != NULL)
                delete cgb;
        }
    }
}

void Renderer::DecThread9()
{
    if(!cityBarOutlines.isEmpty())
    {
        cityBarOutlines.detach();
        foreach(QGraphicsPixmapItem* cbo, cityBarOutlines)
        {
            if(cbo != NULL)
                delete cbo;
        }
    }

    if(!cityPopulationLabels.isEmpty())
    {
        cityPopulationLabels.detach();
        foreach(QGraphicsTextItem* cpl, cityPopulationLabels)
        {
            if(cpl != NULL)
                delete cpl;
        }
    }
}

/*
 * AddCityHealthBars adds the health and
 * progression meters to every city created
 */
void Renderer::AddCityHealthBars(City *city, GameView *view)
{
    //------------------------------------------------------------------------------
#ifdef __APPLE__
QDir bin(QCoreApplication::applicationDirPath());
bin.cdUp();
bin.cdUp();
bin.cdUp();
QDir::setCurrent(bin.absolutePath());
#endif
    QPixmap *cityOutlines = new QPixmap("Assets/UI/CityStatusBarOutline.png");

    cityBarOutlines.push_back(view->addPixmap(*cityOutlines));
    cityBarOutlines.last()->setZValue(5);
    cityBarOutlines.last()->setPos(city->GetCityTile()->GetItemTexturePoint().x() - 23,
                                   city->GetCityTile()->GetCityLabelPoint().y() + 13);

    QRect *health = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                              city->GetCityTile()->GetCityLabelPoint().y() + 15,
                              65, 5);

    cityHealthBars.push_back(view->addRect(health, QPen(QColor(Qt::transparent)), QBrush(QColor(Qt::green))));
    cityHealthBars.last()->setZValue(4);

    //--------------------------------------------------------------------------------
    QRect *growth = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                              city->GetCityTile()->GetCityLabelPoint().y() + 20,
                              1, 2);

    cityGrowthBars.push_back(view->addRect(growth, QPen(QColor(Qt::transparent)), QBrush(QColor(Qt::cyan))));
    cityGrowthBars.last()->setZValue(4);

    //-------------------------------------------------------------------------------
    QRect *production = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                                  city->GetCityTile()->GetCityLabelPoint().y() + 13,
                                  1, 2);

    cityProductionBars.push_back(view->addRect(production, QPen(QColor(Qt::transparent)), QBrush(QColor(255, 113, 0, 255))));
    cityProductionBars.last()->setZValue(4);

    //---------------------------------------------------------------------------------
    QString population = QString("%1").arg(city->GetCitizenCount());
    cityPopulationLabels.push_back(view->addText(population));

    cityPopulationLabels.last()->setDefaultTextColor(Qt::white);
    cityPopulationLabels.last()->setZValue(4);
    cityPopulationLabels.last()->setFont(QFont("Helvetica", 7, QFont::Normal));
    cityPopulationLabels.last()->font().setStretch(QFont::ExtraCondensed);
    cityPopulationLabels.last()->setPos(city->GetCityTile()->GetItemTexturePoint().x() - 25,
                            city->GetCityTile()->GetCityLabelPoint().y() + 8);
    //--------------------------------------------------------------------------------
}

void Renderer::PrepareForDelete(GameView *view)
{
    QList<QGraphicsItem*> itemList = view->items();
    foreach(QGraphicsItem* i, itemList)
    {
        view->removeItem(i);
    }
}

/*
 * DrawCityBorders is user to draw a city's borders for the first time.
 */
void Renderer::DrawCityBorders(City *city, GameView *view, Nation owner)
{
    qDebug() << "   Drawing City borders for" << city->GetName();
    SetOutlinePen(owner);

    cityBorders.push_back(view->addPolygon(city->GetCityBorders(), outlinePen));
    cityBorders.last()->setPen(outlinePen);
    cityBorders.last()->setZValue(3);

    qDebug() << "   --done";
}

/*
 * LoadCities is used during game initialization to
 * load all cities that are controlled by a civ
 */
void Renderer::LoadCities(QVector<City*> cities, GameView *view)
{
    for(int i = 0; i < cities.size(); i++)
    {
        this->AddCity(cities.at(i), view, false);
    }
}

/*
 * This function is used to set show that player what tiles
 * are currently being worked by a city.
 */
void Renderer::SetTileWorkedIcon(Tile *tile, GameView *view)
{
    int index = tile->GetTileIndex();

    if(view->items().contains(tileWorked.at(index)))
        view->removeItem(tileWorked.at(index));

    if(tile->IsWorked)
    {
        tileWorked.replace(index, view->addPixmap(*tileWorkedIcon));
    }
    else
    {
        tileWorked.replace(index, view->addPixmap(*tileUnworked));
    }

    tileWorked.at(index)->setScale(0.6f);
    tileWorked.at(index)->setPos(tile->GetResourceIconPoint().x() + 23, tile->GetResourceIconPoint().y() + 10);
    tileWorked.at(index)->setZValue(2);
}

/*
 * SetUnitNeedsOrders is used to show the player what
 * units can be given orders.
 */
void Renderer::SetUnitNeedsOrders(int tile, bool needsOrders)
{
    if(needsOrders)
        ordersIcon.at(tile)->setOpacity(100);
    else
        ordersIcon.at(tile)->setOpacity(0);
}

/*
 * SetFortifyIcon is used to show the player what
 * unit, including AI units, are currently fortified.
 */
void Renderer::SetFortifyIcon(int tile, bool unfortify)
{
    if(!unfortify)
        fortifiedIcon.at(tile)->setOpacity(100);
    else
        fortifiedIcon.at(tile)->setOpacity(0);
}

/*
 * SetTileImprovement is used to set the proper icon
 * the passed tile's improvement type slot. Different
 * improvements have different effects on a city's yield.
 */
void Renderer::SetTileImprovement(TileImprovement ti, Tile* tile, GameView *view)
{
    int index = tile->GetTileIndex();

    switch(ti)
    {
    case MINE:
        tileImprovementIcons.replace(index, view->addPixmap(*mine));
        break;
    case TRADE_POST:
        tileImprovementIcons.replace(index, view->addPixmap(*tradePost));
        break;
    case PLANTATION:
        tileImprovementIcons.replace(index, view->addPixmap(*plantation));
        break;
    case FARM:
        tileImprovementIcons.replace(index, view->addPixmap(*farm));
        break;
    case NONE:
        tileImprovementIcons.replace(index, view->addPixmap(*none));
        break;
    case QUARRY:
        tileImprovementIcons.replace(index, view->addPixmap(*quarry));
        break;
    case FISHING_BOAT:
        tileImprovementIcons.replace(index, view->addPixmap(*fishingboat));
        break;
    case PASTURE:
        tileImprovementIcons.replace(index, view->addPixmap(*pasture));
        break;
    case CAMP:
        tileImprovementIcons.replace(index, view->addPixmap(*camp));
        break;
    case OIL_WELL:
        tileImprovementIcons.replace(index, view->addPixmap(*oilwell));
    case ROAD:
        break;
    default:
        break;
    }

    if(ti != NONE)
    {
        tileImprovementIcons.at(index)->setOpacity(100);
    }
    else
    {
        tileImprovementIcons.at(index)->setOpacity(0);
    }

    tileImprovementIcons.at(index)->setZValue(3);
    tileImprovementIcons.at(index)->setScale(0.5f);
    tileImprovementIcons.at(index)->setPos(tile->GetResourceIconPoint().x() + 43, tile->GetResourceIconPoint().y());
}

void Renderer::SetTileTooltip(int index, Yield tileYield, Nation owner, QString tileID)
{
    tiles.at(index)->setToolTip(QString("Tile Owned by: %1\nGold: %2\nProd: %3\nScience: %4\nFood: %5\nCulture: %6\n%7")
                            .arg(UnitController::NationName(owner))
                            .arg(tileYield.GetGoldYield())
                            .arg(tileYield.GetProductionYield())
                            .arg(tileYield.GetScienceYield())
                            .arg(tileYield.GetFoodYield())
                            .arg(tileYield.GetCultureYield())
                            .arg(tileID));
}

/*
 * UpdateCityGrowthBar is used to show current progress
 * towards generating a new citizen in a particular city
 * as well as keeping the population counter up to date.
 */
void Renderer::UpdateCityGrowthBar(City *city, GameView *view)
{
    int index = city->GetCityRenderIndex();
    static bool textAdjusted = false;

    view->removeItem(cityGrowthBars.at(index));
    int barSize;

    if(city->IsStagnant())
    {
        barSize = 1;
    }
    else
    {
        barSize = 65 - ceil(65 * (city->GetTurnsToNewCitizen() / (2 *(static_cast<double>(city->GetFoodNeededToGrow() / city->GetFoodSurplus())))));
    }

    barSize = barSize <= 0 ? 1 : barSize;

    QRect *growth = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                              city->GetCityTile()->GetCityLabelPoint().y() + 20,
                              barSize, 2);

    cityGrowthBars.replace(index, view->addRect(growth, QPen(QColor(Qt::transparent)), QBrush(QColor(Qt::cyan))));
    cityGrowthBars.at(index)->setZValue(4);

    cityPopulationLabels.at(index)->setPlainText(QString("%1").arg(city->GetCitizenCount()));

    if(city->GetCitizenCount() == 10 && !textAdjusted)
    {
        textAdjusted = true;
        cityPopulationLabels.at(index)->setFont(QFont("Helvetica", 6, QFont::Normal));
        cityPopulationLabels.at(index)->moveBy(-2, 0);
    }
}

/*
 * UpdateCityProductionBar is used to show current progress
 * towards completing a particular city's project. When the bar
 * is full, then production has been completed.
 */
void Renderer::UpdateCityProductionBar(City *city, GameView *view)
{
    int index = city->GetCityRenderIndex();
    view->removeItem(cityProductionBars.at(index));

    int barSize = ceil(65 * (static_cast<double>(city->getAccumulatedProduction()) / (city->getCurrentProductionCost() + 1)));

    if(city->getProductionName() == "No Current Production")
    {
        barSize = 1;
    }
    else
    {
        barSize = barSize <= 0 ? 1 : barSize;
    }

    QRect *prod = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                                  city->GetCityTile()->GetCityLabelPoint().y() + 13,
                                  barSize, 2);

    cityProductionBars.replace(index, view->addRect(prod, QPen(QColor(Qt::transparent)), QBrush(QColor(255, 113, 0, 255))));
    cityProductionBars.at(index)->setZValue(4);

    cityLabels.at(index)->setPlainText(QString(" %1 [%2] ").arg(city->GetName()).arg(city->GetCityStrength()));

}

/*
 * UpdateCityHealthBar is only used once a city has
 * been attack or has healed from being attacked.
 */
void Renderer::UpdateCityHealthBar(City *city, GameView *view)
{
    int index = city->GetCityRenderIndex();
    view->removeItem(cityHealthBars.at(index));

    int barSize = ceil(65 * (static_cast<double>(city->GetCityHealth()) / city->GetMaxHealth()));

    barSize = barSize <= 0 ? 1 : barSize;

    QRect *health = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 13,
                              city->GetCityTile()->GetCityLabelPoint().y() + 15,
                              barSize, 5);

    cityHealthBars.replace(index, view->addRect(health, QPen(QColor(Qt::transparent)), QBrush(QColor(Qt::green))));
    cityHealthBars.last()->setZValue(4);
}
/*
 * AddCityLabel is part of city creation and
 * is used to display the name of a city above the
 * progression bars.
 */
void Renderer::AddCityLabel(City* city, GameView *view)
{
    QString label = QString(" %1 [%2] ").arg(city->GetName()).arg(city->GetCityStrength());

    cityLabels.push_back(view->addText(label));
    cityLabels.last()->setDefaultTextColor(Qt::white);
    cityLabels.last()->setZValue(6);
    cityLabels.last()->setFont(QFont("Helvetica", 8, QFont::Normal));
    cityLabels.last()->font().setStretch(QFont::ExtraCondensed);


    cityLabels.last()->setPos(city->GetCityTile()->GetCenter().x() - (cityLabels.last()->boundingRect().width() / 2) - 2,
                              city->GetCityTile()->GetCityLabelPoint().y() - 5);
}

/*
 * AddCity is used whenever a city has been founded
 * or conquered by another civ. The conqueredCity flag
 * indicates the circumstance under which the city is being
 * created, and adjusts the visual details of the city
 * appropriately.
 */
void Renderer::AddCity(City *city, GameView *view, bool conqueredCity)
{
#ifdef __APPLE__
QDir bin(QCoreApplication::applicationDirPath());
bin.cdUp();
bin.cdUp();
bin.cdUp();
QDir::setCurrent(bin.absolutePath());
#endif
//    QPixmap *cityImage = new QPixmap("Assets/Icons/CityIcon4944_alt.png");
    cityPixmap.push_back(view->addPixmap(QPixmap("Assets/Icons/CityIcon4944_alt.png")));
    cityPixmap.last()->setZValue(1);
    cityPixmap.last()->setScale(1.0f);
    cityPixmap.last()->setPos(city->GetCityTile()->GetTexturePoint().x() + 22, city->GetCityTile()->GetTexturePoint().y() + 24);

    if(!conqueredCity)
    {
        city->SetCityRenderIndex(this->cityPixmap.size()-1);

        this->AddCityLabel(city, view);
        this->AddCityHealthBars(city, view);
    }
    else
    {
        qDebug() << "   Updating conquered city";
        this->UpdateCityBorders(city, view, city->GetControllingCiv());
        this->UpdateCityGrowthBar(city, view);
        this->UpdateCityHealthBar(city, view);
        this->UpdateCityProductionBar(city, view);
    }
}

/*
 * AddUnit is responsible for getting the correct
 * icon for the new unit and filling in the white
 * space with the appropriate color of the owning civ.
 */
void Renderer::AddUnit(Unit *unit, Map *map, GameView *view)
{
    QPixmap *unitPix;
    QImage *unitImage;
    unitImage = unit->GetUnitIcon();
    QRgb color = cc->GetCivColor(unit->GetOwner()).rgba();
    for(int j = 0; j < 32; j++)
    {
        for(int k = 0; k < 32; k++)
        {
            if((unitImage->pixelColor(j,k) != QColor(Qt::black))
                    && (unitImage->pixelColor(j,k).alpha() != 0))
            {
                unitImage->setPixelColor(j, k, color);
            }
        }
    }
    unit->SetUnitImage(unitImage);
    unitPix = new QPixmap(unitPix->fromImage(*unitImage));
    unitPixmap.push_back(view->addPixmap(*unitPix));
    unitPixmap.last()->setZValue(3);
    unitPixmap.last()->setScale(1.0f);
    unit->SetPixmapIndex(unitPixmap.size() - 1);
    unitPixmap.last()->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint());
    AddUnitHealthBars(unit, map, view);
}

/*
 * RemoveUnit is used to remove a killed unit from the scene
 */
void Renderer::RemoveUnit(Unit *unit, GameView *view)
{
    view->removeItem(unitHealthBars.at(unit->GetHealthBarIndex()));
    view->removeItem(unitPixmap.at(unit->GetPixmapIndex()));
}

/*
 * RemoveCity is used to remove a city's borders and icon from the scene.
 */
void Renderer::RemoveCity(City *city, GameView *view)
{
    int index = city->GetCityRenderIndex();

    view->removeItem(cityBorders.at(index));
    view->removeItem(cityPixmap.at(index));
}

/*
 * DrawUnits is used during game initalization to
 * run AddUnit for all units a civ owns.
 */
void Renderer::DrawUnits(QVector<Unit *> units, Map *map, GameView *view)
{
    for(int i = 0; i < units.size(); i++)
    {
        this->AddUnit(units.at(i), map, view);
    }
}
