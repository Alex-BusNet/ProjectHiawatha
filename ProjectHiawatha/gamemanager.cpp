#include "gamemanager.h"
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QThread>
#include <ctime>
#include <algorithm>
#include <QtConcurrent/QtConcurrent>
#include <QMediaPlayer>
#include "unittype.h"
#include "datatypes.h"
#include <QDesktopWidget>
#include <random>
#include <QTime>


QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);
QString warStyle = "QMessageBox { background-color: #145e88 } QPushButton {  background-color: #4899C8; border: 2px solid #f6b300; border-radius: 3px; font: 10px; min-width: 70px; } QPushButton#Cancel { background-color: #f53252 } QPushButton:pressed { background-color: #77adcb; }";

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player, int numAI) : QWidget(parent)
{
    gameView = new GameView(this);
    ac = new AI_Controller();
    clv = new QListWidget(this);
    ns = new NotificationSystem(this);
    about = new About();
    diplo = new Diplomacy(this);
    diplo->hide();

    warbox = new QMessageBox();
    warbox->addButton(QMessageBox::Cancel);
    warbox->addButton(QMessageBox::Ok);
    warbox->button(QMessageBox::Ok)->setText("Go to War");
    warbox->button(QMessageBox::Cancel)->setText("Let me reconsider...");

    warbox->setStyleSheet(warStyle);

    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    gameLayout = new QHBoxLayout();
    playerControlButtons = new QVBoxLayout();
    unitControlButtons = new QVBoxLayout();
    Yieldinfo = new QHBoxLayout();

    cityScreen = new CityScreen();
    cityScreen->hide();
    techTree = new TechTree(this);

    selectedTileQueue = new QQueue<SelectData>();
    tileModifiedQueue = new QQueue<SelectData>();
    viewUpdateTiles = new QQueue<ViewData>();

    techLabel = new QLabel(" NO RESEARCH ");
    techText = new QLabel(" 00/000 ");
    endGameProgress = new QLabel("Capitals Controlled:");
    endGameText = new QString(" 0/0 ");

    unitToMove = NULL;
    targetUnit = NULL;
    targetCity = NULL;
    unitTile = NULL;
    targetTile = NULL;
    state = IDLE;

    cityScreenVisible = false;
    techTreeVisible = false;
    diploVisible = false;
    toggleOn = false;
    relocateUnit = false;
    turnEnded = false;
    turnStarted = true;
    countTime = false;
    citySelected = false;
    updateFoW = false;
    focusChanged = false;
    fortify = false;

    currentProductionName = "No Production Selected";
    playerCiv = player;

    if(!fullscreen)
    {
        this->setFixedSize(1400, 700);
        this->setWindowTitle("Project Hiawatha");
        gameView->setFixedWidth(1195);
    }
    else
    {
        this->setWindowState(Qt::WindowFullScreen);
    }

    gameView->ConfigureGraphicsView();

    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTiles()));

    this->InitButtons();
    this->InitLayouts();
    this->setLayout(vLayout);
    this->show();

    ns->hide();

    renderer = new Renderer(mapSizeX);

    map = new Map(mapSizeX, mapSizeY);
#ifdef __APPLE__
    this->map->InitHexMap();
    this->InitCivs(player, numAI);
#else
    mapInit = QtConcurrent::run(this->map, Map::InitHexMap);
    civInit = QtConcurrent::run(this, GameManager::InitCivs, player, numAI);
#endif
    civInit.waitForFinished();

    this->playersAliveCount = civList.size();

    renderer->DrawHexScene(map, gameView);

    for(int i = 0; i < civList.size(); i++)
    {
        renderer->LoadCities(civList.at(i)->GetCityList(), gameView);
        renderer->DrawUnits(civList.at(i)->GetUnitList(), map, gameView);

        for(int j = 0; j < civList.at(i)->GetCityList().size(); j++)
        {
            renderer->DrawCityBorders(civList.at(i)->GetCityAt(j), gameView, civList.at(i)->getCiv());
            if(i == 0)
            {
                if(j == 0)
                {
                    clv->addItem(civList.at(i)->GetCityAt(j)->GetName().append(" (").append(QChar(0x2605)).append(")"));
                }
                else
                {
                    clv->addItem(civList.at(i)->GetCityAt(j)->GetName());
                }

                foreach(Tile* tile, civList.at(i)->GetCityAt(j)->GetControlledTiles())
                {
                    renderer->SetTileWorkedIcon(tile, gameView);
                    renderer->SetTileTooltip(tile->GetTileIndex(), *tile->GetYield(), civList.at(i)->getCiv(), tile->GetTileIDString());
                }
            }

            civList.at(i)->GetCityAt(j)->loadUnits("Assets/Units/UnitList.txt");
            civList.at(i)->GetCityAt(j)->loadBuildings("Assets/Buildings/BuildingList.txt");
        }

        if(i == 0)
        {
            qDebug() << "Setting up Fog of War";
            foreach(Tile* n, map->GetNeighborsRange(civList.at(i)->GetCityAt(0)->GetCityTile(), 3))
            {
                if(!n->DiscoveredByPlayer)
                {
                    n->DiscoveredByPlayer = true;
                    n->CanAlwaysBeSeen = true;
                    viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), DISCOVERED});
                    renderer->SetTileTooltip(n->GetTileIndex(), *n->GetYield(), n->GetControllingCiv(), n->GetTileIDString());
                }

                if(!n->IsSeenByPlayer)
                {
                    n->IsSeenByPlayer = true;
                    n->CanAlwaysBeSeen = true;
                    viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), VISIBLE});
                }
            }

            endGameText = new QString("Capitals Controlled:");
            endGameText->append(QString("\nYou  1/%1").arg(civList.size()));
        }
        else
        {
            endGameText->append(QString("\n%1     1/%2").arg(civList.at(i)->GetLeaderName()).arg(civList.size()));
        }

        civList.at(i)->UpdateCivYield();
    }

    endGameProgress->setText(*endGameText);

    InitYieldDisplay();

    ////Keep this statement. I need it at different points
    /// in the debugging process. -Port
//    renderer->DrawGridLines(gameView);
//    renderer->DrawGuiText(map, stringData, gameView);

    zoomScale = 1;

    for(int i = 0; i < 3; i++)
    {
        zoomIn();
    }

    currentTurn = 0;
    gameTurn = 0;
    //Start at 4000 BC. The game increments the turn to 1, and subsequently the in game year by 40 years, upon game start.
    year = -4040;

    gameView->centerOn(civList.at(0)->GetCityAt(0)->GetCityTile()->GetCenter());
    qsrand(QTime::currentTime().msec());

    playerInfoRect = new QRect(0, 0, this->width(), 20);
    gameStatusRect = new QRect(0, this->height() - 20, this->width(), 20);
    statusMessage = " ";

    updateTimer->start();
}

GameManager::~GameManager()
{
    qDebug() << "   GameManager Dec'tor called";
    updateTimer->stop();

    foreach(Civilization* c, civList)
    {
        foreach(Unit* unit, c->GetUnitList())
        {
            delete unit;
        }

        foreach(City *ci, c->GetCityList())
        {
            delete ci;
        }

        delete c;
    }

    foreach(QGraphicsTextItem* sd, stringData)
    {
        delete sd;
    }

    if(renderer != NULL)
        delete renderer;

    if(map != NULL)
        delete map;

    if(diplo != NULL)
        delete diplo;

    if(uc != NULL)
        delete uc;

    if(ns != NULL)
        delete ns;

    if(about != NULL)
        delete about;

    if(cityScreen != NULL)
        delete cityScreen;

    if(techTree != NULL)
        delete techTree;

    qDebug() << "       Deleting GameManager Buttons";
    delete exitGame;
    delete endTurn;
    delete moveUnit;
    delete showTechTreeButton;
    delete showDiplomacy;
    delete buildFarm;
    delete buildMine;
    delete buildTradePost;
    delete buildPlantation;
    delete buildRoad;
    delete foundCity;
    delete attackUnit;
    delete attackCity;
    delete rangeAttack;
    delete fortifyUnit;
    delete help;
    delete toggleFoW;

    qDebug() << "       Deleting city focus buttons";
    delete goldFocus;
    delete productionFocus;
    delete scienceFocus;
    delete foodFocus;
    delete cultureFocus;

    qDebug() << "       Deleting YieldDisplay, tech progress, and endGame";
    delete goldLabel;
    delete goldText;
    delete prodLabel;
    delete prodText;
    delete sciLabel;
    delete sciText;
    delete foodLabel;
    delete foodText;
    delete culLabel;
    delete culText;
    delete techLabel;
    delete techText;
    delete endGameProgress;
    delete endGameText;
    delete warbox;

    qDebug() << "       Deleting yield pixmaps";
    if(goldPix != NULL)
        delete goldPix;

    if(prodPix != NULL)
        delete prodPix;

    if(sciPix != NULL)
        delete sciPix;

    if(foodPix != NULL)
        delete foodPix;

    if(culPix != NULL)
        delete culPix;

    qDebug() << "       Deleting misc pointers";
    if(playerInfoRect != NULL)
        delete playerInfoRect;

    if(gameStatusRect != NULL)
        delete gameStatusRect;

    if(unitToMove != NULL)
        delete unitToMove;

    if(targetUnit != NULL)
        delete targetUnit;

    if(targetCity != NULL)
        delete targetCity;

    qDebug() << "       Deleting Queues";
    if(!selectedTileQueue->isEmpty())
        selectedTileQueue->clear();

    if(selectedTileQueue != NULL)
        delete selectedTileQueue;

    if(!tileModifiedQueue->isEmpty())
        tileModifiedQueue->clear();

    if(tileModifiedQueue != NULL)
        delete tileModifiedQueue;

    if(!viewUpdateTiles->isEmpty())
        viewUpdateTiles->clear();

    if(viewUpdateTiles != NULL)
        delete viewUpdateTiles;

    if(unitTile != NULL)
        delete unitTile;

    if(targetTile != NULL)
        delete targetTile;

    delete updateTimer;

    delete gameView;

    qDebug() << "   Game Manager Deconstructed";
}

void GameManager::WarByDiplomacy()
{
    int targetCivListIndex = diplo->GetIndex();

    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetCivListIndex)->GetLeaderName())});

    civList.at(currentTurn)->SetAtWar(targetCivListIndex);
    civList.at(targetCivListIndex)->SetAtWar(currentTurn);
    diplo->DeclareWarOn(civList.at(targetCivListIndex)->getCiv(), targetCivListIndex, civList.at(currentTurn)->getCiv());
    diplo->declareWar->setEnabled(false);
}

void GameManager::MakePeace()
{
    ProcessPeace(diplo->GetIndex());
}

