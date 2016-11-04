
#include "renderer.h"
#include "civcolors.h"
#include <QPen>
#include <QDebug>
#include <QProgressBar>

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


}

void Renderer::UpdateScene(Map *map, GameScene *scene, TileData data)
{
    int col = data.column, row = data.row;
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
        if((map->GetTileAt(index)->Selected))
        {
//                map->GetTileAt(index)->Selected = true;
            outlinePen.setColor(Qt::yellow);
            lastIndex = index;
        }
        else
        {
            SetOutlinePen(NO_NATION);

            map->GetTileFromCoord(col, row)->Selected = false;
        }

        map->GetTileFromCoord(col, row)->SetTilePen(outlinePen);
        scene->removeItem(tiles.at(lastIndex));
        tiles.remove(index);
        tiles.insert(index, scene->addPolygon(map->GetTileAt(index)->GetTilePolygon(), outlinePen));
        tiles.at(index)->setPen(map->GetTileAt(index)->GetTilePen());
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
    }
}

void Renderer::UpdateUnits(Map *map, GameScene *view, Unit *unit)
{
    if(view->redrawTile)
    {
        qDebug() << "       Moving Unit" << unit->GetName();

        unitPixmap.at(unit->GetPixmapIndex())->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint());

        qDebug() << "   Unit health:" << unit->GetHealth() << "Unit Max health:" << unit->GetMaxHealth() << " healthBar value:" << unitHealthBars.at(unit->GetHealthBarIndex())->rect().width() << "Health Ratio:" << (static_cast<double>(unit->GetHealth()) / unit->GetMaxHealth());
        if((unit->GetHealth() / unit->GetMaxHealth()) != 1)
        {
            qDebug() << "Updating HealthBars";
            view->removeItem(unitHealthBars.at(unit->GetHealthBarIndex()));

            unitHealthBars.replace(unit->GetHealthBarIndex(), view->addRect(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().x(),
                                                                            map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().y() + unit->GetUnitIcon()->height() + 1,
                                                                            ceil(35 * (static_cast<double>(unit->GetHealth()) / unit->GetMaxHealth())), 5, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green))));
            unitHealthBars.at(unit->GetHealthBarIndex())->setZValue(6);
        }
        else
        {
            unitHealthBars.at(unit->GetHealthBarIndex())->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().x(),
                                                             map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().y() + unit->GetUnitIcon()->height() + 1);
        }
    }
}

void Renderer::UpdateCityBorders(City *city, GameScene *scene, Nation owner)
{
    SetOutlinePen(owner);

    scene->removeItem(cityBorders.at(city->GetCityBordersIndex()));
    cityBorders.replace(city->GetCityBordersIndex(), scene->addPolygon(city->GetCityBorders(), outlinePen));
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

QString Renderer::SetYieldDisplay(Civilization *player)
{
    return QString("        %1(+%2)         %3          %4          %5(+%6)              %7")
                                         .arg(player->GetTotalGold())
                                         .arg(player->getCivYield()->GetYield(Yield::GOLD))
                                         .arg(player->getCivYield()->GetYield(Yield::PRODUCTION))
                                         .arg(player->getCivYield()->GetYield(Yield::FOOD))
                                         .arg(player->GetTotalScience())
                                         .arg(player->getCivYield()->GetYield(Yield::RESEARCH))
                                         .arg(player->getCivYield()->GetYield(Yield::CULTURE));
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

void Renderer::AddUnitHealthBars(Unit *unit, Map *map, GameView *view)
{
    QRect *health = new QRect(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().x(),
                              map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint().y() + unit->GetUnitIcon()->height() + 1,
                              35, 5);

//    unitHealthRects.push_back(health);
    unit->SetHealthBarIndex(unitHealthBars.size());
    unitHealthBars.push_back(view->addRect(health, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green))));
    unitHealthBars.last()->setZValue(6);
}

void Renderer::AddCityHealthBars(City *city, Map *map, GameView *view)
{
    QRect *health = new QRect(city->GetCityTile()->GetItemTexturePoint().x() - 15,
                              city->GetCityTile()->GetItemTexturePoint().y() + 15,
                              65, 5);

    city->SetCityHealthBarIndex(cityHealthBars.size());
    cityHealthBars.push_back(view->addRect(health, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green))));
    cityHealthBars.last()->setZValue(6);
}

void Renderer::DrawGuiImages(QGraphicsScene *scene)
{

}

void Renderer::DrawCityBorders(QVector<City*> cities, GameScene *scene, Nation owner)
{
    SetOutlinePen(owner);

    int i = 0;
    foreach(City* city, cities)
    {
        city->SetCityBordersIndex(i);
        i++;

        cityBorders.push_back(scene->addPolygon(city->GetCityBorders(), outlinePen));
        cityBorders.last()->setPen(outlinePen);
    }
}

void Renderer::LoadCities(QVector<City*> cities, Map *map, GameView *view)
{
    for(int i = 0; i < cities.size(); i++)
    {
        this->AddCity(cities.at(i), map, view);
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

void Renderer::AddCity(City *city, Map *map, GameView *view)
{
    QPixmap *cityImage = new QPixmap("../ProjectHiawatha/Assets/Icons/CityIcon4944.png");
    cityPixmap.push_back(view->addPixmap(*cityImage));
    cityPixmap.last()->setZValue(2);
    cityPixmap.last()->setScale(2.0f);
    cityPixmap.last()->setPos(map->GetTileFromCoord(city->GetCityTile()->GetTileID())->GetTexturePoint());

    this->AddCityHealthBars(city, map, view);
}

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
    unitPixmap.last()->setZValue(2);
    unitPixmap.last()->setScale(1.0f);
    unit->SetPixmapIndex(unitPixmap.size() - 1);
    unitPixmap.last()->setPos(map->GetTileAt(unit->GetTileIndex())->GetItemTexturePoint());

    AddUnitHealthBars(unit, map, view);
}

void Renderer::DrawUnits(QVector<Unit *> units, Map *map, GameView *view)
{
    for(int i = 0; i < units.size(); i++)
    {
        this->AddUnit(units.at(i), map, view);
    }
}

void Renderer::DrawCityHealthBars(QVector<City *> cities, GameView *scene)
{
//    QProgressBar* health;

//    foreach(City *city, cities)
//    {
//        health = new QProgressBar();
//        health->setGeometry(city->GetCityTile()->GetCityLabelPoint().x() - 15,
//                            city->GetCityTile()->GetCityLabelPoint().y() + 15,
//                            75, 10);

//        health->setMaximumWidth(75);
//        health->setMaximumHeight(10);

//        health->setMaximum(100);
//        health->setMinimum(0);

//        health->setValue(100);
//        health->setTextVisible(false);

//        cityHealthBars.push_back(scene->addWidget(health));
//        cityHealthBars.last()->setZValue(6);
//    }
}


