
#include "renderer.h"
#include "civcolors.h"
#include <QPen>
#include <QDebug>

#ifndef MAXSIZE
#define MAXSIZE 128
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

Renderer::Renderer(int mapSizeX)
{
    this->mapSizeX = mapSizeX;
    cc = new CivColors();
}

void Renderer::DrawHexScene(Map *map, GameView *scene)
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        map->GetTileAt(i)->SetTilePen(outlinePen);
        tiles.push_back(scene->addPolygon(map->GetTileAt(i)->GetTilePolygon()));
        tiles.at(i)->setPen(map->GetTileAt(i)->GetTilePen());
        tiles.at(i)->setZValue(1);
        tiles.at(i)->setOpacity(50);
        // Create new vector that stores the oddly-shaped border polygon for each civ (this may be one vector per civ)
        // Set pen of vector to proper civ color (may require adjustment of if/else condition above

        tilePixmap.push_back(scene->addPixmap((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setScale(0.64f); //textureScale = 0.32f * drawScale
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }

    // Add map edge generation code here
}

void Renderer::UpdateScene(Map *map, GameScene *scene)
{
    if(scene->redrawTile)
    {
        int col = scene->column, row = scene->row;
        // 20 is for duel sized maps (the default). That value will need to be adjusted later.
        int index = (col / 2) + (mapSizeX * row);
        static int lastIndex;

        // if the tile at the coordinates contains a unit
        //      if the tile contains a unit, check if it is the active selected tile
        //          if the tile is the active selected tile, reset the tile border to its controlling civ's color
        //              clear the tile's selected flag and the global isTileSelected flag.
        //          else, set the tiles border color to yellow
        //
        //      find the index of the tile in the render map vector (tiles)
        //      replace the existing tile with the updated tile (including the updated boarder color)
        //      set the render map's tile pen at the replaced index, to the border pen of the new tile.
        // else if the tile at the coordinates contains a city
        //      reset the existing selected tile, if there is one
        //      clear the global isTileSelected flag
        //      open the city screen.
        // else if the tile at the coordinates is empty
        //      reset the existing selected tile, if there is one.
        //      clear the global isTileSelected flag.

        if(map->GetTileAt(index)->ContainsUnit)
        {
            if(!(map->GetTileAt(index)->Selected))
            {
                map->GetTileAt(index)->Selected = true;
                outlinePen.setColor(Qt::yellow);
                lastIndex = index;
            }
            else
            {
                SetOutlinePen(NO_NATION);

                scene->isTileSelected = false;
                map->GetTileFromCoord(col, row)->Selected = false;
            }

            map->GetTileFromCoord(col, row)->SetTilePen(outlinePen);
            scene->removeItem(tiles.at(lastIndex));
            tiles.remove(index);
            tiles.insert(index, scene->addPolygon(map->GetTileAt(index)->GetTilePolygon(), outlinePen));
            tiles.at(index)->setPen(map->GetTileAt(index)->GetTilePen());

            scene->redrawTile = false;
        }
        else if(map->GetTileAt(index)->HasCity)
        {
            SetOutlinePen(NO_NATION);
            map->GetTileAt(lastIndex)->SetTilePen(outlinePen);
            map->GetTileAt(lastIndex)->Selected = false;

            scene->removeItem(tiles.at(lastIndex));
            tiles.remove(lastIndex);
            tiles.insert(lastIndex, scene->addPolygon(map->GetTileAt(index)->GetTilePolygon()));
            tiles.at(lastIndex)->setPen(map->GetTileAt(lastIndex)->GetTilePen());

            map->GetTileAt(index)->GetGoverningCity();
            scene->isTileSelected = false;
            scene->redrawTile = false;
        }
        else
        {
            SetOutlinePen(NO_NATION);
            map->GetTileAt(lastIndex)->SetTilePen(outlinePen);
            map->GetTileAt(lastIndex)->Selected = false;

            scene->removeItem(tiles.at(lastIndex));
            tiles.remove(lastIndex);
            tiles.insert(lastIndex, scene->addPolygon(map->GetTileAt(index)->GetTilePolygon()));
            tiles.at(lastIndex)->setPen(map->GetTileAt(lastIndex)->GetTilePen());

            scene->isTileSelected = false;
            scene->redrawTile = false;
        }
    }
}

void Renderer::UpdateUnits(Map *map, GameView *view, Unit *unit)
{
    QPixmap *unitImage;

    if(view->GetScene()->redrawTile)
    {
        qDebug() << "       Moving Unit";
        view->GetScene()->removeItem(unitPixmap.at(unit->GetPixmapIndex()));
        unitImage = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
        unitPixmap.replace(unit->GetPixmapIndex(), view->addPixmap(*unitImage));
        unitPixmap.at(unit->GetPixmapIndex())->setScale(2.0f);
        unitPixmap.at(unit->GetPixmapIndex())->setZValue(2);
        unitPixmap.at(unit->GetPixmapIndex())->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint());

    }
}