void GameManager::InitCivs(Nation player, int numAI)
{
    Civilization* civ = new Civilization(player, false, " ");
    civ->loadTechs("Assets/Techs/Technology.txt");
    civ->setCurrentTech(civ->GetTechList().at(0));
    civ->setNextTech(civ->GetTechList().at(1));
    civ->setCivIndex(0);
    techLabel->setText(QString(" %1 ").arg(civ->getCurrentTech()->getName()));
    QPixmap pic;

    QString str = "Assets/CityLists/";
    QString str2;
    switch (player)
    {
    case America:
        str2 = "america.txt";
        civ->SetLeaderName(QString("Washington"));
        pic = QPixmap("Assets/Leaders/George_head.jpg");
        break;
    case Germany:
        str2 = "germany.txt";
        civ->SetLeaderName(QString("Bismark"));
        pic = QPixmap("Assets/Leaders/bismark.jpg");
        break;
    case India:
        str2 = "india.txt";
        civ->SetLeaderName(QString("Gandhi"));
        pic = QPixmap("Assets/Leaders/gandhi.jpg");
        break;
    case China:
        str2 = "china.txt";
        civ->SetLeaderName(QString("Zedong"));
        pic = QPixmap("Assets/Leaders/Mao.jpg");
        break;
    case Mongolia:
        str2 = "mongolia.txt";
        civ->SetLeaderName(QString("Genghis Khan"));
        pic = QPixmap("Assets/Leaders/khan.jpg");
        break;
    case Aztec:
        str2 = "aztec.txt";
        civ->SetLeaderName(QString("Montezuma"));
        pic = QPixmap("Assets/Leaders/montezuma.jpg");
        break;
    case France:
        str2 = "france.txt";
        civ->SetLeaderName(QString("Napoleon"));
        pic = QPixmap("Assets/Leaders/napoleon.jpg");
        break;
    case Iroquois:
        str2 = "iroquois.txt";
        civ->SetLeaderName(QString("Hiawatha"));
        pic = QPixmap("Assets/Leaders/Hiawatha.jpg");
        break;
    case Greece:
        str2 = "greece.txt";
        civ->SetLeaderName(QString("Alexander"));
        pic = QPixmap("Assets/Leaders/Alexander.jpg");
        break;
    case Rome:
        str2 = "rome.txt";
        civ->SetLeaderName(QString("Ceasar"));
        pic = QPixmap("Assets/Leaders/Julius_Caesar.jpg");
        break;
    case England:
        str2 = "england.txt";
        civ->SetLeaderName(QString("Elizabeth"));
        pic = QPixmap("Assets/Leaders/Queen_Elizabeth.jpg");
        break;
    case Arabia:
        str2 = "arabia.txt";
        civ->SetLeaderName(QString("al-Rashid"));
        pic =  QPixmap("Assets/Leaders/Harun-Rashid.jpg");
        break;
    case Persia:
        str2 = "persia.txt";
        civ->SetLeaderName(QString("Cyrus"));
        pic = QPixmap("Assets/Leaders/Cyrus.jpg");
        break;
    case Russia:
        str2 = "russia.txt";
        civ->SetLeaderName(QString("Stalin"));
        pic = QPixmap("Assets/Leaders/stalin.jpg");
        break;
    case Japan:
        str2 = "japan.txt";
        civ->SetLeaderName(QString("Nobunga"));
        pic = QPixmap("Assets/Leaders/Oda_Nobunga.jpg");
        break;
    case Egypt:
        str2= "egypt.txt";
        civ->SetLeaderName(QString("Ramesses"));
        pic = QPixmap("Assets/Leaders/Ramseses.jpg");
        break;
    default:
        str2 = "india.txt";
        civ->SetLeaderName(QString("Gandhi"));
        pic = QPixmap("Assets/Leaders/gandhi.jpg");
        break;
    }
    str = str + str2;
    civ->loadCities(str);
    civList.push_back(civ);
    diplo->AddLeader(civ->GetLeaderName(), pic, player, true);
    srand(time(0));
    int civNum;
    bool found;
    QVector<int> selNat;

    selNat.push_back(player);

    for(int i = 0; i < numAI; i++)
    {
newCivRand:
        // The modulo number at the end indicates the
        // max number of civs in the game.
        civNum = rand() % 16;

        if(civNum != player)
        {
            // look to see if the selected civ has already been chosen
            foreach(int j, selNat)
            {
                if(j == civNum)
                {
                    found = true;
                    break;
                }
            }

            switch (civNum)
            {
            case America:
                civ = new Civilization(America, true, "Washington");
                civ->loadCities("Assets/CityLists/america.txt");
                pic = QPixmap("Assets/Leaders/George_head.jpg");
                selNat.push_back(civNum);
                break;
            case Germany:
                civ = new Civilization(Germany, true, "Bismark");
                civ->loadCities("Assets/CityLists/germany.txt");
                pic = QPixmap("Assets/Leaders/bismark.jpg");
                selNat.push_back(civNum);
                break;
            case India:
                civ = new Civilization(India, true, "Gandhi");
                civ->loadCities("Assets/CityLists/india.txt");
                pic = QPixmap("Assets/Leaders/gandhi.jpg");
                selNat.push_back(civNum);
                break;
            case China:
                civ = new Civilization(China, true, "Zedong");
                civ->loadCities("Assets/CityLists/china.txt");
                pic = QPixmap("Assets/Leaders/Mao.jpg");
                selNat.push_back(civNum);
                break;
            case Mongolia:
                civ = new Civilization(Mongolia, true, "Genghis Khan");
                civ->loadCities("Assets/CityLists/mongolia.txt");
                pic = QPixmap("Assets/Leaders/khan.jpg");
                selNat.push_back(civNum);
                break;
            case Aztec:
                civ = new Civilization(Aztec, true, "Montezuma");
                civ->loadCities("Assets/CityLists/aztec.txt");
                pic = QPixmap("Assets/Leaders/montezuma.jpg");
                selNat.push_back(civNum);
                break;
            case France:
                civ = new Civilization(France, true, "Napoleon");
                civ->loadCities("Assets/CityLists/france.txt");
                pic = QPixmap("Assets/Leaders/napoleon.jpg");
                selNat.push_back(civNum);
                break;
            case Iroquois:
                civ = new Civilization(Iroquois, true, "Hiawatha");
                civ->loadCities("Assets/CityLists/iroquois.txt");
                pic = QPixmap("Assets/Leaders/Hiawatha.jpg");
                selNat.push_back(Iroquois);
                break;
            case Greece:
                civ = new Civilization(Greece, true, "Alexander");
                civ->loadCities("Assets/CityLists/greece.txt");
                pic = QPixmap("Assets/Leaders/Alexander.jpg");
                selNat.push_back(civNum);
                break;
            case Rome:
                civ = new Civilization(Rome, true, "Ceasar");
                civ->loadCities("Assets/CityLists/rome.txt");
                pic = QPixmap("Assets/Leaders/Julius_Caesar.jpg");
                selNat.push_back(civNum);
                break;
            case England:
                civ = new Civilization(England, true, "Elizabeth");
                civ->loadCities("Assets/CityLists/england.txt");
                pic = QPixmap("Assets/Leaders/Queen_Elizabeth.jpg");
                selNat.push_back(civNum);
                break;
            case Arabia:
                civ = new Civilization(Arabia, true, "al-Rashid");
                civ->loadCities("Assets/CityLists/arabia.txt");
                pic =  QPixmap("Assets/Leaders/Harun-Rashid.jpg");
                selNat.push_back(civNum);
                break;
            case Persia:
                civ = new Civilization(Persia, true, "Cyrus");
                civ->loadCities("Assets/CityLists/persia.txt");
                pic = QPixmap("Assets/Leaders/Cyrus.jpg");
                selNat.push_back(civNum);
                break;
            case Russia:
                civ = new Civilization(Russia, true, "Stalin");
                civ->loadCities("Assets/CityLists/russia.txt");
                pic = QPixmap("Assets/Leaders/stalin.jpg");
                selNat.push_back(civNum);
                break;
            case Japan:
                civ = new Civilization(Japan, true, "Nobunga");
                civ->loadCities("Assets/CityLists/japan.txt");
                pic = QPixmap("Assets/Leaders/Oda_Nobunga.jpg");
                selNat.push_back(civNum);
                break;
            case Egypt:
                civ = new Civilization(Egypt, true, "Ramesses");
                civ->loadCities("Assets/CityLists/egypt.txt");
                pic = QPixmap("Assets/Leaders/Ramseses.jpg");
                selNat.push_back(civNum);
                break;
            default:
                //Always default to Ghandi.
                civ = new Civilization(India, true, "Gandhi");
                civ->loadCities("Assets/CityLists/india.txt");
                pic = QPixmap("Assets/Leaders/gandhi.jpg");
                selNat.push_back(civNum);
                break;
            }

            // If was not found, place in civList vector
            if(!found)
            {
                civ->loadTechs("Assets/Techs/Technology.txt");
                civ->setCurrentTech(civ->GetTechList().at(0));
                civ->setNextTech(civ->GetTechList().at(1));
                civ->setCivIndex(i+1);
                civList.push_back(civ);
                diplo->AddLeader(civ->GetLeaderName(), pic, (Nation)civNum, false);
            }
            // Otherwise, delete it and try again.
            else
            {
                found = false;
                delete civ;
                goto newCivRand;
            }
        }
        else
        {
            goto newCivRand;
        }

    }


    if(!mapInit.isFinished())
    {
        mapInit.waitForFinished();
    }

    map->SpawnCivs(civList);
}

void GameManager::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);

    paint.fillRect(*playerInfoRect, QBrush(Qt::black));
    paint.fillRect(*gameStatusRect, QBrush(Qt::black));
    paint.setPen(Qt::white);
    paint.drawText(*playerInfoRect, (Qt::AlignRight | Qt::AlignVCenter), QString("Turn %1 | %2 %3  ").arg(gameTurn).arg(abs(year)).arg((year < 0) ? "BC" : "AD"));
    paint.drawText(*gameStatusRect, (Qt::AlignHCenter | Qt::AlignVCenter), statusMessage);
}

void GameManager::TurnController()
{
    //Is it currently the Player's turn?
    if(currentTurn == 0)
    {
        //Has the player ended their turn?
        if(turnEnded)
        {
            turnEnded = false;
            EndTurn();
        }
        else if(turnStarted)
        {
            turnStarted = false;
            StartTurn();
        }
    }
    else
    {
        //Cycle through the AI turns.
        statusMessage = QString("--------<< Processing Turn for %1 >>--------").arg(civList.at(currentTurn)->GetLeaderName());
        StartTurn();

#ifdef __APPLE__
        this->ac->turnStarted(civList.at(currentTurn),civList.at(0),this->map);
        while(!civList.at(currentTurn)->isEmpty())
        {
            AIQueueData data = civList.at(currentTurn)->dequeue();
            state = data.action;

            if(state == AI_FOUND_CITY)
            {
                unitToMove = data.unit;
            }
            else if(state == CONQUER)
            {
                state = FIND_CITY;
                unitToMove = data.unit;
                targetTile = map->GetTileAt(data.unit->GetTargetTileIndex());
            }
            else if(state == AI_DECLARE_WAR)
            {
                WarByDiplomacy();
            }

            this->UpdateTileData();
        }
#else
        QFuture<void> future = QtConcurrent::run(this->ac, AI_Controller::turnStarted, civList.at(currentTurn), civList.at(0), this->map);
        //While the AI is taking its turn, check the Queue
        // if the AI is trying to settle a city or conquer one
        while(future.isRunning())
        {
            if(!civList.at(currentTurn)->isEmpty())
            {
                AIQueueData data = civList.at(currentTurn)->dequeue();
                state = data.action;

                if(state == AI_FOUND_CITY)
                {
                    unitToMove = data.unit;
                }
                else if(state == CONQUER)
                {
                    state = FIND_CITY;
                    unitToMove = data.unit;
                    targetTile = map->GetTileAt(data.unit->GetTargetTileIndex());
                }
                else if(state == AI_DECLARE_WAR)
                {
                    WarByDiplomacy();
                }

                this->UpdateTileData();
            }
        }
        //This is just for extra precautions so that we don't try to end the AI's
        // turn and move on before the AI thread has completly finished.
        future.waitForFinished();
#endif
        EndTurn();
    }

}

