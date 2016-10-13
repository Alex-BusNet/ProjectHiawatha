#include "gamemanager.h"
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QThread>
#include <ctime>
#include <algorithm>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    gameView = new GameView(this, fullscreen);
    ac = new AI_Controller();
    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    gameLayout = new QHBoxLayout();
    unitControlButtons = new QVBoxLayout();
    playerControlButtons = new QVBoxLayout();
    cityScreen = new CityScreen();
    cityScreenVisible = false;
    relocateUnit = false;
    turnEnded = false;

    if(!fullscreen)
    {
        this->setFixedSize(1400, 700);
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

    moveUnit = new QPushButton("Move Unit");
    connect(moveUnit, SIGNAL(clicked(bool)), this, SLOT(moveUnitTo()));
//    moveUnit->hide();

    endTurn = new QPushButton("End Turn");
    connect(endTurn, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));
//    endTurn->hide();

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

    unitControlButtons->addSpacing(800);
    unitControlButtons->addWidget(moveUnit);

    gameLayout->addLayout(unitControlButtons);
    gameLayout->addWidget(gameView);

    playerControlButtons->addWidget(exitGame);
    playerControlButtons->addSpacing(700);
    playerControlButtons->addWidget(endTurn);

    gameLayout->addLayout(playerControlButtons);

    vLayout->addLayout(gameLayout);

    hLayout->addWidget(renderPlusOne);
    hLayout->addWidget(renderMinusOne);
    hLayout->addWidget(showDummyCityScreen);

    vLayout->addLayout(hLayout);

//    qDebug() << "Done.\nAdding buttons to screen.";

//    exitGame->setGeometry(this->width() - 200, this->height() + 190, 180, 60);
//    renderPlusOne->setGeometry(gameView->width() - 200, this->height() + 125, 180, 60);
//    renderMinusOne->setGeometry(gameView->width()- 200, this->height() + 60, 180, 60);

//    proxy.push_back(gameView->addWidget(exitGame));
//    proxy.push_back(gameView->addWidget(renderPlusOne));
//    proxy.push_back(gameView->addWidget(renderMinusOne));

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(map, gameView);

    qDebug() << "Initializing Civs";
    ////This is for testing purposes;
    InitCivs(player, 4);

//    qDebug() << "Done.\nDrawing Units.";
//    renderer->DrawTestUnits(map, gameView);

//    renderer->DrawTestCities(map, gameView);

    qDebug() << "   CivList size: " << civList.size();
    qDebug() << "Done.\nDrawing Cities, Borders, and Units.";
    for(int i = 0; i < civList.size(); i++)
    {
        renderer->LoadCities(civList.at(i)->GetCityList(), map, gameView);
        renderer->DrawUnits(civList.at(i)->GetUnitList(), map, gameView);
        renderer->DrawCityBorders(map, civList.at(i)->GetCityList(), gameView->GetScene());
        civList.at(i)->UpdateCivYield();
    }

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
    for(int i = 0; i < 3; i++)
    {
        zoomIn();
    }
    qDebug() << "Done.\nCentering on Player's Capital at:" << civList.at(0)->GetCityAt(0)->GetCityTile()->GetTileIDString();
    currentTurn = 0;
    gameView->centerOn(civList.at(0)->GetCityAt(0)->GetCityTile()->GetCenter());
    this->setLayout(vLayout);
    this->show();

    qDebug() << "Scene size: " << gameView->GetScene()->sceneRect().width() << "x" << gameView->GetScene()->sceneRect().height();

    qDebug() << "Done.";

    StartTurn();
}

void GameManager::InitCivs(Nation player, int numAI)
{
    Civilization* civ = new Civilization(player, false);
    civList.push_back(civ);

    srand(time(0));
    int civNum;

    std::vector<Nation> selNat;

    selNat.push_back(player);

    AI_Strategic* ai;
    qDebug() << "   Player:" << player;
    for(int i = 0; i < numAI; i++)
    {
newCivRand:
        // The number multiplied at the end indicates the
        // max number of civs in the game.
        civNum = rand() % 7;

        if(civNum != player)
        {
            switch (civNum)
            {
            case America:
                civ = new Civilization(America, true);
                ai = new AI_Strategic();
                selNat.push_back(America);
                break;
            case Germany:
                civ = new Civilization(Germany, true);
                ai = new AI_Strategic();
                selNat.push_back(Germany);
                break;
            case India:
                civ = new Civilization(India, true);
                ai = new AI_Strategic();
                selNat.push_back(India);
                break;
            case China:
                civ = new Civilization(China, true);
                ai = new AI_Strategic();
                selNat.push_back(China);
                break;
            case Mongolia:
                civ = new Civilization(Mongolia, true);
                ai = new AI_Strategic();
                selNat.push_back(Mongolia);
                break;
            case Aztec:
                civ = new Civilization(Aztec, true);
                ai = new AI_Strategic();
                selNat.push_back(Aztec);
                break;
            case France:
                civ = new Civilization(France, true);
                ai = new AI_Strategic();
                selNat.push_back(France);
                break;
            default:
                //Always default to Ghandi.
                civ = new Civilization(India, true);
                ai = new AI_Strategic();
                selNat.push_back(India);
                break;
            }

            // look to see if the selected civ has already been selected
            auto found = std::find(std::begin(selNat), std::end(selNat), civNum);

            // If was not found, place in civList vector
            if(found != std::end(selNat))
            {
                qDebug() << "   Civ" << i << ": " << civ->getCiv();
                civList.push_back(civ);
                ac->AddAIToList(ai);
            }
            // Otherwise, delete it and try again.
            else
            {
                delete civ;
                goto newCivRand;
            }

        }
        else
        {
            goto newCivRand;
        }

    }

    qDebug() << "   Spawning Civs";
    map->SpawnCivs(civList);
}

