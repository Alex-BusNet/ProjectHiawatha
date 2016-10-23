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

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player, int numAI) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    gameView = new GameView(this, fullscreen);
    ac = new AI_Controller();

    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    gameLayout = new QHBoxLayout();
    unitControlButtons = new QVBoxLayout();
    playerControlButtons = new QVBoxLayout();
    Yieldinfo = new QHBoxLayout();

    cityScreen = new CityScreen(this);
    techTree = new TechTree(this);

    cityScreenVisible = false;
    techTreeVisible = false;
    relocateUnit = false;
    turnEnded = false;

    playerCiv = player;

    if(!fullscreen)
    {
        this->setFixedSize(1400, 700);
    }
    else
    {
        this->setWindowState(Qt::WindowFullScreen);
    }

    gameView->ConfigureGraphicsView();

    this->InitButtons();

    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTiles()));
    updateTimer->start();

//    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer(mapSizeX);

    qDebug() << "Done.\nInitializing Map.";
    map = new Map(mapSizeX, mapSizeY);
    map->InitHexMap();

    qDebug() << "Done.\nSetting up Scene.";

    this->InitLayouts();

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
    ////The 1 is for testing purposes;
    /// Change to numAI when done.
    InitCivs(player, 1);

    qDebug() << "   CivList size: " << civList.size();
    qDebug() << "Done.\nDrawing Cities, Borders, and Units.";
    for(int i = 0; i < civList.size(); i++)
    {
        renderer->LoadCities(civList.at(i)->GetCityList(), map, gameView);

        renderer->AddCityLabel(civList.at(i)->GetNextCityName(), civList.at(i), gameView);

        renderer->DrawUnits(civList.at(i)->GetUnitList(), map, gameView);
        renderer->DrawCityBorders(civList.at(i)->GetCityList(), gameView->GetScene(), civList.at(i)->getCiv());
        civList.at(i)->UpdateCivYield();
    }

    InitYieldDisplay();

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
    gameTurn = 0;
    //Start at 4000 BC. The game increments the turn to 1, and subsequently the in game year by 40 years, upon game start.
    year = -4040;

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
    civ->loadTechs("../ProjectHiawatha/Assets/Techs/Technology.txt");
    civ->setCurrentTech(civ->GetTechList().at(0));
    civ->setNextTech(civ->GetTechList().at(1));
    civ->setTechIndex(0);
    QString str = "../ProjectHiawatha/Assets/CityLists/";
    QString str2;
    switch (player)
    {
    case America:
        str2 = "america.txt";
        break;
    case Germany:
        str2 = "germany.txt";
        break;
    case India:
        str2 = "india.txt";
        break;
    case China:
        str2 = "china.txt";
        break;
    case Mongolia:
        str2 = "mongolia.txt";
        break;
    case Aztec:
        str2 = "aztec.txt";
        break;
    case France:
        str2 = "france.txt";
        break;
    default:
        str2 = "india.txt";
        break;
    }
    str = str + str2;
    qDebug()<<str;
    civ->loadCities(str);
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
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/america.txt");
                //ai = new AI_Strategic();
                selNat.push_back(America);
                break;
            case Germany:
                civ = new Civilization(Germany, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/germany.txt");
                //ai = new AI_Strategic();
                selNat.push_back(Germany);
                break;
            case India:
                civ = new Civilization(India, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/india.txt");
                //ai = new AI_Strategic();
                selNat.push_back(India);
                break;
            case China:
                civ = new Civilization(China, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/china.txt");
               // ai = new AI_Strategic();
                selNat.push_back(China);
                break;
            case Mongolia:
                civ = new Civilization(Mongolia, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/mongolia.txt");
                //ai = new AI_Strategic();
                selNat.push_back(Mongolia);
                break;
            case Aztec:
                civ = new Civilization(Aztec, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/aztec.txt");
                //ai = new AI_Strategic();
                selNat.push_back(Aztec);
                break;
            case France:
                civ = new Civilization(France, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/france.txt");
               // ai = new AI_Strategic();
                selNat.push_back(France);
                break;
            default:
                //Always default to Ghandi.
                civ = new Civilization(India, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/india.txt");
               // ai = new AI_Strategic();
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
    paint.drawText(playerInfoRect, Qt::AlignRight, QString("Turn %1 | %2 %3  ").arg(gameTurn).arg(abs(year)).arg((year < 0) ? "BC" : "AD"));
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
            qDebug() << "Player's turn ended";
            turnEnded = false;
            EndTurn();
        }
    }
    else
    {
        StartTurn();
        QFuture<void> future = QtConcurrent::run(this->ac, AI_Controller::turnStarted, civList.at(currentTurn));
        future.waitForFinished();
        qDebug() << "   AI Turn Finished";
        EndTurn();
    }

}

void GameManager::StartTurn()
{
    if(currentTurn == 0)
    {
        gameTurn++;

        //Set the number of years to pass per turn.
        //  This is based on the standard game pace in Civ V

        if(gameTurn < 75)
        {
            yearPerTurn = 40;
        }
        else if(gameTurn < 135)
        {
            yearPerTurn = 25;
        }
        else if(gameTurn < 160)
        {
            yearPerTurn = 20;
        }
        else if(gameTurn < 210)
        {
            yearPerTurn = 10;
        }
        else if(gameTurn < 270)
        {
            yearPerTurn = 5;
        }
        else if(gameTurn < 320)
        {
            yearPerTurn = 2;
        }
        else if(gameTurn < 440)
        {
            yearPerTurn = 1;
        }
        else
        {
            yearPerTurn = 0.5;
        }

        year += yearPerTurn;

        goldText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalGold()).arg(civList.at(0)->getCivYield()->GetGoldYield()));
        prodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetProductionYield()));
        foodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetFoodYield()));
        sciText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalScience()).arg(civList.at(0)->getCivYield()->GetScienceYield()));
        culText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetCultureYield()));
    }

    civList.at(currentTurn)->UpdateProgress();

    qDebug() << "  Starting turn for civ" << currentTurn;
}