void GameManager::StartTurn()
{
    for(int i = 0; i < civList.size(); i++)
    {
        if(i == 0)
        {
            delete endGameText;
            endGameText = new QString("Capitals Controlled:");
            endGameText->append(QString("\nYou      %1/%2").arg(civList.at(i)->GetCapitalsControlled()).arg(civList.size()));
        }
        else
        {
            if(civList.at(i)->alive)
                endGameText->append(QString("\n%1     %2/%3").arg(civList.at(i)->GetLeaderName()).arg(civList.at(i)->GetCapitalsControlled()).arg(civList.size()));
        }
    }

    endGameProgress->setText(*endGameText);
    endGameProgress->setAlignment(Qt::AlignRight);

    //Update the progress in all cities the Civ controls.
    Update_t update = civList.at(currentTurn)->UpdateProgress();

    //Filter out each city's MEB list for any potential tiles that
    // have been claimed by another city.
    foreach(City* city, civList.at(currentTurn)->GetCityList())
    {
        city->FilterMEBList();
    }

    //Did a city grow?
    if(update.updateBorders)
    {
        //Alert the renderer to redraw all city borders.
        foreach(City* city, civList.at(currentTurn)->GetCityList())
        {
            map->GetTileQueue(city);
            map->DefineCityBordersNew(city);
            city->SortTileQueue();
            renderer->UpdateCityBorders(city, gameView, civList.at(currentTurn)->getCiv());

            if(currentTurn == 0)
            {
                foreach(Tile* tile, city->GetControlledTiles())
                {
                    renderer->SetTileWorkedIcon(tile, gameView);
                    renderer->SetTileTooltip(tile->GetTileIndex(), *tile->GetYield(), tile->GetControllingCiv(), tile->GetTileIDString());

                    foreach(Tile* n, map->GetNeighborsRange(tile, 2))
                    {
                        if(n->GetControllingCivListIndex() == 0)
                            continue;

                        if(!n->DiscoveredByPlayer)
                        {
                            viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), DISCOVERED});
                            renderer->SetTileTooltip(n->GetTileIndex(), *n->GetYield(), n->GetControllingCiv(), n->GetTileIDString());
                        }

                        if(!n->IsSeenByPlayer)
                        {
                            viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), VISIBLE});
                        }
                    }
                }
            }
        }
    }

    if(update.updateCitizens)
    {
        foreach(City* city, civList.at(currentTurn)->GetCityList())
        {
            if(currentTurn == 0)
            {
                foreach(Tile* tile, city->GetControlledTiles())
                {
                    renderer->SetTileWorkedIcon(tile, gameView);
                    renderer->SetTileTooltip(tile->GetTileIndex(), *tile->GetYield(), tile->GetControllingCiv(), tile->GetTileIDString());
                }

                if(update.updateCitizens)
                {
                    ns->PostNotification(Notification{2, QString("The city of %1 has grown!").arg(city->GetName())});
                }
            }
        }

    }

    //If it is the player's turn, then all the AI have completed their turns
    // so we can increment the turn and year counter.
    if(currentTurn == 0)
    {
        statusMessage = " ";
        gameTurn++;
        diplo->UpdateTurn();

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
    }

    int localIndex = 0;
    QString str[20];

    //--------------------------------------------------------
    // The following section updates the science yield of the
    // active civ object

    int scienceYield = civList.at(currentTurn)->getCivYield()->GetScienceYield();
    civList.at(currentTurn)->setAccumulatedScience(scienceYield);

    int accumulatedScience = civList.at(currentTurn)->getAccumulatedScience();

    int techCost = civList.at(currentTurn)->getCurrentTech()->getCost();
    if(accumulatedScience >= techCost)
    {
        if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv())
        {
            statusMessage = QString("--------<< You have finished researching %1 >>--------").arg(civList.at(0)->GetTechList().at(civList.at(0)->getTechIndex())->getName());
        }
        QString unlocks = "UNLOCKED: ";
        QString techName = civList.at(currentTurn)->GetTechList().at(civList.at(0)->getTechIndex())->getName();
        QString firstLine = "Finished: ";
        firstLine += techName;
        firstLine += "\n";
        civList.at(currentTurn)->setTechIndex();
        int techIndex = civList.at(currentTurn)->getTechIndex();

        for(int i = 0; i<civList.at(currentTurn)->GetCityList().size();i++)
        {
            for(int j = 0; j<civList.at(currentTurn)->GetCityList().at(i)->getInitialUnitList().size();j++)
            {
                if(civList.at(currentTurn)->GetCityList().at(0)->getInitialUnitList().at(j)->GetTechIndex() == (techIndex-1))
                {
                    if(i == 0)
                    {
                        unlocks+= civList.at(currentTurn)->GetCityList().at(0)->getInitialUnitList().at(j)->GetName();
                        unlocks+= ", ";
                    }

                    civList.at(currentTurn)->GetCityList().at(i)->getInitialUnitList().at(j)->setUnlocked(1);
                }  
            }

            for(int k = 0; k<civList.at(currentTurn)->GetCityList().at(i)->getInitialBuildingList().size();k++)
            {
                if(civList.at(currentTurn)->GetCityList().at(0)->getInitialBuildingList().at(k)->getTechIndex() == (techIndex - 1))
                {
                    if(i == 0)
                    {
                        unlocks+= civList.at(currentTurn)->GetCityList().at(0)->getInitialBuildingList().at(k)->getName();
                        unlocks+= ", ";
                    }

                    civList.at(currentTurn)->GetCityList().at(i)->getInitialBuildingList().at(k)->setUnlocked(1);
                }
            }

            civList.at(currentTurn)->GetCityAt(i)->SetBaseCityStrength(10);
        }

        civList.at(currentTurn)->setNextTech(civList.at(currentTurn)->GetTechList().at(techIndex+1));
        civList.at(currentTurn)->setCurrentTech(civList.at(currentTurn)->GetTechList().at(techIndex));
        civList.at(currentTurn)->resetAccumulatedScience();

        if(currentTurn == 0)
        {
            techLabel->setText(QString(" %1 ").arg(civList.at(0)->getCurrentTech()->getName()));
        }

        if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv())
        {

            if(unlocks.endsWith(", "))
            {
                unlocks.chop(2);
            }
            firstLine+=unlocks;
            QMessageBox *mbox = new QMessageBox();
            mbox->setText(firstLine);
            mbox->setWindowFlags(Qt::FramelessWindowHint);
            mbox->setStyleSheet(warStyle);
            mbox->exec();
            delete mbox;
        }
    }

    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // The following section updates the production progress of
    // active civ object

    for(int i = 0; i<civList.at(currentTurn)->GetCityList().size();i++)
    {
        if(update.productionFinished)
        {
            if(civList.at(currentTurn)->GetCityAt(i)->getProductionFinished())
            {
                civList.at(currentTurn)->GetCityAt(i)->setProductionFinished(false);

                if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv())
                {
                    str[localIndex] = civList.at(currentTurn)->GetCityList().at(i)->GetName();
                    localIndex++;

                }

                if(civList.at(currentTurn)->GetCityAt(i)->getIsUnit())
                {
                    civList.at(currentTurn)->GetCityAt(i)->setProducedUnit(civList.at(currentTurn)->GetCityAt(i)->getInitialUnitList().at(civList.at(currentTurn)->GetCityAt(i)->getProductionIndex()));
                    Unit* unit = new Unit(0);
                    Unit* unitData = civList.at(currentTurn)->GetCityAt(i)->getProducedUnit();
                    unit->setUnitType(unitData->GetUnitType());
                    unit->SetName(unitData->GetName());
                    unit->SetCost(unitData->GetCost());
                    unit->SetMovementPoints(unitData->GetMovementPoints());
                    unit->SetStrength(unitData->GetStrength());
                    unit->SetRange(unitData->GetRange());
                    unit->SetRangeStrength(unitData->GetRangeStrength());
                    unit->SetUnitIcon(unitData->GetUnitType());
                    unit->SetOwner(civList.at(currentTurn)->getCiv());
                    unit->SetUnitListIndex(civList.at(currentTurn)->GetUnitList().size());

                    for(int j = 0; j < civList.at(currentTurn)->GetCityAt(i)->GetControlledTiles().size();j++)
                    {
                        int tileIndex = civList.at(currentTurn)->GetCityAt(i)->GetControlledTiles().at(j)->GetTileIndex();
                        if(unit->isNaval())
                        {
                            if(map->GetTileAt(tileIndex)->ContainsUnit  || !(map->GetTileTypeAt(tileIndex) == WATER)) { continue; }
                            else
                            {

                                    unit->SetPositionIndex(tileIndex);
                                    map->GetTileAt(tileIndex)->ContainsUnit = true;
                                    break;

                            }
                        }
                        else
                        {
                            if(map->GetTileAt(tileIndex)->ContainsUnit || !(map->GetTileAt(tileIndex)->Walkable) || (map->GetTileTypeAt(tileIndex) == WATER)) { continue; }
                            else
                            {

                                    unit->SetPositionIndex(tileIndex);
                                    map->GetTileAt(tileIndex)->ContainsUnit = true;
                                    break;

                            }
                        }
                    }

                    civList.at(currentTurn)->AddUnit(unit);
                    renderer->AddUnit(unit,map,gameView);
                }
                else
                {
                    int science = 0;
                    int gold = 0;
                    int production = 0;
                    int culture = 0;
                    int food = 0;
                    civList.at(currentTurn)->GetCityList().at(i)->IncrementNumberOfBuildings();
                    int productionIndex = civList.at(currentTurn)->GetCityList().at(i)->getProductionIndex();
                    Building* building = civList.at(currentTurn)->GetCityList().at(i)->getInitialBuildingList().at(productionIndex);
                    int bonusType = building->getbonusType();
                    if(bonusType == 2)
                    {
                         science = building->getBonusValue();

                    }else if(bonusType == 0)
                    {
                       gold = building->getBonusValue();

                    }else if(bonusType == 3)
                    {
                         food = building->getBonusValue();
                    }else if(bonusType == 1)
                    {
                         production = building->getBonusValue();
                    }else if(bonusType == 4)
                    {
                         culture = building->getBonusValue();
                    }

                    if(0==currentTurn)
                    {
                        civList.at(0)->GetCityList().at(i)->GetCityTile()->SetYield(gold,production,science,food,culture);
                    }

                    civList.at(currentTurn)->GetCityList().at(i)->addBuilding(building);
                }

                if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv() && update.productionFinished)
                {
                    ns->PostNotification(Notification{4, QString("Production in %1 finished").arg(civList.at(currentTurn)->GetCityAt(i)->GetName())});
                }
            }

        }

        //-----------------------------------------------------------------------

        //If a city took damage and has healed, alert the renderer of this change.
        if(update.cityHealed)
        {
            renderer->UpdateCityHealthBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        }

        civList.at(currentTurn)->GetCityAt(i)->UpdateCityYield();

        civList.at(currentTurn)->UpdateCivYield();

        //Update the production and population growth bars for the city.
        renderer->UpdateCityProductionBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        renderer->UpdateCityGrowthBar(civList.at(currentTurn)->GetCityAt(i), gameView);
    }

    int civMilStr = 0;
    foreach(Unit* unit, civList.at(currentTurn)->GetUnitList())
    {
        if(!unit->RequiresOrders && unit->isPathEmpty() && !unit->isFortified)
        {
            unit->RequiresOrders = true;

            if(currentTurn == 0)
                renderer->SetUnitNeedsOrders(unit->GetTileIndex(), true);
            else
                renderer->SetUnitNeedsOrders(unit->GetTileIndex(), false);
        }
        else if(currentTurn != 0)
        {
            renderer->SetUnitNeedsOrders(unit->GetTileIndex(), false);
        }

        if(!unit->isNonCombat())
            civMilStr += unit->GetUnitPower();
    }

    qDebug() << "Military Strength for" << uc->NationName(civList.at(currentTurn)->getCiv()) << ": " << civMilStr;
    civList.at(currentTurn)->SetMilitaryStrength(civMilStr);

    if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv() && update.productionFinished)
    {
        QString cityList;
        for(int j = 0;j<20;j++)
        {
            if(str[j].isEmpty()) { continue; }
            else
            {
                QString str3 = str[j];
                str3+= ", ";
                cityList += str3;
            }

        }
        QString prodString = "Production has finished in: ";
        QString finalString = prodString+cityList;
        finalString.chop(2);
        QMessageBox *mbox = new QMessageBox();
        mbox->setText(finalString);
        mbox->setWindowFlags(Qt::FramelessWindowHint);
        mbox->setStyleSheet(warStyle);
        mbox->exec();
        delete mbox;

    }

    //Update the yield status bar for the player.
    if(currentTurn == 0)
    {
        goldText->setText(QString("%1 (%2%3)").arg(civList.at(0)->GetTotalGold()).arg(civList.at(0)->losingGold ? "-" : "+").arg(civList.at(0)->getCivYield()->GetGoldYield()));
        prodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetProductionYield()));
        foodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetFoodYield()));
        sciText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalScience()).arg(civList.at(0)->getCivYield()->GetScienceYield()));
        culText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalCulture()).arg(civList.at(0)->getCivYield()->GetCultureYield()));
        techText->setText(QString(" %1 / %2").arg(accumulatedScience).arg(techCost));

        endTurn->setEnabled(true);
    }
}