void Renderer::DrawGuiText(Map *map, QVector<QGraphicsTextItem*> tVect, GameView *view)
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tVect.push_back(view->addText(QString("%1,%2\n%3").arg(map->GetTileAt(i)->GetTileID().column).arg(map->GetTileAt(i)->GetTileID().row).arg(i)));
        tVect.at(i)->setPos(map->GetTileAt(i)->GetTextCenter());
        tVect.at(i)->setZValue(7);
        if(map->GetTileAt(i)->GetTileType() == ICE || map->GetTileAt(i)->ContainsUnit || map->GetTileAt(i)->HasCity)
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

void Renderer::DrawUnitPath(GameScene *scene, Unit *unit)
{
    if(!unit->isPathEmpty())
    {
        int tileCount = 0;
        QPoint lastTile;

        foreach(Tile *tile, unit->GetPath())
        {
            if(tileCount == 0)
            {
                lastTile = tile->GetCenter();
                tileCount++;
            }
            else
            {
                QLine travelLine(lastTile, tile->GetCenter());
                unitGraphicsPath.push_back(scene->addLine(travelLine, QPen(QColor(Qt::red))));
                lastTile = tile->GetCenter();
            }
        }
    }
}

void Renderer::ClearUnitPath(GameScene *scene)
{
    if(!unitGraphicsPath.isEmpty())
    {
        foreach(QGraphicsLineItem *line, unitGraphicsPath)
        {
            scene->removeItem(line);
        }

        unitGraphicsPath.clear();
    }
}

////This is for development and debug purposes only
QString Renderer::SetYieldDisplay(Map *map)
{
    //This is a placeholder, it will need to be re-adjusted once the player class is added.
     return QString("Gold: %1  Production: %2  Food: %3  Science: %4  Culture: %5")
                                          .arg(map->GetTileAt(0)->GetYield()->GetYield(Yield::GOLD))
                                          .arg(map->GetTileAt(0)->GetYield()->GetYield(Yield::PRODUCTION))
                                          .arg(map->GetTileAt(0)->GetYield()->GetYield(Yield::FOOD))
                                          .arg(map->GetTileAt(0)->GetYield()->GetYield(Yield::RESEARCH))
                                          .arg(map->GetTileAt(0)->GetYield()->GetYield(Yield::CULTURE));
}

QString Renderer::SetYieldDisplay(Yield *yield)
{
    return QString("Gold: %1  Production: %2  Food: %3  Science: %4  Culture: %5")
                                         .arg(yield->GetYield(Yield::GOLD))
                                         .arg(yield->GetYield(Yield::PRODUCTION))
                                         .arg(yield->GetYield(Yield::FOOD))
                                         .arg(yield->GetYield(Yield::RESEARCH))
                                         .arg(yield->GetYield(Yield::CULTURE));
}

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
    default:
        outlinePen.setColor(cc->NO_NATION_PRIMARY);
        break;
    }

    outlinePen.setWidth(5);
}

void Renderer::DrawGuiImages(QGraphicsScene *scene)
{

}