void GameManager::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter paint(this);
    QRect playerInfoRect(0, 0, this->width(), 20);
    paint.fillRect(playerInfoRect, QBrush(Qt::black));
    paint.setPen(Qt::white);
    paint.drawText(playerInfoRect, Qt::AlignVCenter, renderer->SetYieldDisplay(civList.at(0)->getCivYield()));
}

void GameManager::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug() << "Widget mouse release event";
}

void GameManager::TurnController()
{
    if(currentTurn == 0)
    {
        if(turnEnded)
        {
            turnEnded = false;
            EndTurn();
        }
    }
    else
    {
        QFuture<void> future = QtConcurrent::run(this->ac, AI_Controller::turnStarted, currentTurn - 1);
        future.waitForFinished();
        qDebug() << "Finished";
        EndTurn();
    }

}

void GameManager::StartTurn()
{
    qDebug() << "Starting turn for civ" << currentTurn;
    qDebug() << "Current Turn:" << currentTurn << "civList size:" << civList.size();

    currentCityList = civList.at(currentTurn)->GetCityList();
    currentUnitList = civList.at(currentTurn)->GetUnitList();
    currentCiv = civList.at(currentTurn)->GetCivObject();
}

void GameManager::EndTurn()
{
    qDebug() << "Ending Turn";
    currentCiv->SetCityList(currentCityList);
    currentCiv->SetUnitList(currentUnitList);
    qDebug() << "Storing current Civ data" << currentTurn;
    civList.at(currentTurn)->SetCivObj(currentCiv);

    qDebug() << "Updating unit positions";
    foreach(Unit* unit, currentUnitList)
    {
        if(!unit->RequiresOrders)
        {
            uc->MoveUnit(unit, map, gameView->GetScene());
        }
    }

    if(currentTurn == civList.size() - 1)
    {
        qDebug() << "Player's turn next";
        currentTurn = 0;
        turnEnded = false;
        StartTurn();
    }
    else
    {
        qDebug() << "AI turn next";
        currentTurn++;
    }
}


void GameManager::closeGame()
{
    this->close();
}

void GameManager::zoomIn()
{
    gameView->zoomIn();
}

void GameManager::zoomOut()
{
    gameView->zoomOut();
}

void GameManager::showCity()
{
    if(!cityScreenVisible)
    {
        if(cityScreen != NULL)
        {
            delete cityScreen;
        }
        cityScreen = new CityScreen(this);
        //ONLY DID THIS SO I CAN SEE TEXT FOR DEBUGGING PURPOSES
        cityScreen->setAutoFillBackground(true);
        cityScreen->loadBuildings("../ProjectHiawatha/Assets/Buildings/buildings3.txt");
        cityScreen->updateList();
        civList.at(0)->GetCityAt(0)->GetCityTile()->GetCenter();

        gameView->centerOn(civList.at(0)->GetCityAt(0)->GetCityTile()->GetCenter());

//        gameView->setDragMode(QGraphicsView::NoDrag);
        cityScreen->show();
        cityScreenVisible = true;
    }
    else
    {
        cityScreen->hide();
        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        cityScreenVisible = false;
    }
}

void GameManager::updateTiles()
{
    //// The false will need to be changed once the Unit Controller is added.
    gameView->GetScene()->ProcessTile(map, relocateUnit);

    if(gameView->GetScene()->unitMoveOrdered)
    {
        Unit* unitToMove = uc->FindUnitAtTile(gameView->GetScene()->unitSelectedTile, map, currentUnitList);
        qDebug() <<"Finding path";
//        QFuture<void> future = QtConcurrent::run(this->uc, UnitController::FindPath, gameView->GetScene()->unitSelectedTile, gameView->GetScene()->unitTargetTile, map, gameView->GetScene(), unitToMove);
//        future.waitForFinished();
        uc->FindPath(gameView->GetScene()->unitSelectedTile, gameView->GetScene()->unitTargetTile, map, gameView->GetScene(), unitToMove);
        relocateUnit = false;
        gameView->GetScene()->unitMoveOrdered = false;
        qDebug() << "Done";
    }

    TurnController();

    if(gameView->GetScene()->redrawTile)
    {
        renderer->UpdateScene(map, gameView->GetScene());
    }

}

void GameManager::moveUnitTo()
{
    relocateUnit = true;
}

void GameManager::nextTurn()
{
    // Ends the players turn
    turnEnded = true;
    EndTurn();
}




