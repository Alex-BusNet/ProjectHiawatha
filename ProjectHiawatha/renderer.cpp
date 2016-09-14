
#include "renderer.h"
#include <QPen>
#include <QDebug>

#ifndef MAXSIZE
#define MAXSIZE 100
#endif

Renderer::Renderer()
{
    //These will need to be changed once different map sizes are added.
    //These values represent the number of tiles on the map
    // not the number of tiles on screen.
    mapSizeX = 30; //max = 32
    mapSizeY = 39;  //max = 40
}

void Renderer::DrawMap(Map *map, QPainter &painter)
{
    QPen pen;
    QRect rect;

    pen.setColor(Qt::black);
    pen.setWidth(3);

    painter.setPen(pen);

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        //Build a new QRect for each Tile.
        rect = QRect(map->GetTileAt(i)->GetPosX(), map->GetTileAt(i)->GetPosY(), map->GetTileAt(i)->GetSizeX(), map->GetTileAt(i)->GetSizeY());

        painter.drawRect(rect);

        if(map->GetTileAt(i)->GetTileType() == GRASS)
        {
            painter.fillRect(rect, Qt::green);
        }
        else if(map->GetTileAt(i)->GetTileType() == WATER)
        {
            painter.fillRect(rect, Qt::blue);
        }
    }
}

void Renderer::DrawHex(Map *map, QPainter &painter)
{
    QPen pen;
    QPainterPath path;

    pen.setColor(Qt::black);
    pen.setWidth(3);

    painter.setPen(pen);

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        painter.drawPixmap(map->GetTileAt(i)->GetHexPosX() - 11, map->GetTileAt(i)->GetHexPosY() + 2, 47, 41, map->GetTileAt(i)->GetTileTexture());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        painter.drawPolyline(map->GetTileAt(i)->GetHexPoints(), 7);

        pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawPoints(map->GetTileAt(i)->GetHexPoints(), 7);
        painter.drawPoint(map->GetTileAt(i)->GetCenter());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        painter.drawText(map->GetTileAt(i)->GetTextCenter(), QString("%1, %2").arg(map->GetTileAt(i)->GetTileID().column).arg(map->GetTileAt(i)->GetTileID().row));
    }
}

void Renderer::DrawHexScene(Map *map, QGraphicsView *view, QGraphicsScene *scene)
{

}

void Renderer::InitMap()
{
//    qDebug() << "InitMap() called";
//    Tile * tile;
//    int posX = (1200 / (mapSizeX / 3));
//    int posY = 0;

//    qDebug() << "MapSizeX: " << mapSizeX << " MapSizeY: " << mapSizeY << endl;

//    //Each vector is a row of tiles;
//    //Therefore, there will be 16 vectors containing 48 tiles each.
//    for(int i = 0; i < mapSizeY; i++)
//    {
//        for(int j = 0; j < mapSizeX; j++)
//        {
//            tile = new Tile(posX, posY);

//            if(j > 3 && (((i > 3) && (i < 23)) || ((i > 26) && (i < 39))))
//            {
//                if(j < 23)
//                {
//                    tile->SetTileType(GRASS);
//                }
//                else if(j > 26)
//                {
//                    if(j < 44)
//                    {
//                        tile->SetTileType(GRASS);
//                    }
//                    else
//                    {
//                        tile->SetTileType(WATER);
//                    }
//                }
//                else
//                {
//                    tile->SetTileType(WATER);
//                }
//            }
//            else
//            {
//                tile->SetTileType(WATER);
//            }

//            board.push_back(tile);

//            posX += 21;
//        }

//        posX = (1200 / (mapSizeX / 3));
//        posY += 21;

//    }
}