void GameManager::EndTurn()
{
    countTime = true;
    state = IDLE;
    processedData.relocateOrderGiven = false;
    processedData.newData = false;
    //This is for debugging purposes.
    begin = std::chrono::steady_clock::now();
    bool unitMoved = false;
    QList<Tile*> t;

    //Disable all the buttons when the player ends their turn.
    if(currentTurn == 0)
    {
        statusMessage = " ";
        moveUnit->setEnabled(false);

        buildFarm->setEnabled(false);
        buildMine->setEnabled(false);
        buildPlantation->setEnabled(false);
        buildTradePost->setEnabled(false);
        buildRoad->setEnabled(false);

        attackUnit->setEnabled(false);
        attackCity->setEnabled(false);
        rangeAttack->setEnabled(false);
        fortifyUnit->setEnabled(false);

        goldFocus->setEnabled(false);
        productionFocus->setEnabled(false);
        scienceFocus->setEnabled(false);
        cultureFocus->setEnabled(false);
        foodFocus->setEnabled(false);
        endTurn->setEnabled(false);
    }

    // Update all units that a city controls
    for(int i = 0; i < civList.at(currentTurn)->GetUnitList().size(); i++)
    {
        if(!civList.at(currentTurn)->GetUnitAt(i)->RequiresOrders && !civList.at(currentTurn)->GetUnitAt(i)->isPathEmpty())
        {
            if(currentTurn == 0)
            {
                renderer->SetUnitNeedsOrders(civList.at(0)->GetUnitAt(i)->GetTileIndex(), false);

                if(!toggleOn)
                {
                    t = map->GetNeighborsRange(map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex()), 2);
                    foreach(Tile* ti, t)
                    {
                        if(ti->IsSeenByPlayer && !ti->CanAlwaysBeSeen)
                        {
                            ti->IsSeenByPlayer = false;
                            viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), HIDDEN});
                        }
                    }
                }
            }

            uc->MoveUnit(civList.at(currentTurn)->GetUnitAt(i), map, currentTurn);

            if(currentTurn == 0)
            {
                tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

                if(!toggleOn)
                {
                    t = map->GetNeighborsRange(map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex()), 2);

                    foreach(Tile* ti, t)
                    {
                        if(!ti->CanAlwaysBeSeen)
                        {
                            if(!ti->DiscoveredByPlayer)
                            {
                                ti->DiscoveredByPlayer = true;
                                viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), DISCOVERED});
                            }

                            if(!ti->IsSeenByPlayer)
                            {
                                ti->IsSeenByPlayer = true;
                                viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), VISIBLE});
                            }
                        }

                        renderer->SetTileTooltip(ti->GetTileIndex(), *ti->GetYield(), ti->GetControllingCiv(), ti->GetTileIDString());
                    }
                }
            }

            unitMoved = true;
        }

        if(civList.at(currentTurn)->GetUnitAt(i)->GetHealth() < 0)
        {
            if(currentTurn == 0)
                ns->PostNotification(Notification{1, QString("Your %1 has been killed!").arg(civList.at(currentTurn)->GetUnitAt(i)->GetName())});

            if(civList.at(currentTurn)->getCiv() == civList.at(0)->getCiv())
            {
                QMediaPlayer *musicPlayer = new QMediaPlayer();
                musicPlayer->setMedia(QUrl::fromLocalFile("Assets/Sound/notificationunitkilled.wav"));
                musicPlayer->setVolume(50);
                musicPlayer->play();

            }

            //These statements handle removing a unit from the game if it is killed.
            renderer->SetFortifyIcon(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex(), true);
            renderer->SetUnitNeedsOrders(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex(), false);
            map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex())->ContainsUnit = false;
            renderer->RemoveUnit(civList.at(currentTurn)->GetUnitAt(i), gameView);
            civList.at(currentTurn)->RemoveUnit(i);
        }
        else
        {
            if((civList.at(currentTurn)->GetUnitAt(i)->GetHealth() < 100) && civList.at(currentTurn)->GetUnitAt(i)->isFortified)
            {
                uc->HealUnit(civList.at(currentTurn)->GetUnitAt(i));
            }

            renderer->UpdateUnits(map, gameView, civList.at(currentTurn)->GetUnitAt(i), unitMoved);

            if(currentTurn == 0)
            {
                if(civList.at(currentTurn)->GetUnitAt(i)->isPathEmpty() && !civList.at(currentTurn)->GetUnitAt(i)->isFortified)
                    civList.at(currentTurn)->GetUnitAt(i)->RequiresOrders = true;

                if(civList.at(currentTurn)->GetUnitAt(i)->RequiresOrders)
                {
                    renderer->SetUnitNeedsOrders(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex(), true);
                }
            }

            unitMoved = false;
        }
    }

    if(currentTurn == 0)
    {
        if(!tileModifiedQueue->isEmpty())
            renderer->UpdateScene(map, gameView, tileModifiedQueue);
    }

nextCivAlive:
    if(currentTurn == civList.size() - 1)
    {
        currentTurn = 0;
        turnEnded = false;
        turnStarted = true;
    }
    else
    {
        currentTurn++;
        //This allows an AI to be 'removed' from the game
        // without all references to the CivList index
        // needing to be adjusted when a civ is eliminated.
        if(!civList.at(currentTurn)->alive)
        {
            goto nextCivAlive;
        }
    }
}

/*
 * UpdateTileData() is the function that handles all of the tile selection, unit combat,
 * unit-to-city combat, and Declarations of War done by the player as well as handling
 * city founding and city conquering for both the player and the AI.
 */
