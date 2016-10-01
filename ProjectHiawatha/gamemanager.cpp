#include "gamemanager.h"
#include <QDebug>
#include <QDialog>
#include <QThread>

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

//    this->setWindowState(Qt::WindowFullScreen);
    gameView = new GameView(this, fullscreen);
    gameView->setScene(gameView->GetScene());

    QVBoxLayout *vlayout = new QVBoxLayout();
    YieldDisplay = new QRect(0,0, this->width(), 20);

    if(!fullscreen)
    {
        this->setFixedSize(1200, 700);
    }
    else
    {
        this->setWindowState(Qt::WindowFullScreen);
    }

    vlayout->addWidget(gameView);

    gameView->ConfigureGraphicsView();
    gameView->setDragMode(QGraphicsView::ScrollHandDrag);

    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    renderPlusOne = new QPushButton("Zoom in");
    connect(renderPlusOne, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    renderPlusOne->setShortcut(QKeySequence(Qt::Key_Up));

    renderMinusOne = new QPushButton("Zoom out");
    connect(renderMinusOne, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));
    renderMinusOne->setShortcut(QKeySequence(Qt::Key_Down));

    updateTimer = new QTimer();
    updateTimer->setInterval(1);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
    updateTimer->start();

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
    QHBoxLayout *hLayout = new QHBoxLayout();
    exitGame->setGeometry(this->width() - 200, this->height() + 190, 180, 60);
    renderPlusOne->setGeometry(gameView->width() - 200, this->height() + 125, 180, 60);
    renderMinusOne->setGeometry(gameView->width()- 200, this->height() + 60, 180, 60);

    hLayout->addWidget(exitGame);
    hLayout->addWidget(renderPlusOne);
    hLayout->addWidget(renderMinusOne);

    vlayout->addLayout(hLayout);

    qDebug() << "gameView width: " << gameView->width() << "gameView height: " << gameView->height();

//    proxy.push_back(gameView->addWidget(exitGame));
//    proxy.push_back(gameView->addWidget(renderPlusOne));
//    proxy.push_back(gameView->addWidget(renderMinusOne));

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(map, tile, tilePixmap, gameView);

    qDebug() << "Done.\nDrawing Units.";
    renderer->DrawTestUnits(map, unitPixmap, gameView);

    qDebug() << "Done.\nDrawing Cities.";
    renderer->DrawTestCities(map, cityPixmap, gameView);

//    renderer->DrawGuiImages(game);
    guiRects.push_back(gameView->addRect(YieldDisplay, gmPen, gmBrush));

    for(int i = 0; i < proxy.size(); i++)
    {
        proxy.at(i)->setZValue(7);
    }

    for(int i = 0; i < guiRects.size(); i++)
    {
        guiRects.at(i)->setZValue(6);
    }

    renderer->DrawGuiText(map, stringData, gameView);
    zoomScale = 1;

    this->setLayout(vlayout);
//    this->setFixedSize(1200, 800);
    this->show();

    qDebug() << "Done.";
    //==================================================================
}

void GameManager::closeGame()
{
    this->close();
}

void GameManager::zoomIn()
{
    qDebug() << "Widget called ZoomIn()";
    gameView->zoomIn();
}

void GameManager::zoomOut()
{
    qDebug() << "Widget called ZoomOut()";
    gameView->zoomOut();
}

void GameManager::updateGameWindow()
{
    gameView->update();
    this->update();
}




