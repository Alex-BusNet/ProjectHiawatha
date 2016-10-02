
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

QPen outlinePen(QColor(255, 255, 255, 0));
QBrush brush(Qt::black);

Renderer::Renderer()
{

}

void Renderer::DrawHexScene(Map *map, GameView *scene)
{
//    QPen pen;
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(map->GetTileAt(i)->Selected)
        {
            outlinePen.setColor(Qt::yellow);
        }
        else
        {
            // This sets the pen to be transparent
            if(map->GetTileAt(i)->GetControllingCiv() == NO_NATION)
            {
                outlinePen.setColor(QColor(255, 255, 255, 0));
            }
            else if(map->GetTileAt(i)->GetControllingCiv() == America)
            {
                outlinePen.setColor(QColor(0, 255, 0, 255));
            }
            else if(map->GetTileAt(i)->GetControllingCiv() == Germany)
            {
                outlinePen.setColor(QColor(255, 0, 0, 255));
            }
            else if(map->GetTileAt(i)->GetControllingCiv() == India)
            {
                outlinePen.setColor(QColor(0, 0, 255, 255));
            }
            else if(map->GetTileAt(i)->GetControllingCiv() == China)
            {
                outlinePen.setColor(QColor(0, 250, 150, 255));
            }
        }

        map->GetTileAt(i)->SetTilePen(outlinePen);
        tiles.push_back(scene->addPolygon(map->GetTileAt(i)->GetTilePolygon()));
        tiles.at(i)->setPen(map->GetTileAt(i)->GetTilePen());
        tiles.at(i)->setZValue(1);
//        polyVect.at(i)->setFlag(QGraphicsItem::ItemIsSelectable, true);

        tilePixmap.push_back(scene->addPixmap((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setScale(0.64f); //textureScale = 0.32f * drawScale
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }
}

void Renderer::UpdateScene(Map *map, QGraphicsView *view)
{

}

void Renderer::DrawGuiText(Map *map, QVector<QGraphicsTextItem*> tVect, GameView *view)
{
    ////THIS COMMENTED SECTION IS FOR DEBUGGING PURPOSES.
    /// IF YOU NEED IT, COMMENT OUT THE YIELD DISPLAY TEXT
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tVect.push_back(view->addText(QString("%1,%2").arg(map->GetTileAt(i)->GetTileID().column).arg(map->GetTileAt(i)->GetTileID().row)));
        tVect.at(i)->setPos(map->GetTileAt(i)->GetTextCenter());
        tVect.at(i)->setZValue(7);
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

void Renderer::DrawButtons(QWidget *obj, QVector<QGraphicsProxyWidget *> wVect, QGraphicsScene *view)
{
    wVect.push_back(view->addWidget(obj));
}

QString Renderer::SetYieldDisplay(Map *map)
{
    //This is a placeholder, it will need to be re-adjusted once the player class is added.
     return QString("Gold: %1  Production: %2  Food: %3  Science: %4  Culture: %5")
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::GOLD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::PRODUCTION))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::FOOD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::RESEARCH))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::CULTURE));
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
}

void Renderer::DrawTestUnits(Map *map, GameView* view)
{
    QPixmap *TestUnit = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
    unitPixmap.push_back(view->addPixmap(*TestUnit));
    unitPixmap.at(0)->setZValue(4);
    for(int i = 0; i <map->GetBoardSize(); i++)
    {
        if(map->GetTileTypeAt(i) == GRASS || map->GetTileTypeAt(i) == DESERT)
        {
            unitPixmap.at(0)->setPos(map->GetHexTilePoint(i, 0));
            unitPixmap.at(0)->setScale(2.0f);
            map->GetTileAt(i)->ContainsUnit = true;
            break;
        }
    }
}

void Renderer::DrawTestCities(Map *map, GameView *view)
{
    QPixmap *TestCity = new QPixmap("../ProjectHiawatha/Assets/Icons/CityIcon4944.png");
    cityPixmap.push_back(view->addPixmap(*TestCity));
    cityPixmap.at(0)->setZValue(2);
    for(int i = 0; i <map->GetBoardSize(); i++)
    {
        if(i + 90 < map->GetBoardSize())
        {
            int j = i + 90;

            if(map->GetTileTypeAt(j) == GRASS || map->GetTileTypeAt(j) == DESERT)
            {
                if(map->GetTileAt(j)->ContainsUnit == false)
                {
                    cityPixmap.at(0)->setPos(map->GetTileAt(j)->GetTexturePoint());
                    cityPixmap.at(0)->setScale(2.0f);
                    map->GetTileAt(j)->HasCity = true;
                    break;
                }
                if(map->GetTileAt(j)->HasCity)
                {
                    map->GetTileAt(j)->SetControllingCiv(India);
                }
            }
        }
    }
}
