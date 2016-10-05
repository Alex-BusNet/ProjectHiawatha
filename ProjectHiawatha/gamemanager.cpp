#include "gamemanager.h"
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QThread>

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    gameView = new GameView(this, fullscreen);

    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    gameLayout = new QHBoxLayout();
    cityScreen = new CityScreen();
    cityScreenVisible = false;

    if(!fullscreen)
    {
        this->setFixedSize(1200, 700);
    }
    else
    {
        this->setWindowState(Qt::WindowFullScreen);
    }

    gameView->ConfigureGraphicsView();

    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    renderPlusOne = new QPushButton("Zoom in");
    connect(renderPlusOne, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    renderPlusOne->setShortcut(QKeySequence(Qt::Key_Up));

    renderMinusOne = new QPushButton("Zoom out");
    connect(renderMinusOne, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));
    renderMinusOne->setShortcut(QKeySequence(Qt::Key_Down));

    showDummyCityScreen = new QPushButton("Show Dummy City");
    connect(showDummyCityScreen, SIGNAL(clicked(bool)), this, SLOT(showCity()));

    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTiles()));
    updateTimer->start();

    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map.";
    map = new Map(mapSizeX, mapSizeY);
    map->InitHexMap();

    qDebug() << "Done.\nSetting up Scene.";

    vLayout->setMargin(0);
    vLayout->addSpacing(20);
    vLayout->addWidget(gameView);

    hLayout->addWidget(exitGame);
    hLayout->addWidget(renderPlusOne);
    hLayout->addWidget(renderMinusOne);
    hLayout->addWidget(showDummyCityScreen);

    vLayout->addLayout(hLayout);

    qDebug() << "Done.\nAdding buttons to screen.";

//    exitGame->setGeometry(this->width() - 200, this->height() + 190, 180, 60);
//    renderPlusOne->setGeometry(gameView->width() - 200, this->height() + 125, 180, 60);
//    renderMinusOne->setGeometry(gameView->width()- 200, this->height() + 60, 180, 60);

//    proxy.push_back(gameView->addWidget(exitGame));
//    proxy.push_back(gameView->addWidget(renderPlusOne));
//    proxy.push_back(gameView->addWidget(renderMinusOne));

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(map, gameView);

    qDebug() << "Done.\nDrawing Units.";
    renderer->DrawTestUnits(map, gameView);

    qDebug() << "Done.\nDrawing Cities.";
    renderer->DrawTestCities(map, gameView);
    renderer->DrawDebugCityBorders(map, gameView->GetScene());

//    renderer->DrawGuiImages(game);

//    for(int i = 0; i < proxy.size(); i++)
//    {
//        proxy.at(i)->setZValue(7);
//    }

//    for(int i = 0; i < guiRects.size(); i++)
//    {
//        guiRects.at(i)->setZValue(6);
//    }

    renderer->DrawGuiText(map, stringData, gameView);
    zoomScale = 1;

    gameView->SetGameMap(map);
    this->setLayout(vLayout);
    this->show();

    qDebug() << "Done.";
}

void GameManager::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter paint(this);
    QRect playerInfoRect(0, 0, this->width(), 20);
    paint.fillRect(playerInfoRect, QBrush(Qt::black));
    paint.setPen(Qt::white);
    paint.drawText(playerInfoRect, Qt::AlignVCenter, renderer->SetYieldDisplay(map));

//    if(gameView->GetScene()->isTileSelected)
//    {
//        gameView->GetScene()->drawForeground(&paint, gameView->sceneRect());
//    }
}

void GameManager::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug() << "Widget mouse release event";
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

void GameManager::showCity()
{

    static QGraphicsRectItem *cityScreenRect;
    static QRect* cityRect;

    if(!cityScreenVisible)
    {
        if(cityScreen != NULL)
        {
            delete cityScreen;
        }

        cityScreen = new CityScreen(this);
        cityRect = new QRect(cityScreen->pos().x(), cityScreen->pos().y(), cityScreen->width(), cityScreen->height());
        for(int i = 0; i < map->GetBoardSize(); i++)
        {
            if(map->GetTileAt(i)->HasCity)
            {
                gameView->centerOn(map->GetTileAt(i)->GetCenter());
            }
        }
        gameView->setDragMode(QGraphicsView::NoDrag);
        QPen cityPen(QColor(Qt::red));
        cityPen.setWidth(4);
        cityScreenRect = gameView->addRect(cityRect, cityPen);
        cityScreen->show();
        cityScreenVisible = true;
    }
    else
    {
        cityScreen->hide();
        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        gameView->removeRect(cityScreenRect);
        cityScreenVisible = false;
    }
}

void GameManager::updateTiles()
{
    //// The false will need to be changed once the Unit Controller is added.
    gameView->GetScene()->ProcessTile(map, false);

    if(gameView->GetScene()->redrawTile)
    {
        renderer->UpdateScene(map, gameView->GetScene());
    }
}




