#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent, bool fullscreen) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    if(!fullscreen)
    {
        QWidget::setFixedSize(1200, 900);
    }
    else
    {
        QWidget::setFixedSize(1920, 1080);
    }

    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    renderPlusOne = new QPushButton("Zoom in");
    connect(renderPlusOne, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    renderPlusOne->setShortcut(QKeySequence(Qt::Key_Up));

    renderMinusOne = new QPushButton("Zoom out");
    connect(renderMinusOne, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));
    renderMinusOne->setShortcut(QKeySequence(Qt::Key_Down));

//    updateTimer = new QTimer();
//    updateTimer->setInterval(50);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

//    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    map = new Map();
    map->InitHexMap();
    map->InitTerrain();

    qDebug() << "Done.\nSetting up Scene.";

    /*=================================================================
     * Some of this needs to be moved to Renderer::DrawHexScene()
     * the function parameters also need to be adjusted. -Port
    */

    qDebug() << "Done.\nAdding buttons to screen.";
    exitGame->setGeometry(this->width() - 100, this->height() - 50, 90, 30);
    renderPlusOne->setGeometry(this->width() - 100, this->height() - 100, 90, 30);
    renderMinusOne->setGeometry(this->width() - 100, this->height() - 150, 90, 30);

    qDebug() << "Done. Drawing map.";
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tile.push_back(gameView.addPolygon(map->GetTileAt(i)->GetTilePolygon()));

        // This is going to change. I have an idea of how
        // I may be able to make this work. -Port
        tilePixmap.push_back(gameView.addPixmap((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setScale(0.32f);
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }

    proxy.push_back(gameView.addWidget(exitGame));
    proxy.push_back(gameView.addWidget(renderPlusOne));
    proxy.push_back(gameView.addWidget(renderMinusOne));

    zoomScale = 1;

    qDebug() << "Done.";
    //==================================================================
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Move Event";

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

void GameWindow::wheelEvent(QWheelEvent *e)
{
        qDebug() << "Wheel Event: " << e->delta() << "ZoomScale: " << zoomScale;
        if(e->delta() > 0)
        {
            if(zoomScale < 7)
            {
                gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                gameView.scale(1.2, 1.2);
                zoomScale++;
            }
        }
        else if (e->delta() < 0)
        {
            if(zoomScale > 1)
            {
                gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                gameView.scale(1 / 1.2, 1 / 1.2);
                zoomScale--;
            }
        }
}

void GameWindow::closeGame()
{
    gameView.closeGame();
}

void GameWindow::zoomIn()
{
    gameView.zoomIn();
}

void GameWindow::zoomOut()
{
    gameView.zoomOut();
}

//void GameWindow::updateGameWindow()
//{
//    gameView.scene()->update(gameView.sceneRect());
//}




