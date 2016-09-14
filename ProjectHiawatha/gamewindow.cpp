#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    QWidget::setFixedSize(1200, 900);

//    updateTimer = new QTimer();
//    updateTimer->setInterval(500);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    map = new Map();
    map->InitHexMap();

    qDebug() << "Done." << endl;

    /*=================================================================
     * Some of this needs to be moved to Renderer::DrawHexScene()
     * the function parameters also need to be adjusted. -Port
    */
    game = new QGraphicsScene(this);
    gameView.setScene(game);

    gameView.setMinimumWidth(1200);
    gameView.setMinimumHeight(900);

    gameView.setMaximumHeight(900);
    gameView.setMaximumWidth(1200);

    gameView.show();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tile = game->addPolygon(map->GetTileAt(i)->GetTilePolygon());

        // This is going to change. I have an idea of how
        // I may be able to make this work. -Port
        tilePixmap = game->addPixmap(map->GetTileAt(i)->GetTileTexture());
        tilePixmap->setScale(0.32f);
        tilePixmap->setPos(0, 1.3);
    }
    //==================================================================
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->pos().x() > 1150)
    {
        //Scroll to the right (continuous)
    }
    else if(event->pos().y() > 850)
    {
        //Scroll down to bottom of map
    }
    else if(event->pos().x() < 50)
    {
        //Scroll to the left(continuous)
    }
    else if(event->pos().y() < 50)
    {
        //Scroll up to top of map
    }
}




