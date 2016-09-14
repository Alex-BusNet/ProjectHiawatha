
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

void Renderer::DrawMap(QPainter &painter)
{
    QPen pen;
    QRect rect;

    pen.setColor(Qt::black);
    pen.setWidth(3);

    painter.setPen(pen);

    for(int i = 0; i < board.size(); i++)
    {
        //Build a new QRect for each Tile.
        rect = QRect((*(board.at(i))).GetPosX(), (*(board.at(i))).GetPosY(), (*(board.at(i))).GetSizeX(), (*(board.at(i))).GetSizeY());

        painter.drawRect(rect);

        if((*(board.at(i))).GetTileType() == GRASS)
        {
            painter.fillRect(rect, Qt::green);
        }
        else if((*(board.at(i))).GetTileType() == WATER)
        {
            painter.fillRect(rect, Qt::blue);
        }
    }
}

void Renderer::DrawHex(QPainter &painter)
{
    QPen pen;
    QPainterPath path;

    pen.setColor(Qt::black);
    pen.setWidth(3);

    painter.setPen(pen);

    for(int i = 0; i < board.size(); i++)
    {
//        path.addPolygon((*(board.at(i))).GetTilePolygon());
//        path.closeSubpath();

//        if((*(board.at(i))).GetTileType() == GRASS)
//        {
//            painter.fillPath(path, Qt::green);
//        }
//        else if((*(board.at(i))).GetTileType() == WATER)
//        {
//            painter.fillPath(path, Qt::blue);
//        }

        painter.drawPixmap((*(board.at(i))).GetHexPosX() - 11, (*(board.at(i))).GetHexPosY() + 2, 47, 41, (*(board.at(i))).GetTileTexture());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        painter.drawPolyline((*(board.at(i))).GetHexPoints(), 7);

        pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawPoints((*(board.at(i))).GetHexPoints(), 7);
        painter.drawPoint((*(board.at(i))).GetCenter());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        painter.drawText((*(board.at(i))).GetTextCenter(), QString("%1, %2").arg((*(board.at(i))).GetTileID().column).arg((*(board.at(i))).GetTileID().row));
    }
}

void Renderer::DrawHexScene(QGraphicsScene *scene)
{
    for(int i = 0; i < board.size(); i++)
    {
         scene->addPolygon((*(board.at(i))).GetTilePolygon());
    }
}

Tile * Renderer::GetTileAt(int index)
{
    return this->board.at(index);
}

int Renderer::GetBoardSize()
{
    return this->board.size();
}

void Renderer::InitMap()
{
    qDebug() << "InitMap() called";
    Tile * tile;
    int posX = (1200 / (mapSizeX / 3));
    int posY = 0;

    qDebug() << "MapSizeX: " << mapSizeX << " MapSizeY: " << mapSizeY << endl;

    //Each vector is a row of tiles;
    //Therefore, there will be 16 vectors containing 48 tiles each.
    for(int i = 0; i < mapSizeY; i++)
    {
        for(int j = 0; j < mapSizeX; j++)
        {
            tile = new Tile(posX, posY);

            if(j > 3 && (((i > 3) && (i < 23)) || ((i > 26) && (i < 39))))
            {
                if(j < 23)
                {
                    tile->SetTileType(GRASS);
                }
                else if(j > 26)
                {
                    if(j < 44)
                    {
                        tile->SetTileType(GRASS);
                    }
                    else
                    {
                        tile->SetTileType(WATER);
                    }
                }
                else
                {
                    tile->SetTileType(WATER);
                }
            }
            else
            {
                tile->SetTileType(WATER);
            }

            board.push_back(tile);

            posX += 21;
        }

        posX = (1200 / (mapSizeX / 3));
        posY += 21;

    }
}

void Renderer::InitHexMap()
{
    qDebug() << "InitHexMap() called";

    Tile *tile;
    float posX = 12;
    float posY = 0;
    float rowOffset = 37;
    bool odd = false;
    int column = 0, row = 0;

    qDebug() << "MapSizeX: " << mapSizeX << " MapSizeY: " << mapSizeY;

    for(int i = 0; i < (mapSizeY); i++)
    {
        if((i % 2) != 0)
        {
            odd = true;
            column = 1;
        }
        else
        {
            odd = false;
            column = 0;
        }

        for(int j = 0; j < (mapSizeX / 2); j++)
        {
            tile = new Tile(posX, posY);

            tile->SetTileTexture(GRASS);

            tile->SetTileID(row, column, tile);

            board.push_back(tile);

            posX += 74;
            column += 2;
        }

        if(!odd)
        {
            posX = 12 + rowOffset;
        }
        else
        {
            posX = 12;
        }
        row++;
        posY += 22;

    }
}
