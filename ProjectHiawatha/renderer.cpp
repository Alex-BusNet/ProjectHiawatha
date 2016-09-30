
#include "renderer.h"
#include <QPen>
#include <QDebug>

#ifndef MAXSIZE
#define MAXSIZE 100
#endif

//======================================
//  Render Layers
//
// Layer 0: Map Grip (initialized)
// Layer 1: Terrain
// Layer 2: Cities / Tile Improvements
// Layer 3: Tile Outlines (Game View)
// Layer 4: Units
// Layer 5: Fog of War
// Layer 6: GUI Images
// Layer 7: Gui Text
//
// Use setZValue() to change each item's render layer.
//=======================================

QPen outlinePen(Qt::black);
QBrush brush(Qt::black);

Renderer::Renderer()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 30; //max = 32
    mapSizeY = 39;  //max = 40

}

void Renderer::DrawHexScene(Map *map, QVector<QGraphicsPolygonItem*> polyVect, QVector<QGraphicsPixmapItem*> itemVect, GameView *scene)
{
    QPen pen;
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(map->GetTileAt(i)->Selected)
        {
            pen.setColor(Qt::yellow);
        }
        else
        {
            // This sets the pen to be transparent
            pen.setColor(QColor(255, 255, 255, 0));
        }

        polyVect.push_back(scene->addPolygon(map->GetTileAt(i)->GetTilePolygon()));
        polyVect.at(i)->setPen(pen);
        polyVect.at(i)->setZValue(1);

        itemVect.push_back(scene->addPixmap((*(map->GetTilePixmap(i)))));
        itemVect.at(i)->setScale(0.64f); //textureScale = 0.32f * drawScale
        itemVect.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }
}

void Renderer::UpdateScene(QGraphicsView *view)
{

}

void Renderer::DrawGuiText(Map *map, QVector<QGraphicsTextItem*> tVect, GameView *view)
{
    ////THIS COMMENTED SECTION IS FOR DEBUGGING PURPOSES.
    /// IF YOU NEED IT, COMMENT OUT THE YIELD DISPLAY TEXT
//    for(int i = 0; i < map->GetBoardSize(); i++)
//    {
//        tVect.push_back(view->addText(QString("%1,%2").arg(map->GetTileAt(i)->GetTileID().column).arg(map->GetTileAt(i)->GetTileID().row)));
//        tVect.at(i)->setPos(map->GetTileAt(i)->GetTextCenter());
//        tVect.at(i)->setZValue(7);
//        tVect.at(i)->setDefaultTextColor(Qt::red);
//    }

    //This is a placeholder, it will need to be re-adjusted once the player class is added.
    tVect.push_back(view->addText(QString("Gold: %1  Production: %2  Food: %3  Science: %4  Culture: %5")
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::GOLD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::PRODUCTION))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::FOOD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::RESEARCH))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::CULTURE))));
    tVect.at(0)->setPos(5, 2);
    tVect.at(0)->setZValue(7);
    tVect.at(0)->setDefaultTextColor(Qt::white);

}

void Renderer::DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget *> wVect, QGraphicsScene *view)
{
    wVect.push_back(view->addWidget(obj));
}

void Renderer::AddItemToGroup(QGraphicsItem *item, Renderer::ItemGroup iGroup)
{
    switch(iGroup)
    {
    case MAP:
        item->setGroup(&MapGrid);
        break;
    case TERRAIN:
        item->setGroup(&Terrain);
        break;
    case CITY_IMPROVEMENTS:
        item->setGroup(&CitiesImprovements);
        break;
    case OUTLINES:
        item->setGroup(&TileOutlines);
        break;
    case UNITS:
        item->setGroup(&Units);
        break;
    case FOG_OF_WAR:
        item->setGroup(&FogOfWar);
        break;
    case GUI_IMAGES:
        item->setGroup(&GUI_Images);
        break;
    case GUI_TEXT:
        item->setGroup(&GUI_Text);
        break;
    default:
        break;
    }
}

void Renderer::DrawGuiImages(QGraphicsScene *scene)
{
    GUI_Images.setZValue(6);
    YieldDisplay = scene->addRect(0, 0, 200, 50, outlinePen, brush);
    YieldDisplay->setZValue(6);
}

void Renderer::DrawTestUnits(Map *map, QVector<QGraphicsPixmapItem*> uVect, GameView* view)
{
    QPixmap *TestUnit = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
    uVect.push_back(view->addPixmap(*TestUnit));
    uVect.at(0)->setZValue(4);
    for(int i = 0; i <map->GetBoardSize(); i++)
    {
        if(map->GetTileTypeAt(i) == GRASS || map->GetTileTypeAt(i) == DESERT)
        {
            uVect.at(0)->setPos(map->GetHexTilePoint(i, 0));
            map->GetTileAt(i)->ContainsUnit = true;
            break;
        }
    }
}

void Renderer::DrawTestCities(Map *map, QVector<QGraphicsPixmapItem*> cVect, GameView *view)
{
    QPixmap *TestCity = new QPixmap("../ProjectHiawatha/Assets/Icons/CityIcon4944.png");
    cVect.push_back(view->addPixmap(*TestCity));
    cVect.at(0)->setZValue(2);
    for(int i = 0; i <map->GetBoardSize(); i++)
    {
        if(map->GetTileTypeAt(i) == GRASS || map->GetTileTypeAt(i) == DESERT)
        {
            if(map->GetTileAt(i)->ContainsUnit == false)
            {
                cVect.at(0)->setPos(map->GetTileAt(i)->GetTexturePoint());
                break;
            }
        }
    }
}