void GameManager::EndTurn()
{
    qDebug() << "Ending Turn";

    foreach(Unit* unit, civList.at(currentTurn)->GetUnitList())
    {
        qDebug() << "           is unit path empty:" << unit->isPathEmpty();
        if(!unit->RequiresOrders && !unit->isPathEmpty())
        {
            qDebug() << "  Updating unit positions";
            uc->MoveUnit(unit, map, gameView->GetScene());
            renderer->UpdateUnits(map, gameView, unit);
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

void GameManager::InitButtons()
{
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

    showTechTreeButton = new QPushButton("Technology Tree");
    connect(showTechTreeButton, SIGNAL(clicked(bool)), this, SLOT(showTechTree()));

    moveUnit = new QPushButton("Move Unit");
    connect(moveUnit, SIGNAL(clicked(bool)), this, SLOT(moveUnitTo()));
    moveUnit->setEnabled(false);

    endTurn = new QPushButton("End Turn");
    connect(endTurn, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));

    buildFarm = new QPushButton("Build Farm");
    connect(buildFarm, SIGNAL(clicked(bool)), this, SLOT(buildNewFarm()));
    buildFarm->setEnabled(false);

    buildMine = new QPushButton("Build Mine");
    connect(buildMine, SIGNAL(clicked(bool)), this, SLOT(buildNewMine()));
    buildMine->setEnabled(false);

    buildPlantation = new QPushButton("Build Plantation");
    connect(buildPlantation, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildPlantation->setEnabled(false);

    buildTradePost = new QPushButton ("Build Trading Post");
    connect(buildTradePost, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildTradePost->setEnabled(false);

    buildRoad = new QPushButton("Build Road");
    connect(buildRoad, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildRoad->setEnabled(false);

    foundCity = new QPushButton("Found City");
    connect(foundCity, SIGNAL(clicked(bool)), this, SLOT(foundNewCity()));
    foundCity->setEnabled(false);
}

void GameManager::InitLayouts()
{
    vLayout->setMargin(0);

    unitControlButtons->addWidget(showTechTreeButton);
    unitControlButtons->addSpacing(800);
    unitControlButtons->addWidget(buildFarm);
    unitControlButtons->addWidget(buildMine);
    unitControlButtons->addWidget(buildPlantation);
    unitControlButtons->addWidget(buildTradePost);
    unitControlButtons->addWidget(buildRoad);
    unitControlButtons->addWidget(moveUnit);

    gameLayout->addLayout(unitControlButtons);
    gameLayout->addWidget(cityScreen);
    gameLayout->addWidget(gameView);
    gameLayout->addWidget(techTree);

    playerControlButtons->addWidget(exitGame);
    playerControlButtons->addSpacing(700);
    playerControlButtons->addWidget(endTurn);

    gameLayout->addLayout(playerControlButtons);

    vLayout->addLayout(gameLayout);

    hLayout->addWidget(renderPlusOne);
    hLayout->addWidget(renderMinusOne);
    hLayout->addWidget(showDummyCityScreen);

    vLayout->addLayout(hLayout);
}

void GameManager::InitYieldDisplay()
{
    goldText = new QLabel(QString("%1 (+%2)").arg(civList.at(0)->GetTotalGold()).arg(civList.at(0)->getCivYield()->GetGoldYield()));
    prodText = new QLabel(QString("%1").arg(civList.at(0)->getCivYield()->GetProductionYield()));
    foodText = new QLabel(QString("%1").arg(civList.at(0)->getCivYield()->GetFoodYield()));
    sciText = new QLabel(QString("%1 (+%2)").arg(civList.at(0)->GetTotalScience()).arg(civList.at(0)->getCivYield()->GetScienceYield()));
    culText = new QLabel(QString("%1").arg(civList.at(0)->getCivYield()->GetCultureYield()));

    goldText->setStyleSheet("QLabel { color: white; }");
    prodText->setStyleSheet("QLabel { color: white; }");
    foodText->setStyleSheet("QLabel { color: white; }");
    sciText->setStyleSheet("QLabel { color: white; }");
    culText->setStyleSheet("QLabel { color: white; }");

    goldText->setAlignment(Qt::AlignLeft);
    prodText->setAlignment(Qt::AlignLeft);
    foodText->setAlignment(Qt::AlignLeft);
    sciText->setAlignment(Qt::AlignLeft);
    culText->setAlignment(Qt::AlignLeft);

    goldText->setAlignment(Qt::AlignVCenter);
    prodText->setAlignment(Qt::AlignVCenter);
    foodText->setAlignment(Qt::AlignVCenter);
    sciText->setAlignment(Qt::AlignVCenter);
    culText->setAlignment(Qt::AlignVCenter);

    goldPix = new QPixmap("../ProjectHiawatha/Assets/Icons/gold.png");
    prodPix = new QPixmap("../ProjectHiawatha/Assets/Icons/production.png");
    foodPix = new QPixmap("../ProjectHiawatha/Assets/Icons/food.png");
    sciPix = new QPixmap("../ProjectHiawatha/Assets/Icons/research.png");
    culPix = new QPixmap("../ProjectHiawatha/Assets/Icons/culture.png");

    goldLabel = new QLabel();
    prodLabel = new QLabel();
    foodLabel = new QLabel();
    sciLabel = new QLabel();
    culLabel = new QLabel();

    goldLabel->setPixmap(*goldPix);
    prodLabel->setPixmap(*prodPix);
    foodLabel->setPixmap(*foodPix);
    sciLabel->setPixmap(*sciPix);
    culLabel->setPixmap(*culPix);

    Yieldinfo->addWidget(goldLabel);
    Yieldinfo->addWidget(goldText);

    Yieldinfo->addWidget(prodLabel);
    Yieldinfo->addWidget(prodText);

    Yieldinfo->addWidget(foodLabel);
    Yieldinfo->addWidget(foodText);

    Yieldinfo->addWidget(sciLabel);
    Yieldinfo->addWidget(sciText);

    Yieldinfo->addWidget(culLabel);
    Yieldinfo->addWidget(culText);

    Yieldinfo->addSpacing(1500);

    vLayout->insertLayout(0, Yieldinfo);
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
        cityScreen->setGeometry(100, 25, this->width() - 190, this->height() - 150);
        gameView->setDragMode(QGraphicsView::NoDrag);
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
    gameView->GetScene()->ProcessTile(map, relocateUnit);

    if(gameView->GetScene()->isTileSelected)
    {
        moveUnit->setEnabled(true);

//        unitToMove = uc->FindUnitAtTile(gameView->GetScene()->unitSelectedTile, map, civList.at(currentTurn)->GetUnitList());

//        if(unitToMove->isNonCombat())
//        {
//            if(unitToMove->isSettler())
//            {
//                foundCity->setEnabled(true);
//            }
//            else
//            {
//                buildFarm->setEnabled(true);
//                buildMine->setEnabled(true);
//                buildPlantation->setEnabled(true);
//                buildTradePost->setEnabled(true);
//                buildRoad->setEnabled(true);
//            }
//        }
    }

    if(gameView->GetScene()->unitMoveOrdered)
    {
        unitToMove = uc->FindUnitAtTile(gameView->GetScene()->unitSelectedTile, map, civList.at(currentTurn)->GetUnitList());

        qDebug() <<"    Finding path";
        uc->FindPath(gameView->GetScene()->unitSelectedTile, gameView->GetScene()->unitTargetTile, map, gameView->GetScene(), unitToMove);

        relocateUnit = false;
        gameView->GetScene()->unitMoveOrdered = false;
        map->GetTileAt(unitToMove->GetTileIndex())->Selected = false;
        gameView->GetScene()->redrawTile = true;
        moveUnit->setEnabled(false);

        qDebug() << "   Done";
    }

    if(gameView->GetScene()->citySelected  && !cityScreenVisible)
    {
        gameView->GetScene()->citySelected = false;
        this->showCity();
    }

    TurnController();

    if(gameView->GetScene()->redrawTile)
    {
        renderer->UpdateScene(map, gameView->GetScene());
    }

    this->update();

    if(cityScreen->isHidden())
    {
        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        cityScreenVisible = false;
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
}

void GameManager::showTechTree()
{
    if(!techTreeVisible)
    {
        if(techTree != NULL)
        {
            delete techTree;
        }
        techTree = new TechTree(this);
        techTree->loadData(civList.at(0)->getCurrentTech(),civList.at(0)->getNextTech());
        techTree->setGeometry(100, 25, this->width() - 190, this->height() - 150);
        techTree->show();
        techTreeVisible = true;
    }
    else
    {
        techTree->hide();
        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        techTreeVisible = false;
    }
}

void GameManager::foundNewCity()
{

}

void GameManager::buildNewRoad()
{

}

void GameManager::buildNewFarm()
{

}

void GameManager::buildNewPlantation()
{

}

void GameManager::buildNewTradePost()
{

}

void GameManager::buildNewMine()
{

}




