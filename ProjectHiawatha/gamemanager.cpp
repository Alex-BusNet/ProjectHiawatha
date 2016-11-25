#include "gamemanager.h"
#include <QDebug>
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QThread>
#include <ctime>
#include <algorithm>
#include <QtConcurrent/QtConcurrent>
#include "unittype.h"
#include "datatypes.h"

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player, int numAI) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    gameView = new GameView(this, fullscreen);
    ac = new AI_Controller();
    clv = new QListWidget(this);

    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    gameLayout = new QHBoxLayout();
    playerControlButtons = new QVBoxLayout();
    unitControlButtons = new QVBoxLayout();
    Yieldinfo = new QHBoxLayout();

    cityScreen = new CityScreen(this);
    techTree = new TechTree(this);

    selectedTileQueue = new QQueue<SelectData>();
    tileModifiedQueue = new QQueue<SelectData>();

    techLabel = new QLabel(" NO RESEARCH ");
    techText = new QLabel(" 00/000 ");
    endGameProgress = new QLabel("Capitals Controlled:");
    endGameText = new QString(" 0/0 ");

    unitToMove = NULL;
    targetUnit = NULL;
    targetCity = NULL;

    cityScreenVisible = false;
    techTreeVisible = false;
    relocateUnit = false;
    turnEnded = false;
    turnStarted = true;
    countTime = false;
    citySelected = false;
    findUnit = false;
    attackNearby = false;
    focusChanged = false;
    fortify = false;
    attackEnemyCity = false;
    findCity = false;
    foundACity = false;
    aiFoundCity = false;
    attackRange = false;

    currentProductionName = "No Production Selected";
    playerCiv = player;

    if(!fullscreen)
    {
        this->setFixedSize(1400, 700);
        this->setWindowTitle("Project Hiawatha");
    }
    else
    {
        this->setWindowState(Qt::WindowFullScreen);
    }

    gameView->ConfigureGraphicsView();


    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTiles()));
    updateTimer->start();

    this->InitButtons();
    this->InitLayouts();
    this->setLayout(vLayout);
    this->show();

    qDebug() << "Creating new Renderer";
    renderer = new Renderer(mapSizeX);

    qDebug() << "Done.\nInitializing Map.";
    map = new Map(mapSizeX, mapSizeY);
    mapInit = QtConcurrent::run(this->map, Map::InitHexMap);

    qDebug() << "Initializing Civs";
    civInit = QtConcurrent::run(this, GameManager::InitCivs, player, numAI);
    civInit.waitForFinished();

    delete &civInit;
    delete &mapInit;

    this->playersAliveCount = civList.size();

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(*map, gameView);

    qDebug() << "Done.\nDrawing Cities, Borders, and Units.";
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
                }
            }

            civList.at(i)->GetCityAt(j)->loadUnits("../ProjectHiawatha/Assets/Units/UnitList.txt");
            civList.at(i)->GetCityAt(j)->loadBuildings("../ProjectHiawatha/Assets/Buildings/BuildingList.txt");
        }

        if(i == 0)
        {
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

    qDebug() << "Init yield";

    InitYieldDisplay();

//    renderer->DrawGuiText(map, stringData, gameView);
    zoomScale = 1;

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

    playerInfoRect = new QRect(0, 0, this->width(), 20);


    qDebug() << "Screen size:" << gameView->width() << gameView->height();
    qDebug() << "Scene size: " << gameView->GetScene()->sceneRect().width() << "x" << gameView->GetScene()->sceneRect().height();

    qDebug() << "Done.";
}