void Renderer::DrawCityBorders(Map *map, QVector<City*> cities, GameScene *scene)
{
    for(int i = 0; i < cities.size(); i++)
    {
        foreach(Tile* tile, cities.at(i)->GetControlledTiles())
        {
            SetOutlinePen(tile->GetControllingCiv());
            tile->SetTilePen(outlinePen);

            cityBorders.push_back(scene->addPolygon(tile->GetTilePolygon()));
            cityBorders.last()->setPen(tile->GetTilePen());
        }
    }
}

void Renderer::DrawCityBorders(QVector<City*> cities, GameScene *scene, Nation owner)
{
    SetOutlinePen(owner);

    for(int i = 0; i < cities.size(); i++)
    {
        foreach(City* city, cities)
        {
            cityBorders.push_back(scene->addPolygon(city->GetCityBorders(), outlinePen));
            cityBorders.last()->setPen(outlinePen);
        }
    }
}

void Renderer::LoadCities(QVector<City*> cities, Map *map, GameView *view)
{
    QPixmap *cityImage;

    for(int i = 0; i < cities.size(); i++)
    {
        cityImage = new QPixmap("../ProjectHiawatha/Assets/Icons/CityIcon4944.png");
        cityPixmap.push_back(view->addPixmap(*cityImage));
        cityPixmap.last()->setZValue(2);
        cityPixmap.last()->setScale(2.0f);
        cityPixmap.last()->setPos(map->GetTileFromCoord(cities.at(i)->GetCityTile()->GetTileID())->GetItemTexturePoint());
    }
}

void Renderer::AddCityLabel(QString name, Civilization* civ, GameView *view)
{
    QLabel* label;
    foreach(City* city, civ->GetCityList())
    {
        label  = new QLabel(QString(" %1 ").arg(name));
        label->setAutoFillBackground(true);
        cityLabels.push_back(view->addWidget(label));
        cityLabels.last()->setPos(city->GetCityTile()->GetCityLabelPoint());
    }
}

void Renderer::DrawUnits(QVector<Unit *> units, Map *map, GameView *view)
{
    QPixmap *unitImage;

    for(int i = 0; i < units.size(); i++)
    {
        unitImage = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
        unitPixmap.push_back(view->addPixmap(*unitImage));
        unitPixmap.last()->setZValue(2);
        unitPixmap.last()->setScale(2.0f);
        // All unit images are stored in the unitPixmap vector.
        units.at(i)->SetPixmapIndex(unitPixmap.size() - 1);
        unitPixmap.last()->setPos(map->GetTileAt(units.at(i)->GetTileIndex())->GetItemTexturePoint());
    }
}

////For Debug uses only
void Renderer::DrawDebugCityBorders(Map *map, GameScene *scene)
{
    int col = scene->column, row = scene->row;
    // 20 is for duel sized maps (the default). That value will need to be adjusted later.
    int index = (col / 2) + (20 * row);
    if(map->GetTileAt(index)->HasCity)
    {
        SetOutlinePen(map->GetTileAt(index)->GetControllingCiv());
        map->GetTileAt(index)->SetTilePen(outlinePen);
        map->GetTileAt(index)->Selected = false;

        cityBorders.push_back(scene->addPolygon(map->GetTileAt(index)->GetTilePolygon()));
        cityBorders.last()->setPen(map->GetTileAt(index)->GetTilePen());
    }

    scene->redrawTile = false;
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
            unitPixmap.at(0)->setPos(map->GetTileAt(i)->GetItemTexturePoint());
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
            int j = i + 100;

            if(map->GetTileTypeAt(j) == GRASS || map->GetTileTypeAt(j) == DESERT)
            {
                if(map->GetTileAt(j)->ContainsUnit == false)
                {
                    qDebug() << "Configuring city for tile" << j;
                    cityPixmap.at(0)->setPos(map->GetTileAt(j)->GetTexturePoint());
                    cityPixmap.at(0)->setScale(2.0f);
                    map->GetTileAt(j)->HasCity = true;
                    map->GetTileAt(j)->SetControllingCiv(India);

                    view->GetScene()->column = map->GetTileAt(j)->GetTileID().column;
                    view->GetScene()->row = map->GetTileAt(j)->GetTileID().row;
                    break;
                }
            }
        }
    }
}