void GameManager::UpdateTileData()
{
    if(!processedData.relocateOrderGiven && state == IDLE)
    {
        unitTile = map->GetTileFromCoord(processedData.column, processedData.row);

        if(unitTile->Selected)
        {
            unitTile->Selected = false;
            selectedTileQueue->enqueue(SelectData{unitTile->GetTileIndex(), false, false});
            this->redrawTile = true;
        }

        if(unitTile->ContainsUnit)
        {
            state = FIND_UNIT;
        }
    }
    else if(state == ATTACK_MELEE || processedData.relocateOrderGiven || state == ATTACK_RANGE || state == ATTACK_CITY)
    {
        targetTile = map->GetTileFromCoord(processedData.column, processedData.row);

        if(targetTile->ContainsUnit && processedData.relocateOrderGiven)
        {
            QList<Tile*> neighbors = map->GetNeighbors(targetTile);

            foreach(Tile* tile, neighbors)
            {
                if(!tile->ContainsUnit && !tile->HasCity && tile->Walkable)
                {
                    targetTile = tile;
                    break;
                }
            }
        }

        if((targetTile->ContainsUnit || targetTile->HasCity) && (targetTile->GetControllingCivListIndex() != 0) && (targetTile->GetControllingCivListIndex() != -1))
        {
            if(uc->AtPeaceWith(targetTile, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()}))
            {
                if(gameTurn == 1)
                {
                    statusMessage = "--------<< You cannot declare war on the first turn. >>--------";
                    state = IDLE;
                }
                else
                {
                    warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(targetTile->GetControllingCivListIndex())->GetLeaderName()));

                    warbox->setWindowFlags(Qt::FramelessWindowHint);

                    connect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarDeclared()));
                    connect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));

                    warbox->exec();

                    disconnect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarDeclared()));
                    disconnect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));
                }
            }
        }

        if(!targetTile->ContainsUnit  && (state == ATTACK_MELEE || state == ATTACK_RANGE))
        {
            QList<Tile*> neighbors = map->GetNeighbors(targetTile);

            foreach(Tile* tile, neighbors)
            {
                if(tile->ContainsUnit && !tile->HasCity)
                {
                    if(tile->GetOccupyingCivListIndex() != currentTurn)
                    {
                        if(!uc->AtPeaceWith(tile, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()}))
                        {
                            targetTile = tile;
                            break;
                        }
                    }
                }
            }
        }
        else if(!targetTile->HasCity && (state == ATTACK_CITY))
        {
            QList<Tile*> neighbors = map->GetNeighbors(targetTile);

            foreach(Tile* tile, neighbors)
            {
                if(tile->HasCity)
                {
                    targetTile = tile;
                    break;
                }
            }
        }

        if(state == ATTACK_CITY)
        {
            state = FIND_CITY;
        }
    }

    if(state == FIND_UNIT)
    {
        state = IDLE;

        if(unitToMove != NULL)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        }

        unitToMove = uc->FindUnitAtTile(unitTile, civList.at(currentTurn)->GetUnitList());

        selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), true, false});
        tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

        if(unitToMove->GetOwner() == civList.at(currentTurn)->getCiv() && (unitToMove->RequiresOrders || unitToMove->isFortified))
        {
            if(unitToMove->isFortified)
            {
                renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
                unitToMove->isFortified = false;
            }

            map->GetTileAt(unitToMove->GetTileIndex())->Selected = true;
            moveUnit->setEnabled(true);
            this->redrawTile = true;

            if(unitToMove->isNonCombat())
            {
                attackUnit->setEnabled(false);
                attackCity->setEnabled(false);
                rangeAttack->setEnabled(false);
                fortifyUnit->setEnabled(false);

                if(unitToMove->GetUnitType() == SETTLER)
                {
                    if(!map->GetTileAt(unitToMove->GetTileIndex())->HasCity)
                    {
                        foundCity->setEnabled(true);

                        buildFarm->setEnabled(false);
                        buildMine->setEnabled(false);
                        buildPlantation->setEnabled(false);
                        buildTradePost->setEnabled(false);
                        buildRoad->setEnabled(false);
                    }
                }
                else if (unitToMove->GetUnitType() == WORKER)
                {
                    if(map->GetTileAt(unitToMove->GetTileIndex())->GetControllingCiv() == civList.at(currentTurn)->getCiv())
                    {
                        buildFarm->setEnabled(true);
                        buildMine->setEnabled(true);
                        buildPlantation->setEnabled(true);
                        buildTradePost->setEnabled(true);
                    }

                    foundCity->setEnabled(false);

                    buildRoad->setEnabled(true);
                }
            }
            else //Combat Unit button controls
            {
                attackCity->setEnabled(false);
                attackUnit->setEnabled(false);
                rangeAttack->setEnabled(false);
                foundCity->setEnabled(false);
                buildFarm->setEnabled(false);
                buildMine->setEnabled(false);
                buildPlantation->setEnabled(false);
                buildTradePost->setEnabled(false);
                buildRoad->setEnabled(false);

                fortifyUnit->setEnabled(true);

                QList<Tile*> tiles = map->GetNeighborsRange(unitTile, unitToMove->GetRange());

                foreach(Tile *tile, tiles)
                {
                    if((tile->GetOccupyingCivListIndex() != 0) && (tile->GetOccupyingCivListIndex() != -1))
                    {
                        int tileIndex = tile->GetTileIndex();

                        selectedTileQueue->enqueue(SelectData {tileIndex, false, true});
                        tileModifiedQueue->enqueue(SelectData {tileIndex, false, false});

                        if(tile->HasCity)
                        {
                            attackCity->setEnabled(true);
                        }

                        if(tile->ContainsUnit && !tile->HasCity)
                        {
                            if(unitToMove->isMelee)
                            {
                                attackUnit->setEnabled(true);
                            }
                            else if(!unitToMove->isMelee)
                            {
                                attackUnit->setEnabled(false);
                                rangeAttack->setEnabled(true);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if(currentTurn == 0)
                statusMessage = "--------<< You do not own that unit >>--------";

            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        }
    }
    else if(state == FIND_CITY)
    {
        state = IDLE;
        targetCity = uc->FindCityAtTile(targetTile, civList.at(targetTile->GetControllingCivListIndex())->GetCityList());

        if(currentTurn == 0)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            selectedTileQueue->enqueue(SelectData{targetCity->GetCityTile()->GetTileIndex(), false, false});
            attackCity->setEnabled(false);
        }

        if(!uc->AtPeaceWith(targetCity->GetCityTile(), WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()}))
        {
            uc->AttackCity(unitToMove, targetCity);

            //City Conquering Logic
            if(targetCity->GetCityHealth() <= 0 && unitToMove->isMelee)
            {
                if(currentTurn == 0)
                    statusMessage = QString("--------<< %1 Has Been Conquered! >>--------").arg(targetCity->GetName());

                ProcessCityConquer(targetCity, civList.at(currentTurn), civList.at(targetTile->GetControllingCivListIndex()));

                if(civList.at(targetTile->GetControllingCivListIndex())->GetCityList().size() == 0)
                {
                    if(!civList.at(targetTile->GetControllingCivListIndex())->GetUnitList().isEmpty())
                    {
                        foreach(Unit* unit, civList.at(targetTile->GetControllingCivListIndex())->GetUnitList())
                        {
                            renderer->RemoveUnit(unit, gameView);
                        }

                        for(int i = 0; i < civList.at(targetTile->GetControllingCivListIndex())->GetUnitList().size(); i++)
                        {
                            civList.at(targetTile->GetControllingCivListIndex())->RemoveUnit(0);
                        }
                    }

                    playersAliveCount--;
                }
            }

            renderer->UpdateUnits(map, gameView, unitToMove, false);
            renderer->UpdateCityHealthBar(targetCity, gameView);
            renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);
            this->redrawTile = true;
        }
    }

    if(processedData.relocateOrderGiven && state == MOVE_UNIT)
    {
        if(uc->AtPeaceWith(targetTile, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()})
                && unitToMove->GetOwner() != targetTile->GetControllingCiv())
        {
            if(gameTurn == 1)
            {
                statusMessage = "--------<< You cannot declare war on the first turn. >>--------";
                state = IDLE;
                relocateUnit = false;
            }
            else
            {
                if(targetTile->GetOccupyingCivListIndex() == -1)
                    warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(targetTile->GetControllingCivListIndex())->GetLeaderName()));
                else
                    warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(targetTile->GetOccupyingCivListIndex())->GetLeaderName()));

                warbox->setWindowFlags(Qt::FramelessWindowHint);

                connect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarByInvasion()));
                connect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));

                warbox->exec();

                disconnect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarByInvasion()));
                disconnect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));
            }
        }
        else
        {
            if(targetTile->ContainsUnit)
            {
                statusMessage = "--------<< You cannot move there >>--------";
                state = IDLE;
            }
            else
            {
                //Even though we aren't technically invading all the time,
                //  this is set to simply allow the move processing to occur
                state = INVADE;
            }
        }

        if(state == INVADE)
        {
            state = IDLE;

            unitToMove->SetUnitTargetTile(targetTile->GetTileID().column, targetTile->GetTileID().row);
            unitToMove->SetUnitTargetTileIndex(targetTile->GetTileIndex());

            if(unitToMove->isFortified)
            {
                unitToMove->isFortified = false;
                renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
            }

            uc->FindPath(unitTile, targetTile, map, unitToMove, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()});

            relocateUnit = false;
            processedData.relocateOrderGiven = false;
            map->GetTileAt(unitToMove->GetTileIndex())->Selected = false;
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            this->redrawTile = true;
        }

        foundCity->setEnabled(false);

        buildFarm->setEnabled(false);
        buildMine->setEnabled(false);
        buildPlantation->setEnabled(false);
        buildTradePost->setEnabled(false);
        buildRoad->setEnabled(false);

        attackUnit->setEnabled(false);
        attackCity->setEnabled(false);
        rangeAttack->setEnabled(false);
        fortifyUnit->setEnabled(false);
    }

    if(state == ATTACK_MELEE || state == ATTACK_RANGE)
    {
        state = IDLE;
        ProcessAttackUnit();
    }

    //City founding Logic. This operates nearly the same as
    // initial city spawning in the Map class
    if(state == FOUND_CITY || state == AI_FOUND_CITY)
    {
        state = IDLE;
        foundCityIndex = unitToMove->GetTileIndex();
        City* city = map->CreateCity(foundCityIndex, civList.at(currentTurn), false);

        bool valid = true;
        int dstX, dstY;
        for(int i = 0; i < civList.size(); i++)
        {
            foreach(City* existingCity, civList.at(i)->GetCityList())
            {
                dstX = city->GetCityTile()->GetTileID().column - existingCity->GetCityTile()->GetTileID().column;
                dstY = city->GetCityTile()->GetTileID().row - existingCity->GetCityTile()->GetTileID().row;

                if((abs(dstX) < 8 && dstY == 0) || (abs(dstX) <= 4 && abs(dstY) <= 4) || (dstX == 0 && abs(dstY) < 4))
                {
                    if(currentTurn == 0)
                        statusMessage = "--------<< You cannot settle this close to another city. >>--------";

                    civList.at(currentTurn)->SetCityIndex(civList.at(currentTurn)->getCityIndex() - 1);
                    foreach(Tile* tile, city->GetControlledTiles())
                    {
                        tile->SetControllingCiv(NO_NATION, -1);
                    }
                    delete city;
                    unitToMove->RequiresOrders = true;
                    civList.at(currentTurn)->cityFounded= false;
                    valid = false;
                    break;
                }
            }

            if(!valid)
                break;
        }

        if(!valid)
            return;

        city->loadBuildings("Assets/Buildings/BuildingList.txt");
        city->loadUnits("Assets/Units/UnitList.txt");
        civList.at(currentTurn)->AddCity(city);
        map->GetTileAt(foundCityIndex)->HasCity = true;

        QList<Tile*> cityMEB = map->GetNeighborsRange(city->GetCityTile(), 4);
        city->SetMaximumExpansionBorderTiles(cityMEB);
        city->FilterMEBList();

        map->GetTileQueue(city);
        map->DefineCityBordersNew(city);
        city->SortTileQueue();

        city->SortControlledTiles();
        city->GetControlledTiles().first()->IsWorked = true;

        renderer->AddCity(city, gameView, false);

        renderer->DrawCityBorders(city, gameView, civList.at(currentTurn)->getCiv());

        if(currentTurn == 0)
        {
            foreach(Tile* tile, city->GetControlledTiles())
            {
                renderer->SetTileWorkedIcon(tile, gameView);
            }

            renderer->SetFortifyIcon(foundCityIndex, true);
            renderer->SetUnitNeedsOrders(foundCityIndex, false);
        }
        civList.at(currentTurn)->UpdateCivYield();

        map->GetTileAt(foundCityIndex)->ContainsUnit = false;
        map->GetTileAt(foundCityIndex)->HasCity=true;
        map->GetTileAt(foundCityIndex)->SetControllingCiv(civList.at(currentTurn)->getCiv(), currentTurn);

        renderer->RemoveUnit(unitToMove, gameView);
        civList.at(currentTurn)->RemoveUnit(unitToMove->GetUnitListIndex());

        if(currentTurn == 0)
        {
            clv->addItem(city->GetName());
            selectedTileQueue->enqueue(SelectData{foundCityIndex, false, false});
            foundCity->setEnabled(false);
        }
    }

    if((map->GetTileFromCoord(processedData.column, processedData.row)->Selected == false) && state != AI_FOUND_CITY && state != IDLE)
    {
        if(unitToMove != NULL)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        }

        attackCity->setEnabled(false);
        attackUnit->setEnabled(false);
        rangeAttack->setEnabled(false);
        buildFarm->setEnabled(false);
        buildMine->setEnabled(false);
        buildPlantation->setEnabled(false);
        buildTradePost->setEnabled(false);
        buildRoad->setEnabled(false);
        moveUnit->setEnabled(false);
        fortifyUnit->setEnabled(false);
        redrawTile = true;
    }
}