void GameManager::InitCivs(Nation player, int numAI)
{
    Civilization* civ = new Civilization(player, false);
    civ->loadTechs("../ProjectHiawatha/Assets/Techs/Technology.txt");
    civ->setCurrentTech(civ->GetTechList().at(0));
    civ->setNextTech(civ->GetTechList().at(1));
    civ->setCivIndex(0);
    techLabel->setText(QString(" %1 ").arg(civ->getCurrentTech()->getName()));
    QString str = "../ProjectHiawatha/Assets/CityLists/";
    QString str2;
    switch (player)
    {
    case America:
        str2 = "america.txt";
        civ->SetLeaderName(QString("Washington"));
        break;
    case Germany:
        str2 = "germany.txt";
        civ->SetLeaderName(QString("Bismark"));
        break;
    case India:
        str2 = "india.txt";
        civ->SetLeaderName(QString("Gandhi"));
        break;
    case China:
        str2 = "china.txt";
        civ->SetLeaderName(QString("Zedong"));
        break;
    case Mongolia:
        str2 = "mongolia.txt";
        civ->SetLeaderName(QString("Genghis Khan"));
        break;
    case Aztec:
        str2 = "aztec.txt";
        civ->SetLeaderName(QString("Montezuma"));
        break;
    case France:
        str2 = "france.txt";
        civ->SetLeaderName(QString("Napoleon"));
        break;
    case Iroquois:
        str2 = "iroquois.txt";
        civ->SetLeaderName(QString("Hiawatha"));
        break;
    case Greece:
        str2 = "greece.txt";
        civ->SetLeaderName(QString("Alexander"));
        break;
    case Rome:
        str2 = "rome.txt";
        civ->SetLeaderName(QString("Ceasar"));
        break;
    case England:
        str2 = "england.txt";
        civ->SetLeaderName(QString("Elizabeth"));
        break;
    case Arabia:
        str2 = "arabia.txt";
        civ->SetLeaderName(QString("al-Rashid"));
        break;
    case Persia:
        str2 = "persia.txt";
        civ->SetLeaderName(QString("Cyrus"));
        break;
    case Russia:
        str2 = "russia.txt";
        civ->SetLeaderName(QString("Stalin"));
        break;
    case Japan:
        str2 = "japan.txt";
        civ->SetLeaderName(QString("Nobunga"));
        break;
    case Egypt:
        str2= "egypt.txt";
        civ->SetLeaderName(QString("Ramesses"));
        break;
    default:
        str2 = "india.txt";
        civ->SetLeaderName(QString("Gandhi"));
        break;
    }
    str = str + str2;
    qDebug()<<str;
    civ->loadCities(str);
    civList.push_back(civ);

    srand(time(0));
    int civNum;
    bool found;
    QVector<int> selNat;

    selNat.push_back(player);

    qDebug() << "   Player:" << player;
    for(int i = 0; i < numAI; i++)
    {
newCivRand:
        // The modulo number at the end indicates the
        // max number of civs in the game.
        civNum = rand() % 16;

        if(civNum != player)
        {
            foreach(int j, selNat)
            {
                if(j == civNum)
                {
                    qDebug() << "civ exists";
                    found = true;
                    break;
                }
            }

            switch (civNum)
            {
            case America:
                civ = new Civilization(America, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/america.txt");
                civ->SetLeaderName(QString("Washington"));
                selNat.push_back(civNum);
                break;
            case Germany:
                civ = new Civilization(Germany, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/germany.txt");
                civ->SetLeaderName(QString("Bismark"));
                selNat.push_back(civNum);
                break;
            case India:
                civ = new Civilization(India, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/india.txt");
                civ->SetLeaderName(QString("Gandhi"));
                selNat.push_back(civNum);
                break;
            case China:
                civ = new Civilization(China, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/china.txt");
                civ->SetLeaderName(QString("Zedong"));
                selNat.push_back(civNum);
                break;
            case Mongolia:
                civ = new Civilization(Mongolia, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/mongolia.txt");
                civ->SetLeaderName(QString("Genghis Khan"));
                selNat.push_back(civNum);
                break;
            case Aztec:
                civ = new Civilization(Aztec, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/aztec.txt");
                civ->SetLeaderName(QString("Montezuma"));
                selNat.push_back(civNum);
                break;
            case France:
                civ = new Civilization(France, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/france.txt");
                civ->SetLeaderName(QString("Napoleon"));
                selNat.push_back(civNum);
                break;
            case Iroquois:
                civ = new Civilization(Iroquois, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/iroquois.txt");
                civ->SetLeaderName(QString("Hiawatha"));
                selNat.push_back(Iroquois);
                break;
            case Greece:
                civ = new Civilization(Greece, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/greece.txt");
                civ->SetLeaderName(QString("Alexander"));
                selNat.push_back(civNum);
                break;
            case Rome:
                civ = new Civilization(Rome, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/rome.txt");
                civ->SetLeaderName(QString("Ceasar"));
                selNat.push_back(civNum);
                break;
            case England:
                civ = new Civilization(England, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/england.txt");
                civ->SetLeaderName(QString("Elizabeth"));
                selNat.push_back(civNum);
                break;
            case Arabia:
                civ = new Civilization(Arabia, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/arabia.txt");
                civ->SetLeaderName(QString("al-Rashid"));
                selNat.push_back(civNum);
                break;
            case Persia:
                civ = new Civilization(Persia, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/persia.txt");
                civ->SetLeaderName(QString("Cyrus"));
                selNat.push_back(civNum);
                break;
            case Russia:
                civ = new Civilization(Russia, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/russia.txt");
                civ->SetLeaderName(QString("Stalin"));
                selNat.push_back(civNum);
                break;
            case Japan:
                civ = new Civilization(Japan, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/japan.txt");
                civ->SetLeaderName(QString("Nobunga"));
                selNat.push_back(civNum);
                break;
            case Egypt:
                civ = new Civilization(Egypt, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/egypt.txt");
                civ->SetLeaderName(QString("Ramesses"));
                selNat.push_back(civNum);
                break;
            default:
                //Always default to Ghandi.
                civ = new Civilization(India, true);
                civ->loadCities("../ProjectHiawatha/Assets/CityLists/india.txt");
                civ->SetLeaderName(QString("Gandhi"));
                selNat.push_back(civNum);
                break;
            }

            // look to see if the selected civ has already been selected

            // If was not found, place in civList vector
            if(!found)
            {
                qDebug() << "   Civ" << i << ": " << civ->getCiv();
                civ->loadTechs("../ProjectHiawatha/Assets/Techs/Technology.txt");
                civ->setCurrentTech(civ->GetTechList().at(0));
                civ->setNextTech(civ->GetTechList().at(1));
                civ->setCivIndex(i+1);
                civList.push_back(civ);
            }
            // Otherwise, delete it and try again.
            else
            {
                found = false;
                qDebug() << "--Civ" << civ->getCiv() << "exists. Looking for new Civ";
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
        qDebug() << "--Waiting for map";
        mapInit.waitForFinished();
    }

    qDebug() << "   Spawning Civs";
    map->SpawnCivs(civList);
}

void GameManager::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter paint(this);

    paint.fillRect(*playerInfoRect, QBrush(Qt::black));
    paint.setPen(Qt::white);
    paint.drawText(*playerInfoRect, (Qt::AlignRight | Qt::AlignVCenter), QString("Turn %1 | %2 %3  ").arg(gameTurn).arg(abs(year)).arg((year < 0) ? "BC" : "AD"));
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
        else if(turnStarted)
        {
            qDebug() << "Player's turn started";
            turnStarted = false;
            StartTurn();
        }
    }
    else
    {
        StartTurn();
        qDebug() << "running AI";
        QFuture<void> future = QtConcurrent::run(this->ac, AI_Controller::turnStarted, civList.at(currentTurn), civList.at(0), this->map);
//        future.waitForFinished();
        while(future.isRunning())
        {
//            qDebug() << "------Manager reading queue";
            if(!civList.at(currentTurn)->isEmpty())
            {
                qDebug() << "Queue size:" << civList.at(currentTurn)->queueSize();
                qDebug() << "CurrentTurn:" << currentTurn;
                unitToMove = civList.at(currentTurn)->dequeue();
                qDebug() << "Founding index:" << unitToMove->GetTileIndex();

                qDebug() << "Is list empty:" << civList.at(currentTurn)->isEmpty();
                qDebug() << "Unit type:" << unitToMove->GetName();
                qDebug() << "Owner:" << unitToMove->GetOwner();

                if(unitToMove->GetUnitType() == SETTLER)
                {
                    qDebug() << "--AI founded city at" << unitToMove->GetTileIndex();
                    aiFoundCity = true;
                    this->UpdateTileData();
                    break;
                }
            }
        }
        qDebug() << "Waiting for finish";
        future.waitForFinished();
        qDebug() << "   AI Turn Finished";
        EndTurn();
    }

}

void GameManager::StartTurn()
{
    for(int i = 0; i < civList.size(); i++)
    {
        if(i == 0)
        {
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

    Update_t update = civList.at(currentTurn)->UpdateProgress();

    foreach(City* city, civList.at(currentTurn)->GetCityList())
    {
        city->FilterMEBList();
    }

    if(update.updateBorders)
    {
        foreach(City* city, civList.at(currentTurn)->GetCityList())
        {
            map->GetTileQueue(city);
            city->SortTileQueue();
            renderer->UpdateCityBorders(city, gameView, civList.at(currentTurn)->getCiv());
            foreach(Tile* tile, city->GetControlledTiles())
            {
                renderer->SetTileWorkedIcon(tile, gameView);
            }
        }
    }

    if(update.updateCitizens || this->focusChanged)
    {
        if(this->focusChanged == true)
        {
            this->focusChanged = false;
        }

        foreach(City* city, civList.at(currentTurn)->GetCityList())
        {
            qDebug() << "----Updating tiles worked";
            foreach(Tile* tile, city->GetControlledTiles())
            {
                renderer->SetTileWorkedIcon(tile, gameView);
            }
        }
    }

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
    }

    int localIndex = 0;
    QString str[20];

    int scienceYield = civList.at(currentTurn)->getCivYield()->GetScienceYield();
    civList.at(currentTurn)->setAccumulatedScience(scienceYield);

    int accumulatedScience = civList.at(currentTurn)->getAccumulatedScience();

    int techCost = civList.at(currentTurn)->getCurrentTech()->getCost();

    if(accumulatedScience >= techCost)
    {
        if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv())
        {
            QMessageBox* mBox = new QMessageBox();
            mBox->setText("Tech has finished");
            mBox->exec();
            qDebug()<<"Tech finished";
        }

        civList.at(currentTurn)->setTechIndex();
        int techIndex = civList.at(currentTurn)->getTechIndex();
        for(int i = 0; i<civList.at(currentTurn)->GetCityList().size();i++)
        {
            for(int j = 0; j<civList.at(currentTurn)->GetCityList().at(i)->getInitialUnitList().size();j++)
            {
                if(civList.at(currentTurn)->GetCityList().at(0)->getInitialUnitList().at(j)->GetTechIndex() == (techIndex-1))
                {
                    qDebug() << "UNLOCKING: " << civList.at(currentTurn)->GetCityList().at(i)->getInitialUnitList().at(j)->GetName();
                    civList.at(currentTurn)->GetCityList().at(i)->getInitialUnitList().at(j)->setUnlocked(1);
                }  
            }

            for(int k = 0; k<civList.at(currentTurn)->GetCityList().at(i)->getInitialBuildingList().size();k++)
            {
                if(civList.at(currentTurn)->GetCityList().at(0)->getInitialBuildingList().at(k)->getTechIndex() == (techIndex - 1))
                {
                    qDebug() << "UNLOCKING: " << civList.at(currentTurn)->GetCityList().at(i)->getInitialBuildingList().at(k)->getName();
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
    }

    int accumulatedProduction;
    int productionCost;
    for(int i = 0; i<civList.at(currentTurn)->GetCityList().size();i++)
    {
        qDebug()<<civList.at(currentTurn)->GetCityAt(i)->GetName();
        productionCost = civList.at(currentTurn)->GetCityList().at(i)->getCurrentProductionCost();
        accumulatedProduction = civList.at(currentTurn)->GetCityList().at(i)->getAccumulatedProduction();
        qDebug()<<"ACCUMULATED PRODUCTION: "<<accumulatedProduction;
        qDebug()<<"PRODUCTION COST: "<<productionCost;

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
                    qDebug()<<"Unit #"<<civList.at(currentTurn)->GetCityAt(i)->getProductionIndex();
                    //unit->SetName(civList.at(currentTurn)->GetCityAt(i)->getInitialUnitList().at(civList.at(currentTurn)->GetCityAt(i)->getProductionIndex())->GetName());

                    qDebug()<<civList.at(currentTurn)->GetCityAt(i)->getInitialUnitList().at(civList.at(currentTurn)->GetCityAt(i)->getProductionIndex())->GetCost();
                    unit->SetOwner(civList.at(currentTurn)->getCiv());
                    unit->SetUnitListIndex(civList.at(currentTurn)->GetUnitList().size());

                    for(int i = 0; i<map->GetBoardSize();i++)
                    {
                        if(unit->isNaval())
                        {
                            if(map->GetTileAt(i)->ContainsUnit  || !(map->GetTileTypeAt(i) == WATER)) { continue; }
                            else
                            {
                                if(civList.at(currentTurn)->getCiv() == map->GetTileAt(i)->GetControllingCiv())
                                {
                                    unit->SetPositionIndex(i);
                                    map->GetTileAt(i)->ContainsUnit = true;
                                    qDebug()<<"         Naval Unit built"<<unit->GetName();
                                    break;
                                }

                            }
                        }
                        else
                        {
                            if(map->GetTileAt(i)->ContainsUnit || !(map->GetTileAt(i)->Walkable) || (map->GetTileTypeAt(i) == WATER)) { continue; }
                            else
                            {
                                if(civList.at(currentTurn)->getCiv() == map->GetTileAt(i)->GetControllingCiv())
                                {
                                    unit->SetPositionIndex(i);
                                    map->GetTileAt(i)->ContainsUnit = true;
                                    qDebug()<<"         Land Unit built";
                                    qDebug()<<unit->GetName();
                                    break;
                                }
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
                    civList.at(0)->GetCityList().at(i)->GetCityTile()->SetYield(gold,production,science,food,culture);

                    civList.at(currentTurn)->GetCityList().at(i)->addBuilding(building);
                }

                if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv() && update.productionFinished)
                {
                    QString str2;
                    int numOfCities = 1;
                    for(int j = 0;j<20;j++)
                    {
                        if(str[j].isEmpty()) { continue; }
                        else
                        {
                            QString str3 = str[j];
                            if(numOfCities > 1){
                                str3+= ", ";
                            }
                            str2 += str3;
                            numOfCities++;
                        }

                    }
                    QString prodString = "Production has finished in: ";
                    QString finalString = prodString+str2;
                    QMessageBox* mBox = new QMessageBox();
                    mBox->setText(finalString);
                    mBox->exec();
                    qDebug()<<"Production finished";
                }
            }
        }

        foreach(Unit* unit, civList.at(currentTurn)->GetUnitList())
        {
            if(!unit->RequiresOrders && unit->isPathEmpty())
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
        }

        if(update.cityHealed)
        {
            renderer->UpdateCityHealthBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        }

        renderer->UpdateCityProductionBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        renderer->UpdateCityGrowthBar(civList.at(currentTurn)->GetCityAt(i), gameView);

        if(currentTurn == 0)
        {
            goldText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalGold()).arg(civList.at(0)->getCivYield()->GetGoldYield()));
            prodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetProductionYield()));
            foodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetFoodYield()));
            sciText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalScience()).arg(civList.at(0)->getCivYield()->GetScienceYield()));
            culText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalCulture()).arg(civList.at(0)->getCivYield()->GetCultureYield()));
            techText->setText(QString(" %1 / %2").arg(accumulatedScience).arg(techCost));
        }

        qDebug() << "  Starting turn for civ" << currentTurn;
    }
}

void GameManager::EndTurn()
{
    qDebug() << "Ending Turn";
    countTime = true;
    begin = std::chrono::steady_clock::now();
    bool unitMoved = false;

    if(currentTurn == 0)
    {
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
    }

    for(int i = 0; i < civList.at(currentTurn)->GetUnitList().size(); i++)
    {
        qDebug() << "           is unit path empty:" << civList.at(currentTurn)->GetUnitAt(i)->isPathEmpty();
        qDebug() << "           Unit needs orders:" << civList.at(currentTurn)->GetUnitAt(i)->RequiresOrders;

        if(!civList.at(currentTurn)->GetUnitAt(i)->RequiresOrders && !civList.at(currentTurn)->GetUnitAt(i)->isPathEmpty())
        {
            if(currentTurn == 0)
            {
                qDebug() << "   Clearing Unit Needs Orders icon";
                renderer->SetUnitNeedsOrders(civList.at(0)->GetUnitAt(i)->GetTileIndex(), false);
            }

            qDebug() << "  Updating unit positions";
            uc->MoveUnit(civList.at(currentTurn)->GetUnitAt(i), map, currentTurn);

            if(currentTurn == 0)
                tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

            unitMoved = true;
        }

        if(civList.at(currentTurn)->GetUnitAt(i)->GetHealth() < 0)
        {
            qDebug() << "----Removing Unit";
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
                if(civList.at(0)->GetUnitAt(i)->isPathEmpty())
                    civList.at(0)->GetUnitAt(i)->RequiresOrders = true;

                if(civList.at(0)->GetUnitAt(i)->RequiresOrders)
                {
                    qDebug() << "   Setting Unit Needs Orders icon";
                    renderer->SetUnitNeedsOrders(civList.at(0)->GetUnitAt(i)->GetTileIndex(), true);
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
        qDebug() << "Player's turn next";
        currentTurn = 0;
        turnEnded = false;
        turnStarted = true;
    }
    else
    {
        qDebug() << "AI turn next";
        currentTurn++;
        if(!civList.at(currentTurn)->alive)
        {
            goto nextCivAlive;
        }
    }
}

void GameManager::UpdateTileData()
{
    if(!processedData.relocateOrderGiven && !attackNearby && !attackEnemyCity && !aiFoundCity && !attackRange)
    {
        qDebug() << "Relocation not ordered" << processedData.column << processedData.row;
        unitTile = map->GetTileFromCoord(processedData.column, processedData.row);

        if(unitTile->Selected)
        {
            unitTile->Selected = false;
            selectedTileQueue->enqueue(SelectData{unitTile->GetTileIndex(), false, false});
            this->redrawTile = true;
        }

        if(unitTile->ContainsUnit)
        {
            findUnit = true;
        }
    }
    else if((attackNearby || processedData.relocateOrderGiven || attackEnemyCity || attackRange) && !aiFoundCity)
    {
        qDebug() << "   attackNearby:"<< attackNearby << " relocation ordered:" << processedData.relocateOrderGiven << "attackEnemyCity:" << attackEnemyCity << "attackRange" << attackRange;
        targetTile = map->GetTileFromCoord(processedData.column, processedData.row);
        qDebug() << "       ContainsUnit:" << targetTile->ContainsUnit << "HasCity:" << targetTile->HasCity;
        if(targetTile->ContainsUnit && !attackEnemyCity)
        {
            findUnit = true;
        }
        else if(targetTile->HasCity)
        {
            findCity = true;
        }
    }

    if(findUnit && !attackEnemyCity && !aiFoundCity)
    {
        findUnit = false;
        if(!attackNearby && !attackRange)
        {
            if(unitToMove != NULL)
            {
                selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            }

            unitToMove = uc->FindUnitAtTile(unitTile, map, civList.at(currentTurn)->GetUnitList());

            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), true, false});
            tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

            if(unitToMove->GetOwner() == civList.at(currentTurn)->getCiv() && unitToMove->RequiresOrders)
            {
                map->GetTileAt(unitToMove->GetTileIndex())->Selected = true;
                moveUnit->setEnabled(true);
                this->redrawTile = true;

                if(unitToMove->isNonCombat())
                {
                    qDebug() << "   non-combat unit";
                    if(unitToMove->GetUnitType() == SETTLER)
                    {
                        qDebug() << "       unit is settler";
                        qDebug()<<"Tile Index: "<<unitToMove->GetTileIndex();
                        qDebug()<<"HAS CITY: "<<map->GetTileAt(unitToMove->GetTileIndex())->HasCity;
                        if(!map->GetTileAt(unitToMove->GetTileIndex())->HasCity)
                        {
                            foundCity->setEnabled(true);

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
                    }
                    else if (unitToMove->GetUnitType() == WORKER)
                    {
                        qDebug() << "       unit is worker";
                        if(map->GetTileAt(unitToMove->GetTileIndex())->GetControllingCiv() == civList.at(currentTurn)->getCiv())
                        {
                            buildFarm->setEnabled(true);
                            buildMine->setEnabled(true);
                            buildPlantation->setEnabled(true);
                            buildTradePost->setEnabled(true);
                        }
                        foundCity->setEnabled(false);


                        buildRoad->setEnabled(true);

                        attackUnit->setEnabled(false);
                        attackCity->setEnabled(false);
                        rangeAttack->setEnabled(false);
                        fortifyUnit->setEnabled(false);
                    }
                }
                else //Combat Unit button controls
                {
                    qDebug() << "    unit is combat type";

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
                        qDebug() << "   CivListIndex:" << tile->GetCivListIndex();
                        if((tile->GetCivListIndex() != 0) && (tile->GetCivListIndex() != -1))
                        {
                            int tileIndex = tile->GetTileIndex();

                            selectedTileQueue->enqueue(SelectData {tileIndex, false, true});
                            tileModifiedQueue->enqueue(SelectData {tileIndex, false, false});

                            if(tile->HasCity)
                            {
                                if(!attackCity->isEnabled())
                                {
                                    attackCity->setEnabled(true);
                                }
                            }
                            else if(tile->ContainsUnit)
                            {
                                if(!attackUnit->isEnabled() && unitToMove->isMelee)
                                {
                                    buildFarm->setEnabled(false);
                                    buildMine->setEnabled(false);
                                    buildPlantation->setEnabled(false);
                                    buildTradePost->setEnabled(false);
                                    buildRoad->setEnabled(false);
                                    rangeAttack->setEnabled(false);
                                    attackUnit->setEnabled(true);
                                }
                                else if(!rangeAttack->isEnabled())
                                {
                                    buildFarm->setEnabled(false);
                                    buildMine->setEnabled(false);
                                    buildPlantation->setEnabled(false);
                                    buildTradePost->setEnabled(false);
                                    buildRoad->setEnabled(false);
                                    attackUnit->setEnabled(false);
                                    rangeAttack->setEnabled(true);
                                }
                            }
                            else if(attackCity->isEnabled())
                            {
                                attackCity->setEnabled(false);
                            }
                            else if(rangeAttack->isEnabled())
                            {
                                rangeAttack->setEnabled(false);
                            }
                        }
                    }
                }
            }
            else
            {
                qDebug() << "Player does not own that unit";
                selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            }
        }
        else
        {
            attackNearby = false;
            attackRange = false;

            if(unitToMove->isFortified)
            {
                unitToMove->isFortified = false;
                renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
            }

            targetUnit = uc->FindUnitAtTile(targetTile, map, civList.at(targetTile->GetCivListIndex())->GetUnitList());

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
    }
    else if(findCity && !aiFoundCity)
    {
        findCity = false;

        targetCity = uc->FindCityAtTile(targetTile, map, civList.at(targetTile->GetCivListIndex())->GetCityList());
        selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        selectedTileQueue->enqueue(SelectData{targetCity->GetCityTile()->GetTileIndex(), false, false});
        attackCity->setEnabled(false);
        attackEnemyCity = false;

        if(targetCity->GetControllingCiv() != NO_NATION)
        {
            uc->AttackCity(unitToMove, targetCity);

            //City Conquering Logic
            if(targetCity->GetCityHealth() <= 0 && unitToMove->isMelee)
            {
                qDebug() << "--------<<" << targetCity->GetName() << "Has Been Conquered! >>--------";

                ProcessCityConquer(targetCity, civList.at(currentTurn), civList.at(targetTile->GetCivListIndex()));

                if(civList.at(targetTile->GetCivListIndex())->GetCityList().size() == 0)
                {
                    if(!civList.at(targetTile->GetCivListIndex())->GetUnitList().isEmpty())
                    {
                        foreach(Unit* unit, civList.at(targetTile->GetCivListIndex())->GetUnitList())
                        {
                            renderer->RemoveUnit(unit, gameView);
                        }

                        for(int i = 0; i < civList.at(targetTile->GetCivListIndex())->GetUnitList().size(); i++)
                        {
                            civList.at(targetTile->GetCivListIndex())->RemoveUnit(0);
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

    if(processedData.relocateOrderGiven && !findUnit && !aiFoundCity)
    {
        unitToMove->SetUnitTargetTile(targetTile->GetTileID().column, targetTile->GetTileID().row);
        unitToMove->SetUnitTargetTileIndex(targetTile->GetTileIndex());

        if(unitToMove->isFortified)
        {
            unitToMove->isFortified = false;
            renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
        }

        qDebug() <<"    Finding path";
        uc->FindPath(unitTile, targetTile, map, unitToMove);

        relocateUnit = false;
        processedData.relocateOrderGiven = false;
        map->GetTileAt(unitToMove->GetTileIndex())->Selected = false;
        selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        this->redrawTile = true;

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

        qDebug() << "   Done";
    }

    if(foundACity || aiFoundCity)
    {
        qDebug() << "found a city";

        qDebug()<<foundACity<<aiFoundCity;

        if(foundCity)
            this->foundACity = false;
        if(aiFoundCity)
            this->aiFoundCity = false;

        qDebug()<<foundACity<<aiFoundCity;
        City* city;

        foundCityIndex = unitToMove->GetTileIndex();

        city = map->CreateCity(foundCityIndex, civList.at(currentTurn), false);

        bool valid = true;
        for(int i = 0; i < civList.size(); i++)
        {
            foreach(City* existingCity, civList.at(i)->GetCityList())
            {
                if(existingCity->MSDIntersects(city->GetMinimumSettleDistance()))
                {
                    qDebug() << "--------<< You cannot settle this close to another city. >>--------";
                    delete city;
                    civList.at(currentTurn)->SetCityIndex(civList.at(currentTurn)->getCityIndex() - 1);
                    foreach(Tile* tile, city->GetControlledTiles())
                    {
                        tile->SetControllingCiv(NO_NATION);
                    }
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

        qDebug() << "--Loading buildings";
        city->loadBuildings("../ProjectHiawatha/Assets/Buildings/BuildingList.txt");
        qDebug() << "--Loading units";
        city->loadUnits("../ProjectHiawatha/Assets/Units/UnitList.txt");
        qDebug() << "--Adding City";
        civList.at(currentTurn)->AddCity(city);
        qDebug() << "--Setting HasCity flag";
        map->GetTileAt(foundCityIndex)->HasCity = true;

        QList<Tile*> cityMEB = map->GetNeighborsRange(city->GetCityTile(), 4);
        city->SetMaximumExpansionBorderTiles(cityMEB);
        city->FilterMEBList();

        map->GetTileQueue(city);
        city->SortTileQueue();

        city->SortControlledTiles();
        city->GetControlledTiles().first()->IsWorked = true;

        qDebug() << "--Adding City to renderer";
        renderer->AddCity(city, gameView, false);

        qDebug() << "--Adding Drawing city borders";
        renderer->DrawCityBorders(city, gameView, civList.at(currentTurn)->getCiv());

        qDebug () << "--Setting tile worked";
        foreach(Tile* tile, city->GetControlledTiles())
        {
            renderer->SetTileWorkedIcon(tile, gameView);
        }

        qDebug() << "--Updating yield";
        civList.at(currentTurn)->UpdateCivYield();

        renderer->SetFortifyIcon(foundCityIndex, true);
        renderer->SetUnitNeedsOrders(foundCityIndex, false);
        map->GetTileAt(foundCityIndex)->ContainsUnit = false;
        map->GetTileAt(foundCityIndex)->HasCity=true;
        map->GetTileAt(foundCityIndex)->SetControllingCiv(civList.at(currentTurn)->getCiv());
        renderer->RemoveUnit(unitToMove, gameView);
        civList.at(currentTurn)->RemoveUnit(unitToMove->GetUnitListIndex());

        if(currentTurn == 0)
        {
            clv->addItem(city->GetName());
            selectedTileQueue->enqueue(SelectData{foundCityIndex, false, false});
        }

    }

    if((map->GetTileFromCoord(processedData.column, processedData.row)->Selected == false) && !aiFoundCity)
    {
        if(unitToMove != NULL)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
        }

        attackCity->setEnabled(false);
        attackUnit->setEnabled(false);
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
    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    showTechTreeButton = new QPushButton("Technology Tree");
    connect(showTechTreeButton, SIGNAL(clicked(bool)), this, SLOT(showTechTree()));
    showTechTreeButton->setShortcut(QKeySequence(Qt::Key_T));

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

    productionFocus = new QPushButton("Production Focus");
    connect(productionFocus, SIGNAL(clicked(bool)), this, SLOT(SetProdFocus()));
    productionFocus->setEnabled(false);

    scienceFocus = new QPushButton("Science Focus");
    connect(scienceFocus, SIGNAL(clicked(bool)), this, SLOT(SetScienceFocus()));
    scienceFocus->setEnabled(false);

    foodFocus = new QPushButton("Food Focus");
    connect(foodFocus, SIGNAL(clicked(bool)), this, SLOT(SetFoodFocus()));
    foodFocus->setEnabled(false);

    cultureFocus = new QPushButton("Culture Focus");
    connect(cultureFocus, SIGNAL(clicked(bool)), this, SLOT(SetCultureFocus()));
    cultureFocus->setEnabled(false);

    connect(clv, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(parseItem(QListWidgetItem*)));

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

}

void GameManager::InitLayouts()
{
    vLayout->setMargin(2);

    unitControlButtons->addWidget(showTechTreeButton);
    unitControlButtons->addSpacing(500);
    unitControlButtons->addWidget(endGameProgress);
//    unitControlButtons->addWidget(ConquerCity);
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

    gameLayout->addLayout(unitControlButtons);
    gameLayout->addWidget(cityScreen);
    gameLayout->addWidget(gameView);
    gameLayout->addWidget(techTree);

    playerControlButtons->addWidget(exitGame);
    playerControlButtons->addWidget(clv);
    playerControlButtons->addWidget(techLabel);
    playerControlButtons->addWidget(techText);
    playerControlButtons->addWidget(goldFocus);
    playerControlButtons->addWidget(productionFocus);
    playerControlButtons->addWidget(scienceFocus);
    playerControlButtons->addWidget(foodFocus);
    playerControlButtons->addWidget(cultureFocus);
    playerControlButtons->addWidget(endTurn);

    gameLayout->addLayout(playerControlButtons);

    vLayout->addLayout(gameLayout);
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

void GameManager::ProcessCityConquer(City *tCity, Civilization *aCiv, Civilization *tCiv)
{
    City* city = new City();
    city->SetName(tCity->GetName());
    city->SetCityStrength(tCity->GetCityStrength());
    city->SetBaseCityStrength(tCity->GetBaseStrength());
    city->SetControllingCiv(aCiv->getCiv());
    city->SetCityHealth(100);
    city->SetCityFocus(City::GOLD_FOCUS);
    city->SetCitizenCount(tCity->GetCitizenCount() / 2);

    if(city->GetCitizenCount() < 1)
    {
        city->SetCitizenCount(1);
    }

    if(tCity->IsCityCaptial())
    {
        if(tCity->IsOriginalCapital())
        {
            aCiv->IncrementCapitalsControlled();
            tCiv->SetCaptialsControlled(0);

            city->SetCityAsCapital(false, true);
        }
        else
        {
            city->SetCityAsCapital(false, false);
        }
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

    city->loadUnits("../ProjectHiawatha/Assets/Units/UnitList.txt");
    city->loadBuildings("../ProjectHiawatha/Assets/Buildings/BuildingList.txt");

    foreach(Building* building, tCity->getCurrentBuildingList())
    {
        city->addBuilding(building);
    }

    if(tCity->getHasWorker())
    {
        qDebug() << "--Removing worker";
        renderer->RemoveUnit(tCity->GetGarrisonedWorker(), gameView);
        qDebug() << "-----";
        tCiv->RemoveUnit(tCity->GetGarrisonedWorker()->GetUnitListIndex());
        qDebug() << "----------";
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

void GameManager::showCity(City* city)
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
        cityScreen->loadBuildings("../ProjectHiawatha/Assets/Buildings/BuildingList.txt");
        cityScreen->loadUnits("../ProjectHiawatha/Assets/Units/UnitList.txt");
        cityScreen->getCityInfo(city);
        cityScreen->updateList(city->getNumberOfBuildings());
        cityScreen->updateWidget();

        gameView->centerOn(city->GetCityTile()->GetCenter());
        cityScreen->setGeometry(124, 33, 1145, this->height() - 150);
        gameView->setDragMode(QGraphicsView::NoDrag);

        cityScreen->show();
        cityScreenVisible = true;
    }
    else
    {
//        cityScreen->hide();
        this->goldFocus->setEnabled(false);
        this->productionFocus->setEnabled(false);
        this->scienceFocus->setEnabled(false);
        this->cultureFocus->setEnabled(false);
        this->foodFocus->setEnabled(false);

        gameView->setDragMode(QGraphicsView::ScrollHandDrag);
        cityScreenVisible = false;
        renderer->UpdateCityProductionBar(civList.at(0)->GetCityAt(0), gameView);
    }
}

void GameManager::updateTiles()
{
    processedData = gameView->GetScene()->ProcessTile(relocateUnit);

    if(processedData.newData || foundACity)
    {
        this->UpdateTileData();
    }

    TurnController();

    if(fortify && unitToMove != NULL && !unitToMove->isFortified)
    {
        qDebug() << "Fortifying" << unitToMove->GetName();
        fortify = false;
        unitToMove->isFortified = true;
        unitToMove->RequiresOrders = false;
        renderer->SetFortifyIcon(unitToMove->GetTileIndex(), false);
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);
    }

    if(this->redrawTile && !selectedTileQueue->isEmpty())
    {
        qDebug() << "------Redrawing";
        this->redrawTile = false;
        renderer->UpdateScene(map, gameView, selectedTileQueue);
    }

    this->update();

    if(this->currentTurn == 0 && !civList.at(currentTurn)->alive)
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setWindowFlags(Qt::FramelessWindowHint);
        mBox->setFixedSize(200, 500);
        mBox->setText("You Lose!");
        mBox->exec();
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

    if(cityScreen != NULL && cityScreen->isHidden())
    {
        cityScreenVisible = true;
        showCity(NULL);
    }

    //// FOR TESTING PURPOSES. I WANT TO MAKE SURE AI TURN PROCESSING ISN'T TAKING UP A LOT OF TIME
    end = std::chrono::steady_clock::now();
    if(countTime == true)
    {
        countTime = false;
        qDebug() << "------------------ Time to process AI Turns:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us";
    }
}

void GameManager::moveUnitTo()
{
    qDebug() << "MoveUnitTo";
    if(moveUnit->isEnabled())
    {
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
        techTree->setGeometry(124, 33, 1145, this->height() - 150);
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
    this->foundACity = true;
}

void GameManager::buildNewRoad()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),ROAD))
    {
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
}

void GameManager::buildNewFarm()
{
    qDebug()<<"YIELD BEFORE: "<<map->GetTileAt(unitToMove->GetTileIndex())->GetYield()->GetFoodYield();
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),FARM))
    {
        qDebug() << "True";
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(FARM, unitToMove->GetTileIndex(), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;

    qDebug()<<"YIELD AFTER: "<<map->GetTileAt(unitToMove->GetTileIndex())->GetYield()->GetFoodYield();
}

void GameManager::buildNewPlantation()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),PLANTATION))
    {
        qDebug() << "True";
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(PLANTATION, unitToMove->GetTileIndex(), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewTradePost()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),TRADE_POST))
    {
        qDebug() << "True";
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(TRADE_POST, unitToMove->GetTileIndex(), gameView);
    }
    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewMine()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),MINE))
    {
        qDebug() << "True";
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(MINE, unitToMove->GetTileIndex(), gameView);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::attackMelee()
{
    attackNearby = true;
    qDebug() << "Attack nearby to true";
}

/// These are for testing the setting and changing the focus of a city.
void GameManager::SetGoldFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(City::GOLD_FOCUS);
    this->focusChanged = true;
}

void GameManager::SetProdFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(City::PRODUCTION_FOCUS);
    this->focusChanged = true;
}

void GameManager::SetScienceFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(City::SCIENCE_FOCUS);
    this->focusChanged = true;
}

void GameManager::SetFoodFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(City::FOOD_FOCUS);
    this->focusChanged = true;
}

void GameManager::SetCultureFocus()
{
    this->civList.at(currentTurn)->GetCityAt(clv->currentRow())->SetCityFocus(City::CULTURE_FOCUS);
    this->focusChanged = true;
}

void GameManager::AttackCity()
{
    attackEnemyCity = true;
}

void GameManager::RangeAttack()
{
    attackRange = true;
}

void GameManager::Fortify()
{
    qDebug() << "Fortifying unit";
    this->fortify = true;
    fortifyUnit->setEnabled(false);
}

void GameManager::parseItem(QListWidgetItem *item)
{
    qDebug() << "--------" << item->text() << "selected";
    this->goldFocus->setEnabled(true);
    this->productionFocus->setEnabled(true);
    this->scienceFocus->setEnabled(true);
    this->cultureFocus->setEnabled(true);
    this->foodFocus->setEnabled(true);

    this->showCity(civList[0]->GetCityAt(clv->currentRow()));

//    gameView->setDragMode(QGraphicsView::ScrollHandDrag);
//    cityScreenVisible = false;
//    renderer->UpdateCityProductionBar(civList.at(0)->GetCityAt(0), gameView);
}






