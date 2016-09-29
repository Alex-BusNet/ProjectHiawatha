#include "gamemanager.h"
#include <QDebug>
#include <QDialog>

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    if(!fullscreen)
    {
        QWidget::setFixedSize(900, 600);
    }
    else
    {
        QWidget::setFixedSize(parent->width(), parent->height());
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
//    updateTimer->setInterval(1);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

//    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    map = new Map(mapSizeX, mapSizeY);
    map->InitHexMap();

    qDebug() << "Done.\nSetting up Scene.";

    /*=================================================================
     * Some of this needs to be moved to Renderer::DrawHexScene()
     * the function parameters also need to be adjusted. -Port
    */

    qDebug() << "Done.\nAdding buttons to screen.";
    exitGame->setGeometry((gameView.viewport()->width() * 2) - 150, (gameView.viewport()->height() * 2) - 100, 90, 30);
    renderPlusOne->setGeometry((gameView.viewport()->width() * 2) - 150, (gameView.viewport()->height() * 2) - 150, 90, 30);
    renderMinusOne->setGeometry((gameView.viewport()->width() * 2) - 150, (gameView.viewport()->height() * 2) - 200, 90, 30);
    YieldDisplay = new QRect(0,0, 500, 20);

    proxy.push_back(gameView.addWidget(exitGame));
    proxy.push_back(gameView.addWidget(renderPlusOne));
    proxy.push_back(gameView.addWidget(renderMinusOne));

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(map, tile, tilePixmap, &gameView);

    renderer->DrawTestUnits(map, unitPixmap, &gameView);

    renderer->DrawTestCities(map, cityPixmap, &gameView);

//    renderer->DrawGuiImages(game);
    guiRects.push_back(gameView.addRect(YieldDisplay, gmPen, gmBrush));

    for(int i = 0; i < proxy.size(); i++)
    {
        proxy.at(i)->setZValue(7);
    }

    for(int i = 0; i < guiRects.size(); i++)
    {
        guiRects.at(i)->setZValue(6);
    }

    renderer->DrawGuiText(map, stringData, &gameView);
    zoomScale = 1;

    qDebug() << "Done.";
    //==================================================================
}

void GameManager::mouseMoveEvent(QMouseEvent *event)
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

void GameManager::closeGame()
{
    gameView.closeGame();
}

void GameManager::zoomIn()
{
    qDebug() << "Widget called ZoomIn()";
    gameView.zoomIn();
}

void GameManager::zoomOut()
{
    qDebug() << "Widget called ZoomOut()";
    gameView.zoomOut();
}

void GameManager::updateGameWindow()
{
    renderer->UpdateScene(&gameView);
}