void GameManager::InitButtons()
{
    QString GameStyle = "QWidget { background-color: #145e88; } QFrame { background-color: black; }";
    GameStyle += "QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; max-width: 100px; }";
    GameStyle += "QPushButton:pressed { background-color: #77adcb; }";
    GameStyle += "QScrollBar:vertical { border: 2px sold black; background: #77adcb; width: 15px; margin: 12px 0 12px 0;} QScrollBar::handle:vertical { background: #4899C8; min-height: 10px; }";
    GameStyle += "QScrollBar::add-line:vertical { border: 1px solid black; background: #dedede; height: 10px; subcontrol-position: bottom; subcontrol-origin: margin; }  QScrollBar::sub-line:vertical { border: 1px solid black; height: 10px; background: #dedede; subcontrol-position: top; subcontrol-origin: margin; }";
    GameStyle += "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 1px solid black; width: 3px; height: 3px; background: purple; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }";
    GameStyle += "QTabBar::tab { background: #dedede; border: 1px solid black; min-width: 10ex; padding: 3px;} QTabBar::tab:selected { background: #4899C8; } QTabBar::tab:hover { background: #77adcb; }";
    GameStyle += "QGraphicsView { background-color: transparent; border-color: transparent;}";
    GameStyle += "NotificationSystem { background-color: transparent; border-color: transparent; }";
    GameStyle += "QListWidget { background-color: grey; color: white; border: 3px inset black; }";
    GameStyle += "QProgressBar { border: 2px solid grey; border-radius: 5px; } QProgressBar::chunk { background-color: #CD96CD; }";
    GameStyle += "QLabel { background-color: transparent; color: white; }";

    this->setStyleSheet(GameStyle);

    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    showTechTreeButton = new QPushButton("Technology Tree");
    connect(showTechTreeButton, SIGNAL(clicked(bool)), this, SLOT(showTechTree()));
    showTechTreeButton->setShortcut(QKeySequence(Qt::Key_T));

    showDiplomacy = new QPushButton("Diplomacy");
    connect(showDiplomacy, SIGNAL(clicked(bool)), this, SLOT(toggleDiplomacy()));
    showDiplomacy->setShortcut(QKeySequence(Qt::Key_V));

    moveUnit = new QPushButton("Move Unit");
    connect(moveUnit, SIGNAL(clicked(bool)), this, SLOT(moveUnitTo()));
    moveUnit->setEnabled(false);
    moveUnit->setShortcut(QKeySequence(Qt::RightButton));

    endTurn = new QPushButton("End Turn");
    connect(endTurn, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));
    endTurn->setShortcut(QKeySequence(Qt::Key_Return));

    buildFarm = new QPushButton("Build Farm");
    connect(buildFarm, SIGNAL(clicked(bool)), this, SLOT(buildNewFarm()));
    buildFarm->setEnabled(false);
    buildFarm->setShortcut(QKeySequence(Qt::Key_F));

    buildMine = new QPushButton("Build Mine");
    connect(buildMine, SIGNAL(clicked(bool)), this, SLOT(buildNewMine()));
    buildMine->setEnabled(false);
    buildMine->setShortcut(QKeySequence(Qt::Key_S));

    buildPlantation = new QPushButton("Build Plantation");
    connect(buildPlantation, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildPlantation->setEnabled(false);
    buildPlantation->setShortcut(QKeySequence(Qt::Key_D));

    buildTradePost = new QPushButton ("Build Trading Post");
    connect(buildTradePost, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildTradePost->setEnabled(false);
    buildTradePost->setShortcut(QKeySequence(Qt::Key_G));

    buildRoad = new QPushButton("Build Road");
    connect(buildRoad, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildRoad->setEnabled(false);
    buildRoad->setShortcut(QKeySequence(Qt::Key_H));

    foundCity = new QPushButton("Found City");
    connect(foundCity, SIGNAL(clicked(bool)), this, SLOT(foundNewCity()));
    foundCity->setEnabled(false);
    foundCity->setShortcut(QKeySequence(Qt::Key_Q));

    attackUnit = new QPushButton("Attack");
    connect(attackUnit, SIGNAL(clicked(bool)), this, SLOT(attackMelee()));
    attackUnit->setEnabled(false);
    attackUnit->setShortcut(QKeySequence(Qt::Key_W));

    goldFocus = new QPushButton("Gold Focus");
    connect(goldFocus, SIGNAL(clicked(bool)), this, SLOT(SetGoldFocus()));
    goldFocus->setEnabled(false);
    goldFocus->setMaximumWidth(100);

    productionFocus = new QPushButton("Production Focus");
    connect(productionFocus, SIGNAL(clicked(bool)), this, SLOT(SetProdFocus()));
    productionFocus->setEnabled(false);
    productionFocus->setMaximumWidth(100);

    scienceFocus = new QPushButton("Science Focus");
    connect(scienceFocus, SIGNAL(clicked(bool)), this, SLOT(SetScienceFocus()));
    scienceFocus->setEnabled(false);
    scienceFocus->setMaximumWidth(100);

    foodFocus = new QPushButton("Food Focus");
    connect(foodFocus, SIGNAL(clicked(bool)), this, SLOT(SetFoodFocus()));
    foodFocus->setEnabled(false);
    foodFocus->setMaximumWidth(100);

    cultureFocus = new QPushButton("Culture Focus");
    connect(cultureFocus, SIGNAL(clicked(bool)), this, SLOT(SetCultureFocus()));
    cultureFocus->setEnabled(false);
    cultureFocus->setMaximumWidth(100);

    connect(clv, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(parseItem()));
    connect(ns, SIGNAL(itemClicked(QListWidgetItem*)), ns, SLOT(removeNotification(QListWidgetItem*)));
    connect(diplo->makePeace, SIGNAL(clicked(bool)), this, SLOT(MakePeace()));
    connect(diplo->declareWar, SIGNAL(clicked(bool)), this, SLOT(WarByDiplomacy()));

    clv->setMaximumWidth(100);

    attackCity = new QPushButton("Attack City");
    connect(attackCity, SIGNAL(clicked(bool)), this, SLOT(AttackCity()));
    attackCity->setEnabled(false);

    attackCity->setShortcut(QKeySequence(Qt::Key_E));

    rangeAttack = new QPushButton("Range Attack");
    connect(rangeAttack, SIGNAL(clicked(bool)), this, SLOT(RangeAttack()));
    rangeAttack->setEnabled(false);
    rangeAttack->setShortcut(QKeySequence(Qt::Key_R));

    fortifyUnit = new QPushButton("Fortify");
    connect(fortifyUnit, SIGNAL(clicked(bool)), this, SLOT(Fortify()));
    fortifyUnit->setEnabled(false);
    fortifyUnit->setShortcut(QKeySequence(Qt::Key_A));

    help = new QPushButton("Help");
    connect(help, SIGNAL(clicked(bool)), this, SLOT(OpenHelp()));
    help->setShortcut(QKeySequence(Qt::Key_Z));

    toggleFoW = new QPushButton("Toggle FoW");
    connect(toggleFoW, SIGNAL(clicked(bool)), this, SLOT(toggleFog()));

}

void GameManager::InitLayouts()
{
    vLayout->setMargin(2);

    unitControlButtons->addWidget(showTechTreeButton);
    unitControlButtons->addWidget(showDiplomacy);
    unitControlButtons->addWidget(toggleFoW);
    unitControlButtons->addSpacing(widget.screenGeometry(widget.primaryScreen()).height() / 1.8f);
    unitControlButtons->addWidget(attackCity);
    unitControlButtons->addWidget(rangeAttack);
    unitControlButtons->addWidget(attackUnit);
    unitControlButtons->addWidget(fortifyUnit);
    unitControlButtons->addWidget(foundCity);
    unitControlButtons->addWidget(buildFarm);
    unitControlButtons->addWidget(buildMine);
    unitControlButtons->addWidget(buildPlantation);
    unitControlButtons->addWidget(buildTradePost);
    unitControlButtons->addWidget(buildRoad);
    unitControlButtons->addWidget(moveUnit);
    unitControlButtons->setGeometry(QRect(0, 20, 100, this->height() - 20));

    gameLayout->addLayout(unitControlButtons);
//    gameLayout->addWidget(cityScreen);
    gameLayout->addWidget(gameView);
    gameLayout->addWidget(techTree);
//    gameLayout->addWidget(diplo);
    gameLayout->addWidget(ns);
    gameLayout->setGeometry(QRect(100, 20, this->width(), this->height()));
    diplo->setGeometry(gameView->pos().x() + 5, gameView->pos().y() + 2, this->width(), this->height());

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);

    playerControlButtons->addWidget(exitGame);
    playerControlButtons->addWidget(help);
    playerControlButtons->addWidget(clv);
    playerControlButtons->addWidget(endGameProgress);
    playerControlButtons->addWidget(frame);
    playerControlButtons->addWidget(techLabel);
    playerControlButtons->addWidget(techText);
    playerControlButtons->addWidget(goldFocus);
    playerControlButtons->addWidget(productionFocus);
    playerControlButtons->addWidget(scienceFocus);
    playerControlButtons->addWidget(foodFocus);
    playerControlButtons->addWidget(cultureFocus);
    playerControlButtons->addWidget(endTurn);
    playerControlButtons->setGeometry(QRect(this->width() - 100, 20, 100, this->height() - 20));

    gameLayout->addLayout(playerControlButtons);

    vLayout->addLayout(gameLayout);
    vLayout->addSpacing(20);
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

    goldText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    prodText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    foodText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    sciText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    culText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    goldPix = new QPixmap("Assets/Icons/gold.png");
    prodPix = new QPixmap("Assets/Icons/production.png");
    foodPix = new QPixmap("Assets/Icons/food.png");
    sciPix = new QPixmap("Assets/Icons/research.png");
    culPix = new QPixmap("Assets/Icons/culture.png");

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

    if(!this->isFullScreen())
    {
        Yieldinfo->addSpacing(1600);
    }
    else
    {
        float space = (static_cast<float>(widget.screenGeometry(widget.primaryScreen()).width()) / 1.2f) * 0.75f;
        Yieldinfo->addSpacing(ceil(space));
    }
    vLayout->insertLayout(0, Yieldinfo);
}

void GameManager::ProcessCityConquer(City *tCity, Civilization *aCiv, Civilization *tCiv)
{
    City* city = new City();
    city->SetName(tCity->GetName());
    city->SetCityStrength(tCity->GetCityStrength());
    city->SetBaseCityStrength(tCity->GetBaseStrength());
    city->SetControllingCiv(aCiv->getCiv());
    city->SetCityHealth(100);
    city->SetCityFocus(Yield::GOLD);
    city->SetCitizenCount(tCity->GetCitizenCount() / 2);

    if(city->GetCitizenCount() < 1)
    {
        city->SetCitizenCount(1);
    }

    if(tCity->IsCityCaptial())
    {
        if(tCity->IsOriginalCapital())
        {
            ns->PostNotification(Notification{0, QString("%1 has lost their Capital! %2 Now controls %3 capitals.").arg(tCiv->GetLeaderName()).arg(aCiv->GetLeaderName()).arg(aCiv->GetCapitalsControlled())});
            aCiv->IncrementCapitalsControlled();
            tCiv->SetCaptialsControlled(0);

            city->SetCityAsCapital(false, true);
        }
        else
        {
            city->SetCityAsCapital(false, false);
        }
    }
    else
    {
         statusMessage = QString("--------<< %1 have conquered the city of %2 >>--------").arg(aCiv->GetLeaderName()).arg(tCity->GetName());
    }

    city->resetAccumulatedProduction();
    city->setCurrentProductionCost(0);
    city->setProductionName("No Current Production");
    city->setProductionIndex(0);
    city->setProductionFinished(false);

    city->SetCityTile(tCity->GetCityTile());
    city->SetMaximumExpansionBorderTiles(tCity->GetMaximumExpansionBorderTiles());

    foreach(Tile* tile, tCity->tileQueue)
    {
        city->tileQueue.push_back(tile);
    }

    city->SortTileQueue();

    foreach(Tile* tile, tCity->GetControlledTiles())
    {
        if(tile->IsWorked)
        {
            tile->IsWorked = false;
        }

        city->AddControlledTile(tile);
    }

    city->SortControlledTiles();

    city->loadUnits("Assets/Units/UnitList.txt");
    city->loadBuildings("Assets/Buildings/BuildingList.txt");

    foreach(Building* building, tCity->getCurrentBuildingList())
    {
        city->addBuilding(building);
    }

    if(tCity->getHasWorker())
    {
        renderer->RemoveUnit(tCity->GetGarrisonedWorker(), gameView);
        tCiv->RemoveUnit(tCity->GetGarrisonedWorker()->GetUnitListIndex());
    }

    if(tCity->HasGarrisonUnit())
    {
        renderer->RemoveUnit(tCity->GetGarrisonedMilitary(), gameView);
        tCiv->RemoveUnit(tCity->GetGarrisonedMilitary()->GetUnitListIndex());
    }

    city->SetFullyExpanded(tCity->IsFullyExpanded());

    city->SetGrowthCost(tCity->GetFoodNeededToGrow());
    city->SetFoodSurplus(tCity->GetFoodSurplus());
    city->SetBuildingStrength(tCity->GetCityBuildingStrength());
    city->UpdateCityStatus();
    city->UpdateCityYield();

    city->SetCityBorders(tCity->GetCityBorders());
    city->setCapitolConnection(false);
    city->SetCityIndex(aCiv->GetCityList().size());
    city->SetCityRenderIndex(tCity->GetCityRenderIndex());

    renderer->RemoveCity(tCity, gameView);
    tCiv->RemoveCity(tCity->GetCityIndex());

    city->InitializeCity();
    renderer->AddCity(city, gameView, true);
    aCiv->AddCity(city);

    clv->addItem(city->GetName());
}

void GameManager::ProcessAttackUnit()
{
    state = IDLE;

    if(unitToMove->isFortified)
    {
        unitToMove->isFortified = false;
        renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
    }

    targetUnit = uc->FindUnitAtTile(targetTile, civList.at(targetTile->GetControllingCivListIndex())->GetUnitList());

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

    selectedTileQueue->enqueue(SelectData{targetUnit->GetTileIndex(), false, false});

    attackUnit->setEnabled(false);
    rangeAttack->setEnabled(false);

    if(map->GetTileAt(unitToMove->GetTileIndex())->GetTileType() == WATER)
        uc->Attack(unitToMove, targetUnit, true);
    else
        uc->Attack(unitToMove, targetUnit, false);

    renderer->UpdateUnits(map, gameView, unitToMove, false);
    renderer->UpdateUnits(map, gameView, targetUnit, false);

    renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);

    this->redrawTile = true;
}

void GameManager::ProcessPeace(int makePeaceWithIndex)
{
    if(AcceptsPeace(civList.at(makePeaceWithIndex)))
    {
        civList.at(currentTurn)->MakePeace(makePeaceWithIndex);
        civList.at(makePeaceWithIndex)->MakePeace(currentTurn);

        // Move player units outside the enemy's borders
        foreach(City* city, civList.at(makePeaceWithIndex)->GetCityList())
        {
            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->ContainsUnit && tile->GetOccupyingCivListIndex() == 0)
                {
                    Unit *unit = uc->FindUnitAtTile(tile, civList.at(currentTurn)->GetUnitList());

                    foreach(Tile* outside, city->tileQueue)
                    {
                        if(!outside->ContainsUnit)
                        {
                            map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
                            map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);

                            if(map->GetTileAt(unit->GetTileIndex())->Selected)
                                    map->GetTileAt(unit->GetTileIndex())->Selected = false;

                            //update the unit's position
                            unit->SetPositionIndex(outside->GetTileIndex());
                            unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
                            map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(currentTurn);

                            // Set the data for the unit's new tile
                            map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

                            unit->RequiresOrders = true;
                            renderer->UpdateUnits(map, gameView, unit, true);
                            renderer->SetUnitNeedsOrders(unit->GetTileIndex(), unit->RequiresOrders);
                            break;
                        }
                    }
                }
            }

            foreach(Unit *aiUnit, civList.at(makePeaceWithIndex)->GetUnitList())
            {
                if(!aiUnit->isPathEmpty())
                {
                    while(!aiUnit->isPathEmpty())
                    {
                        aiUnit->UpdatePath();
                    }

                    aiUnit->RequiresOrders = true;
                }
            }
        }

        // Move enemy units outside the players borders
        foreach(City* city, civList.at(currentTurn)->GetCityList())
        {
            foreach(Tile* tile, city->GetControlledTiles())
            {
                if(tile->ContainsUnit && tile->GetOccupyingCivListIndex() == makePeaceWithIndex)
                {
                    Unit *unit = uc->FindUnitAtTile(tile, civList.at(makePeaceWithIndex)->GetUnitList());

                    if(unit->isPathEmpty())
                    {
                        foreach(Tile* outside, city->tileQueue)
                        {
                            if(!outside->ContainsUnit)
                            {
                                map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
                                map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);

                                if(map->GetTileAt(unit->GetTileIndex())->Selected)
                                        map->GetTileAt(unit->GetTileIndex())->Selected = false;

                                //update the unit's position
                                unit->SetPositionIndex(outside->GetTileIndex());
                                unit->SetPosition(tile->GetTileID().column, tile->GetTileID().row);
                                map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(makePeaceWithIndex);

                                // Set the data for the unit's new tile
                                map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

                                unit->RequiresOrders = true;
                                renderer->UpdateUnits(map, gameView, unit, true);
                                break;
                            }
                        }
                    }
                    else
                    {
                        uc->MoveUnit(unit, map, currentTurn);
                        renderer->UpdateUnits(map, gameView, unit, true);
                    }
                }
            }
        }

        diplo->MakePeaceWith(civList.at(currentTurn)->getCiv(), makePeaceWithIndex, civList.at(makePeaceWithIndex)->getCiv());

        QMessageBox *mbox = new QMessageBox();
        mbox->setText(QString("%1 has ACCEPTED your offerings of peace of friendship.").arg(civList.at(makePeaceWithIndex)->GetLeaderName()));
        mbox->setStyleSheet(warStyle);
        mbox->exec();
        delete mbox;

        diplo->makePeace->setEnabled(false);

        ns->PostNotification(Notification{6, QString("%1 has made peace with %2").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(makePeaceWithIndex)->GetLeaderName())});

        this->redrawTile = true;
    }
    else
    {
        if(diplo->AtPermanentWar(civList.at(makePeaceWithIndex)->getCiv()))
        {
            QMessageBox *mbox = new QMessageBox();
            mbox->setText(QString("%1 is tired of your lies about peace and friendship\nand has come to the conclusion that your differences are to be settled\nin a fight to the death.").arg(civList.at(makePeaceWithIndex)->GetLeaderName()));
            mbox->setStyleSheet(warStyle);
            mbox->exec();
            delete mbox;
        }
        else
        {
            QMessageBox *mbox = new QMessageBox();
            mbox->setText(QString("%1 has REJECTED your offerings of peace of friendship.").arg(civList.at(makePeaceWithIndex)->GetLeaderName()));
            mbox->setStyleSheet(warStyle);
            mbox->exec();
            delete mbox;
            diplo->makePeace->setEnabled(false);
        }
    }
}

/*
 * AcceptsPeace is used to deterine if the AI will accept a peace treaty.
 * The passed param should be an AI Civ since peace can only be offered by
 * the player (for now). This function returns false if the AI and player
 * are at permanent war, or if the AI rejects the peace treaty.
 */
bool GameManager::AcceptsPeace(Civilization *ai)
{
    bool accepts = false;
    int turnsAtWar = diplo->GetLengthOfWar(ai->getCiv());
    int timesAtWar = diplo->GetNumberOfWars(ai->getCiv());

    if(!diplo->AtPermanentWar(ai->getCiv()))
    {
        if((turnsAtWar - 10) >= 0)
        {
            double chanceToAccept = ((static_cast<double>(civList.at(0)->GetMilitaryStrength()) * (turnsAtWar - 10)) / (ai->GetMilitaryStrength() * (2 * timesAtWar)));

            double weights[] =
            {
                (1.0 - chanceToAccept), // Will not accept Peace
                (chanceToAccept)        // Will accept Peace
            };

            std::mt19937 gen(qrand());
            std::discrete_distribution<> d(std::begin(weights), std::end(weights));

            int chance = d(gen);

            if(chance == 0)
            {
                accepts = false;
            }
            else if(chance == 1)
            {
                accepts = true;
            }
        }
    }

    return accepts;
}

void GameManager::closeGame()
{
    QFile civSaveFile("Saves/latest_civs.json");
    if(!civSaveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open civ save file");
        this->close();
        return;
    }

    QJsonDocument doc;
    QJsonObject civs;
    QJsonArray civArray;
    for(int i = 0; i < civList.size(); i++)
    {
        QJsonObject civObject;
        civList.at(i)->WriteData(civObject);
        civArray.push_back(civObject);
    }

    civs["civilizations"] = civArray;
    doc.setObject(civs);
    civSaveFile.write(doc.toJson());
    civSaveFile.flush();
    civSaveFile.close();

    QFile mapSaveFile("Saves/latest_map.json");
    if(!mapSaveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open map save file");
        this->close();
        return;
    }

    QJsonObject mapObject;
    map->WriteMapSaveData(mapObject);
    doc.setObject(mapObject);

    mapSaveFile.write(doc.toJson());
    mapSaveFile.flush();
    mapSaveFile.close();

    QFile diploSaveFile("Saves/latest_diplo.json");
    if(!diploSaveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open map save file");
        this->close();
        return;
    }

    QJsonObject diploObj;
    diplo->WriteDiploSaveData(diploObj);
    doc.setObject(diploObj);

    diploSaveFile.write(doc.toJson());
    diploSaveFile.flush();
    diploSaveFile.close();

    QFile managerSaveFile("Saves/latest_manager.json");
    if(!managerSaveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open manager save file");
        this->close();
        return;
    }

    QJsonObject gmObj;
    gmObj["currentturn"] = currentTurn;
    gmObj["gameturn"] = gameTurn;
    gmObj["state"] = state;

    doc.setObject(gmObj);
    managerSaveFile.write(doc.toJson());
    managerSaveFile.flush();
    managerSaveFile.close();

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

void GameManager::showCity(City* city)
{
    if(!cityScreenVisible)
    {
        if(cityScreen != NULL)
        {
            delete cityScreen;
        }
        cityScreen = new CityScreen(this);
        cityScreen->loadBuildings("Assets/Buildings/BuildingList.txt");
        cityScreen->loadUnits("Assets/Units/UnitList.txt");
        cityScreen->getCityInfo(city);
        cityScreen->getCivInfo(civList.at(0));
        cityScreen->getGameView(gameView);
        cityScreen->getMapInfo(map);
        cityScreen->getRenderer(renderer);
        cityScreen->getGold(civList.at(0)->GetTotalGold());
        cityScreen->updateList(city->getNumberOfBuildings());
        cityScreen->updateWidget();

        gameView->centerOn(city->GetCityTile()->GetCenter());

        cityScreen->setGeometry(gameView->pos().x() + 5, gameView->pos().y() + 2, gameView->width(), gameView->height());

        gameView->setDragMode(QGraphicsView::NoDrag);

        cityScreen->show();
        cityScreenVisible = true;
    }
    else
    {
        this->goldFocus->setEnabled(false);
        this->productionFocus->setEnabled(false);
        this->scienceFocus->setEnabled(false);
        this->cultureFocus->setEnabled(false);
        this->foodFocus->setEnabled(false);
        this->showTechTreeButton->setEnabled(true);

        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        cityScreenVisible = false;
        renderer->UpdateCityProductionBar(civList.at(0)->GetCityAt(0), gameView);
    }
}

/*
 * updateTiles() is run by the updateTimer every 50 ms
 * this goes to the gameView to see if the player has
 * provided any new mouse inputs and then processes the data
 * appropriately. This is also where TurnController() is called from
 * as well as End Game control and where the notification is shown if
 * there are new notifications for the player.
 */
void GameManager::updateTiles()
{
    processedData = gameView->GetScene()->ProcessTile(relocateUnit);

    if(processedData.newData || state == FOUND_CITY)
    {
        this->UpdateTileData();
    }

    TurnController();

    if(fortify && unitToMove != NULL && !unitToMove->isFortified)
    {
        fortify = false;
        unitToMove->isFortified = true;
        unitToMove->RequiresOrders = false;
        renderer->SetFortifyIcon(unitToMove->GetTileIndex(), false);
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);
    }

    if(this->redrawTile && !selectedTileQueue->isEmpty())
    {
        this->redrawTile = false;
        renderer->UpdateScene(map, gameView, selectedTileQueue);
    }

    if(!viewUpdateTiles->isEmpty())
    {
        renderer->UpdateTileVisibilty(viewUpdateTiles, gameView);
    }

    this->update();

    if(this->currentTurn == 0 && !civList.at(currentTurn)->alive && this->playersAliveCount == 1)
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setWindowFlags(Qt::FramelessWindowHint);
        mBox->setFixedSize(200, 500);
        mBox->setText("You Lose!");
        mBox->exec();
        this->playersAliveCount=0;
        //Defeat screen?
        this->closeGame();
    }
    else if(this->currentTurn == 0 && this->playersAliveCount == 1)
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setWindowFlags(Qt::FramelessWindowHint);
        mBox->setFixedSize(200, 500);
        mBox->setText("You Win!");
        mBox->exec();
        this->playersAliveCount = 0;
        this->closeGame();
    }

    if(!ns->HasNotificationsWaiting() && ns->item(0) == NULL)
    {
        ns->hide();
    }
    else if(ns->HasNotificationsWaiting())
    {
        if(ns->isHidden())
            ns->show();

        ns->ShowNotifications();
    }

    if(cityScreen != NULL && cityScreen->isHidden())
    {
        cityScreenVisible = true;
        showCity(NULL);
    }

    end = std::chrono::steady_clock::now();
    if(countTime == true)
    {
        countTime = false;
        qDebug() << "------------------ Time to process AI Turns:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us";
    }
}

void GameManager::moveUnitTo()
{
    if(moveUnit->isEnabled())
    {
        state = MOVE_UNIT;
        relocateUnit = true;
    }
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
        techTree->loadData(civList.at(0)->getCurrentTech(),civList.at(0)->getNextTech(),civList.at(0)->getAccumulatedScience());
        techTree->loadTechList("Assets/Techs/Technology.txt");
        techTree->updateWidget(civList.at(0)->getNextTech()->getIndex()+1);
        techTree->setGeometry(gameView->pos().x() + 2, gameView->pos().y() + 3, gameView->width() - 6, gameView->height() - 150);
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

void GameManager::toggleDiplomacy()
{
    if(!diploVisible)
    {
        diplo->setGeometry(gameView->pos().x() + 5, gameView->pos().y() + 2, this->width(), this->height());
        diplo->show();
        diploVisible = true;
    }
    else
    {
        diplo->hide();
        diplo->UpdateLeader();
        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        diploVisible = false;
    }
}

void GameManager::foundNewCity()
{
    state = FOUND_CITY;
}

void GameManager::buildNewRoad()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),ROAD))
    {
        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
}

void GameManager::buildNewFarm()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),FARM))
    {
        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(FARM, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewPlantation()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),PLANTATION))
    {
        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(PLANTATION, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewTradePost()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),TRADE_POST))
    {
        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(TRADE_POST, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
    }
    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewMine()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),MINE))
    {
        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(MINE, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::attackMelee()
{
    state = ATTACK_MELEE;
}

void GameManager::SetGoldFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(Yield::GOLD);
    statusMessage = QString("--------<< You have changed %1's focus to gold >>--------").arg(civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetName());

    foreach(Tile* t, civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetControlledTiles())
    {
        renderer->SetTileWorkedIcon(t, gameView);
    }

    this->focusChanged = true;
}

void GameManager::SetProdFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(Yield::PRODUCTION);
    statusMessage = QString("--------<< You have changed %1's focus to production >>--------").arg(civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetName());

    foreach(Tile* t, civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetControlledTiles())
    {
        renderer->SetTileWorkedIcon(t, gameView);
    }

    this->focusChanged = true;
}

void GameManager::SetScienceFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(Yield::RESEARCH);
    statusMessage = QString("--------<< You have changed %1's focus to science >>--------").arg(civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetName());

    foreach(Tile* t, civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetControlledTiles())
    {
        renderer->SetTileWorkedIcon(t, gameView);
    }

    this->focusChanged = true;
}

void GameManager::SetFoodFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(Yield::FOOD);
    statusMessage = QString("--------<< You have changed %1's focus to food >>--------").arg(civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetName());

    foreach(Tile* t, civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetControlledTiles())
    {
        renderer->SetTileWorkedIcon(t, gameView);
    }

    this->focusChanged = true;
}

void GameManager::SetCultureFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(Yield::CULTURE);
    statusMessage = QString("--------<< You have changed %1's focus to culture >>--------").arg(civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetName());

    foreach(Tile* t, civList.at(currentTurn)->GetCityAt(clv->currentRow())->GetControlledTiles())
    {
        renderer->SetTileWorkedIcon(t, gameView);
    }

    this->focusChanged = true;
}

void GameManager::AttackCity()
{
    state = ATTACK_CITY;
}

void GameManager::RangeAttack()
{
    state = ATTACK_RANGE;
}

void GameManager::Fortify()
{
    this->fortify = true;
    fortifyUnit->setEnabled(false);
}

/*
 * WarDecalred is used when the player goes to attack
 * a unit they are not at war with.
 */
void GameManager::WarDeclared()
{
    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetTile->GetControllingCivListIndex())->GetLeaderName())});

    if(targetTile->HasCity)
    {
        state = FIND_CITY;
    }
    else if(targetTile->ContainsUnit)
    {
        state = IDLE;
        ProcessAttackUnit();
    }

    civList.at(currentTurn)->SetAtWar(targetTile->GetControllingCivListIndex());
    civList.at(targetTile->GetOccupyingCivListIndex())->SetAtWar(currentTurn);

    diplo->DeclareWarOn(civList.at(targetTile->GetOccupyingCivListIndex())->getCiv(), targetTile->GetOccupyingCivListIndex(), civList.at(0)->getCiv());
}

void GameManager::WarAvoided()
{
    state = IDLE;
    relocateUnit = false;
}

/*
 * WarByInvasion is used when the player moves into
 * territory controlled by an AI.
 */
void GameManager::WarByInvasion()
{
    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetTile->GetControllingCivListIndex())->GetLeaderName())});

    state = INVADE;
    civList.at(currentTurn)->SetAtWar(targetTile->GetControllingCivListIndex());
    civList.at(targetTile->GetControllingCivListIndex())->SetAtWar(currentTurn);

    diplo->DeclareWarOn(civList.at(targetTile->GetControllingCivListIndex())->getCiv(), targetTile->GetControllingCivListIndex(), civList.at(0)->getCiv());
}

void GameManager::OpenHelp()
{
    about->show();
}


/*
 * Parse item is used when the player selects
 * a city they wish to manage.
 */
void GameManager::parseItem()
{
    this->goldFocus->setEnabled(true);
    this->productionFocus->setEnabled(true);
    this->scienceFocus->setEnabled(true);
    this->cultureFocus->setEnabled(true);
    this->foodFocus->setEnabled(true);

    this->showTechTreeButton->setEnabled(false);
    this->showCity(civList[0]->GetCityAt(clv->currentRow()));
}

void GameManager::toggleFog()
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(!toggleOn)
        {
            renderer->SetTileVisibility(i, true, false);
        }
        else
        {
            if(map->GetTileAt(i)->DiscoveredByPlayer)
            {
                if(map->GetTileAt(i)->CanAlwaysBeSeen)
                {
                    renderer->SetTileVisibility(i, true, false);
                }
                else if(!map->GetTileAt(i)->IsSeenByPlayer)
                {
                    renderer->SetTileVisibility(i, false, false);
                }
            }
            else
            {
                if(map->GetTileAt(i)->CanAlwaysBeSeen)
                {
                    renderer->SetTileVisibility(i, true, false);
                }
                else
                {
                    renderer->SetTileVisibility(i, false, true);
                }
            }
        }
    }

    toggleOn = !toggleOn;
}

