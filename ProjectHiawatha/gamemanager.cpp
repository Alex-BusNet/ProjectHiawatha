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
#include "queuedata.h"
#define DEBUG

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);
QString warStyle = "QMessageBox { background-color: #145e88 } QPushButton {  background-color: #4899C8; border: 2px solid #f6b300; border-radius: 3px; font: 10px; min-width: 70px; } QPushButton#Cancel { background-color: #f53252 } QPushButton:pressed { background-color: #77adcb; }";

GameManager::GameManager(QWidget *parent, bool fullscreen, int mapSizeX, int mapSizeY, Nation player, int numAI) : QWidget(parent)
{
    InitVariables(fullscreen);
    playerCiv = player;
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
    this->inPrimaryState = true;

    InitRenderData();

    lastData = NULL; //map->GetDataAt(1);

    currentTurn = 0;
    gameTurn = 0;
    //Start at 4000 BC. The game increments the turn to 1, and subsequently the in game year by 40 years, upon game start.
    year = -4040;

}

GameManager::GameManager(QWidget *parent, bool fullscreen, bool loadLatest)
{
    InitVariables(fullscreen);
    civLoadFailed = false;

    qDebug() << "Loading data";
    if(loadLatest)
    {
        QFile lastMapSaveFile("Saves/latest_map.json");
        if(!lastMapSaveFile.open(QIODevice::ReadOnly))
        {
            qWarning("Could not open last map save file");
            this->close();
            return;
        }

        QByteArray mapSave = lastMapSaveFile.readAll();
        QJsonDocument mapDoc = QJsonDocument::fromJson(mapSave);
        QJsonObject mapObj = mapDoc.object();
        map = new Map();
#ifdef __APPLE__
        map->ReadMapSaveData(mapObj);
#else
        mapInit = QtConcurrent::run(this->map, Map::ReadMapSaveData, mapObj);
#endif

        QFile lastCivSaveFile("Saves/latest_manager.json");
        if(!lastCivSaveFile.open(QIODevice::ReadOnly))
        {
            qWarning("Could not open last manager save file");
            mapInit.waitForFinished();
            this->close();
            return;
        }

        QByteArray gmSave = lastCivSaveFile.readAll();
        QJsonDocument gmDoc = QJsonDocument::fromJson(gmSave);
        QJsonObject gmObject= gmDoc.object();

        playersAliveCount = gmObject["playersleftalive"].toInt();
        currentTurn = gmObject["currentturn"].toInt();
        gameTurn = gmObject["gameturn"].toInt();
        year = gmObject["year"].toInt();
        state = static_cast<ActionState>(gmObject["state"].toInt());
        int civListSize = gmObject["civlistsize"].toInt();

        for(int i = 0; i < civListSize; i++)
        {
            Civilization* c = new Civilization();
            c->loadTechs("Assets/Techs/Technology.txt");
            civList.push_back(c);
        }

#ifdef __APPLE__
        LoadCivs();
#else
        civInit = QtConcurrent::run(this, GameManager::LoadCivs);
#endif

        QFile lastDiploSaveFile("Saves/latest_diplo.json");
        if(!lastDiploSaveFile.open(QIODevice::ReadOnly))
        {
            qWarning("Could not open last diplomacy save file");
            mapInit.waitForFinished();
            civInit.waitForFinished();
            this->close();
            return;
        }

        QByteArray diploSave = lastDiploSaveFile.readAll();
        QJsonDocument diploDoc = QJsonDocument::fromJson(diploSave);

#ifdef __APPLE__
        diplo->ReadDiploSaveData(diploDoc.object());
#else
        QFuture<void> diploInit = QtConcurrent::run(this->diplo, Diplomacy::ReadDiploSaveData, diploDoc.object());

        mapInit.waitForFinished();
        civInit.waitForFinished();
        diploInit.waitForFinished();
#endif
        renderer = new Renderer(map->GetMapSizeX());

        if(civLoadFailed)
        {
            this->close();
            return;
        }

        playerCiv = civList.at(0)->getCiv();

        QFile civData("Assets/Data/civ_data.json");
        QJsonArray civRefData;
        if(!civData.open(QIODevice::ReadOnly))
        {
            qWarning("Couldn't open civData file");
        }
        else
        {
            QByteArray byte = civData.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(byte);
            civRefData = doc.array();
        }

        // Add city tile and controlled tiles to city objects.
        QPixmap pic;
        foreach(Civilization* c, civList)
        {
            foreach(City* ci, c->GetCityList())
            {
                ci->SetCityTile(map->GetTileAt(ci->loadedCityTileIndex));
                for(int i = 0; i < ci->controlledTilesIndex.size(); i++)
                {
                    ci->AddControlledTile(map->GetTileAt(ci->controlledTilesIndex.at(i)));
                }

                QList<Tile*> cityMEB = map->GetNeighborsRange(map->GetTileAt(ci->loadedCityTileIndex), 4);
                ci->SetMaximumExpansionBorderTiles(cityMEB);
                ci->FilterMEBList();

                map->GetTileQueue(ci);
                map->DefineCityBordersNew(ci);
                ci->SortTileQueue();
                ci->SortControlledTiles();
                ci->GetControlledTiles().first()->IsWorked = true;

                ci->UpdateCityYield();
                ci->UpdateCityStatus();
            }

            if(c->isCivAI())
            {
                QVector<Unit*> threats;
                foreach(int j, c->lowThreatIndex)
                {
                    threats.push_back(uc->FindUnitAtTile(map->GetTileAt(j), c->GetUnitList()));
                }

                c->setLowThreats(threats);
                threats.clear();

                foreach(int j, c->midThreatIndex)
                {
                    threats.push_back(uc->FindUnitAtTile(map->GetTileAt(j), c->GetUnitList()));
                }

                c->setMidThreats(threats);
                threats.clear();

                foreach(int j, c->highThreatIndex)
                {
                    threats.push_back(uc->FindUnitAtTile(map->GetTileAt(j), c->GetUnitList()));
                }

                c->setHighThreats(threats);
                threats.clear();
            }

            c->UpdateCivYield();

            if(c->HasCivMetPlayer())
            {
                diplo->MeetPlayer(c->getCivIndex());
            }

            pic = QPixmap(civRefData.at(c->getCiv()).toObject()["leaderimagepath"].toString());
            diplo->SetLeaderImage(c->getCivIndex(), pic);
        }
    }

    InitRenderData();

    foreach(Civilization *ci, civList)
    {
        foreach(City* c, ci->GetCityList())
        {
            if(ci->getCivIndex() == 0)
            {
                foreach(Tile* t, c->GetControlledTiles())
                {
                    foreach(Tile* n,  map->GetNeighborsRange(t, 2))
                    {
                        if(n->GetControllingCivListIndex() != 0)
                        {
                            if(!n->DiscoveredByPlayer)
                            {
                                n->DiscoveredByPlayer = true;
                                n->IsSeenByPlayer = true;
                                n->CanAlwaysBeSeen = true;
                            }
                            else
                            {
                                n->IsSeenByPlayer = true;
                                n->CanAlwaysBeSeen = true;
                            }
                        }
                    }
                }
            }

            renderer->UpdateCityHealthBar(c, gameView);
            renderer->UpdateCityGrowthBar(c, gameView);
            renderer->UpdateCityProductionBar(c, gameView);
        }

        foreach(Unit* u, ci->GetUnitList())
        {
            if(u->RequiresOrders)
            {
                renderer->SetUnitNeedsOrders(u->GetTileIndex(), true);
            }
        }
    }

    //Find all discovered tiles and load them appropriately.
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        if(map->GetTileAt(i)->DiscoveredByPlayer)
        {
            viewUpdateTiles->enqueue(ViewData{i, DISCOVERED});

            if(!map->GetTileAt(i)->IsSeenByPlayer && !map->GetTileAt(i)->CanAlwaysBeSeen)
            {
                viewUpdateTiles->enqueue(ViewData{i, HIDDEN});
            }

            renderer->SetTileTooltip(map->GetTileAt(i)->GetTileIndex(), *map->GetTileAt(i)->GetYield(), map->GetTileAt(i)->GetControllingCiv(), map->GetTileAt(i)->GetTileIDString());
        }

        if(map->GetTileAt(i)->GetTileImprovement() != NONE)
        {
            renderer->SetTileImprovement(map->GetTileAt(i)->GetTileImprovement(), map->GetTileAt(i), gameView);
        }
    }

    techText->setText(QString(" %1 / %2").arg(civList.at(0)->getAccumulatedScience()).arg(civList.at(0)->getCurrentTech()->getCost()));

    clickedData = NULL;
    lastData = NULL;
    turnStarted = false;
    updateTimer->start();
}

GameManager::~GameManager()
{
    updateTimer->stop();

#ifndef __APPLE__
    QFuture<void> derender;
    if(renderer != NULL)
    {
        derender = QtConcurrent::run(this, GameManager::DeinitRenderer);
    }
#else
    if(renderer != NULL)
        delete renderer;
#endif

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

    disconnect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    disconnect(showDiplomacy, SIGNAL(clicked(bool)), this, SLOT(toggleDiplomacy()));
    disconnect(showTechTreeButton, SIGNAL(clicked(bool)), this, SLOT(showTechTree()));
    disconnect(moveUnit, SIGNAL(clicked(bool)), this, SLOT(moveUnitTo()));
    disconnect(endTurn, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));
    disconnect(buildFarm, SIGNAL(clicked(bool)), this, SLOT(buildNewFarm()));
    disconnect(buildMine, SIGNAL(clicked(bool)), this, SLOT(buildNewMine()));
    disconnect(buildPlantation, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    disconnect(buildTradePost, SIGNAL(clicked(bool)), this, SLOT(buildNewTradePost()));
    disconnect(buildRoad, SIGNAL(clicked(bool)), this, SLOT(buildNewRoad()));
    disconnect(buildCamp, SIGNAL(clicked(bool)), this, SLOT(buildNewCamp()));
    disconnect(buildPasture, SIGNAL(clicked(bool)), this, SLOT(buildNewPasture()));
    disconnect(buildFishBoat, SIGNAL(clicked(bool)), this, SLOT(buildNewFishBoat()));
    disconnect(buildQuarry, SIGNAL(clicked(bool)), this, SLOT(buildNewQuarry()));
    disconnect(buildOilWell, SIGNAL(clicked(bool)), this, SLOT(buildNewOilWell()));
    disconnect(foundCity, SIGNAL(clicked(bool)), this, SLOT(foundNewCity()));
    disconnect(attackUnit, SIGNAL(clicked(bool)), this, SLOT(attackMelee()));
    disconnect(goldFocus, SIGNAL(clicked(bool)), this, SLOT(SetGoldFocus()));
    disconnect(productionFocus, SIGNAL(clicked(bool)), this, SLOT(SetProdFocus()));
    disconnect(scienceFocus, SIGNAL(clicked(bool)), this, SLOT(SetScienceFocus()));
    disconnect(foodFocus, SIGNAL(clicked(bool)), this, SLOT(SetFoodFocus()));
    disconnect(cultureFocus, SIGNAL(clicked(bool)), this, SLOT(SetCultureFocus()));
    disconnect(clv, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(parseItem()));
    disconnect(attackCity, SIGNAL(clicked(bool)), this, SLOT(AttackCity()));
    disconnect(rangeAttack, SIGNAL(clicked(bool)), this, SLOT(RangeAttack()));
    disconnect(fortifyUnit, SIGNAL(clicked(bool)), this, SLOT(Fortify()));
    disconnect(help, SIGNAL(clicked(bool)), this, SLOT(OpenHelp()));

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
    delete buildCamp;
    delete buildPasture;
    delete buildFishBoat;
    delete buildOilWell;
    delete buildQuarry;
    delete foundCity;
    delete attackUnit;
    delete attackCity;
    delete rangeAttack;
    delete fortifyUnit;
    delete help;
    delete spacer;

    delete goldFocus;
    delete productionFocus;
    delete scienceFocus;
    delete foodFocus;
    delete cultureFocus;

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

    if(playerInfoRect != NULL)
        delete playerInfoRect;

    if(gameStatusRect != NULL)
        delete gameStatusRect;

    if(unitToMove != NULL)
        delete unitToMove;

//    if(targetUnit != NULL)
//        delete targetUnit;

    if(targetCity != NULL)
        delete targetCity;

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

    if(updateTimer != NULL)
        delete updateTimer;

    if(lastData != NULL)
        delete lastData;

    if(clickedData != NULL)
        delete clickedData;

    if(targetData != NULL)
        delete targetData;

#ifndef __APPLE__
    if(!derender.isFinished())
    {
        derender.waitForFinished();
    }
#endif

    delete gameView;
}

void GameManager::WarByDiplomacy()
{
    int targetCivListIndex = diplo->GetIndex();
    qDebug() << "   target" << targetCivListIndex;
    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetCivListIndex)->GetLeaderName())});

    civList.at(currentTurn)->SetAtWar(targetCivListIndex);
    civList.at(targetCivListIndex)->SetAtWar(currentTurn);
    diplo->DeclareWarOn(civList.at(targetCivListIndex)->getCiv(), targetCivListIndex, civList.at(currentTurn)->getCiv(), currentTurn);
    diplo->declareWar->setEnabled(false);
}

void GameManager::MakePeace()
{
    ProcessPeace(diplo->GetIndex());
}

void GameManager::InitCivs(Nation player, int numAI)
{
    QFile civData("Assets/Data/civ_data.json");
    if(!civData.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open civData file");
    }
    else
    {
        QByteArray byte = civData.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(byte);
        QJsonArray civRefData = doc.array();

        srand(time(0));
        int civNum;
        QVector<int> selNat;

        if(player == Random)
        {
            player = (Nation)(rand() % 16);
        }

        QJsonObject obj = civRefData.at(player).toObject();
        Civilization* civ = new Civilization(obj, false);

        civ->loadTechs("Assets/Techs/Technology.txt");
        civ->setCurrentTech(civ->GetTechList().at(0));
        civ->setNextTech(civ->GetTechList().at(1));
        civ->setCivIndex(0);
        techLabel->setText(QString(" %1 ").arg(civ->getCurrentTech()->getName()));

        civList.push_back(civ);
        diplo->AddLeader(obj["name"].toString(), QPixmap(obj["leaderimagepath"].toString()), player, true);

        selNat.push_back(player);

        for(int i = 0; i < numAI; i++)
        {
    newCivRand:
            // The modulo number at the end indicates the
            // max number of civs in the game minus the random option.
            civNum = rand() % 16;

            // look to see if the selected civ has already been chosen
            foreach(int j, selNat)
            {
                if(j == civNum)
                {
                    goto newCivRand;
                }
            }

            // Civ has not already been chosen, so create a new civ and
            //  load it into the civList.
            obj = civRefData.at(civNum).toObject();
            civ = new Civilization(obj, true);

            civ->loadTechs("Assets/Techs/Technology.txt");
            civ->setCurrentTech(civ->GetTechList().at(0));
            civ->setNextTech(civ->GetTechList().at(1));
            civ->setCivIndex(civList.size());
            civList.push_back(civ);

            diplo->AddLeader(obj["name"].toString(), QPixmap(obj["leaderimagepath"].toString()), (Nation)civNum, false);

            selNat.push_back(civNum);
        }
    }

    if(!mapInit.isFinished())
    {
        mapInit.waitForFinished();
    }

    map->SpawnCivs(civList);
}

void GameManager::LoadCivs()
{
    QFile lastCivSaveFile("Saves/latest_civs.json");
    if(!lastCivSaveFile.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open last civ save file");
        civLoadFailed = true;
        return;
    }

    QByteArray civSave = lastCivSaveFile.readAll();
    QJsonDocument civDoc = QJsonDocument::fromJson(civSave);
    QJsonArray civArray = civDoc.object()["civilizations"].toArray();

    for(int i = 0; i < civList.size(); i++)
    {
        civList.at(i)->ReadData(civArray.at(i).toObject());
    }
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
    static bool AiTurnInProgress = false;
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
        if(civList.at(currentTurn)->HasCivMetPlayer())
            statusMessage = QString("--------<< Processing Turn for %1 >>--------").arg(civList.at(currentTurn)->GetLeaderName());
        else
            statusMessage = QString("--------<< Processing Turn for Unmet Player %1 >>--------").arg(currentTurn);

        if(!AiTurnInProgress)
        {
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
                    diplo->UpdateLeader(0);
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
                        targetData = map->GetDataAt(data.unit->GetTargetTileIndex());
                    }
                    else if(state == AI_DECLARE_WAR)
                    {
                        diplo->UpdateLeader(0);
                        WarByDiplomacy();
                    }
                    else if(state == ATTACK_MELEE)
                    {
                        unitToMove = data.unit;
                        targetUnit = data.target;
                        ProcessAttackUnit();
                    }
                    else if(state == ATTACK_RANGE)
                    {
                        unitToMove = data.unit;
                        targetUnit = data.target;
                        ProcessAttackUnit();
                    }
                    this->UpdateTileData();
                }
            }
            //This is just for extra precautions so that we don't try to end the AI's
            // turn and move on before the AI thread has completly finished.
            future.waitForFinished();
#endif
            EndTurn();
            AiTurnInProgress = false;
        }
    }

}

void GameManager::StartTurn()
{
    if(currentTurn == 0)
    {
        if(!civList.at(currentTurn)->alive)
        {
            Defeat();
        }
        else if(playersAliveCount == 1)
        {
            Victory();
        }

        for(int i = 0; i < civList.size(); i++)
        {
            if(i == 0)
            {
                delete endGameText;
                endGameText = new QString("Capitals Controlled:");
                endGameText->append(QString("\nYou\t%1/%2").arg(civList.at(i)->GetCapitalsControlled()).arg(civList.size()));
            }
            else
            {
                if(civList.at(i)->alive && civList.at(i)->HasCivMetPlayer())
                    endGameText->append(QString("\n%1\t%2/%3").arg(civList.at(i)->GetLeaderName()).arg(civList.at(i)->GetCapitalsControlled()).arg(civList.size()));
                else
                    endGameText->append(QString("\nUnmet Player %1\t%2/%3").arg(i).arg(civList.at(i)->GetCapitalsControlled()).arg(civList.size()));
            }
        }

        endGameProgress->setText(*endGameText);
        endGameProgress->setAlignment(Qt::AlignRight);
    }

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
            statusMessage = QString("--------<< You have finished researching %1 >>--------").arg(civList.at(0)->getCurrentTech()->getName());
        }
        QString unlocks = "UNLOCKED: ";
        QString techName = civList.at(currentTurn)->getCurrentTech()->getName();
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

		// Moved updates to currentTech and nextTech into Civilization::setTechIndex() function
        //civList.at(currentTurn)->setNextTech(civList.at(currentTurn)->GetTechList().at(techIndex+1));
        //civList.at(currentTurn)->setCurrentTech(civList.at(currentTurn)->GetTechList().at(techIndex));
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
    CityProdData cpd;
    while(!QueueData::isEmpty())
    {
        cpd = QueueData::dequeue();

        if(cpd.isUnit)
        {
            Unit *u = new Unit();
            u->ReadUnitSaveData(UnitData.at(cpd.producedUnitIndex).toObject());

            u->SetOwner(civList.at(cpd.civIndex)->getCiv(), cpd.civIndex);
            u->SetUnitListIndex(civList.at(cpd.civIndex)->GetUnitList().size());
            civList.at(cpd.civIndex)->GetCityAt(cpd.cityIndex)->setProducedUnit(u);

            u->SetOwner(civList.at(currentTurn)->getCiv(), cpd.civIndex);
            u->SetUnitListIndex(civList.at(currentTurn)->GetUnitList().size());


            foreach(Tile* t, civList.at(cpd.civIndex)->GetCityAt(cpd.cityIndex)->GetControlledTiles())
            {
                OccupantData tod = map->GetODFromTileAt(t->GetTileIndex());

                if(u->isNaval())
                {
                    if((tod.civIndex != -1) || !(t->GetTileType() == WATER)) { continue; }
                    else
                    {
//                            t->SetOccupyingUnit(u);
                            u->SetPosition(t);
//                            t->ContainsUnit = true;
                            map->SetOccupantDataAt(t->GetTileIndex(), OccupantData{u->GetOwner(), currentTurn, u->GetUnitListIndex(), u->isNonCombat()});
                            break;
                    }
                }
                else
                {
                    if((tod.civIndex != -1) || !(t->Walkable) || (t->GetTileType() == WATER)) { continue; }
                    else
                    {
//                            t->SetOccupyingUnit(u);
                            u->SetPosition(t);
//                            t->ContainsUnit = true;
                            map->SetOccupantDataAt(t->GetTileIndex(), OccupantData{u->GetOwner(), currentTurn, u->GetUnitListIndex(), u->isNonCombat()});
                            break;
                    }
                }
            }

            civList.at(cpd.civIndex)->AddUnit(u);
            renderer->AddUnit(u, map, gameView);
        }
        else
        {
            Building *b = new Building();
            b->ReadBuildingSaveData(BuildingData.at(cpd.producedUnitIndex).toObject());
            civList.at(cpd.civIndex)->GetCityAt(cpd.cityIndex)->addBuilding(b);
        }

        if(civList.at(0)->getCiv() == civList.at(currentTurn)->getCiv() && update.productionFinished)
        {
            ns->PostNotification(Notification{4, QString("Production in %1 finished").arg(civList.at(currentTurn)->GetCityAt(cpd.cityIndex)->GetName())});
        }
    }

    for(int i = 0; i<civList.at(currentTurn)->GetCityList().size();i++)
    {
        //If a city took damage and has healed, alert the renderer of this change.
        if(update.cityHealed)
        {
            renderer->UpdateCityHealthBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        }

        civList.at(currentTurn)->GetCityAt(i)->UpdateCityYield();

        //Update the production and population growth bars for the city.
        renderer->UpdateCityProductionBar(civList.at(currentTurn)->GetCityAt(i), gameView);
        renderer->UpdateCityGrowthBar(civList.at(currentTurn)->GetCityAt(i), gameView);
    }

    civList.at(currentTurn)->UpdateCivYield();

	//--------------------------------------------
	// Move to Civilization class?
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

    civList.at(currentTurn)->SetMilitaryStrength(civMilStr);
	//--------------------------------------------

    //Update the yield status bar for the player.
    if(currentTurn == 0)
    {
        goldText->setText(QString("%1 (%2%3)").arg(civList.at(0)->GetTotalGold()).arg(civList.at(0)->losingGold ? " " : "+").arg(civList.at(0)->GetGptAdjusted()));
        prodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetProductionYield()));
        foodText->setText(QString("%1").arg(civList.at(0)->getCivYield()->GetFoodYield()));
        sciText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalScience()).arg(civList.at(0)->getCivYield()->GetScienceYield()));
        culText->setText(QString("%1 (+%2)").arg(civList.at(0)->GetTotalCulture()).arg(civList.at(0)->getCivYield()->GetCultureYield()));
        techLabel->setText(QString("%3\n %1 / %2").arg(accumulatedScience).arg(techCost).arg(civList.at(0)->getCurrentTech()->getName()));

        goldText->setToolTip(QString("GPT: %1\nMaintenance: -%2").arg(civList.at(0)->GetGPT()).arg(civList.at(0)->GetMaintenance()));

        endTurn->setEnabled(true);
    }

    state = IDLE;
}

void GameManager::EndTurn()
{
    countTime = true;
    state = IDLE;
    processedData.relocateOrderGiven = false;
    processedData.newData = false;
    bool unitMoved = false;
    QList<Tile*> t;

    //Disable all the buttons when the player ends their turn.
    if(currentTurn == 0)
    {
        statusMessage = " ";
        /*
        moveUnit->setEnabled(false);

        buildFarm->setEnabled(false);
        buildMine->setEnabled(false);
        buildPlantation->setEnabled(false);
        buildTradePost->setEnabled(false);
        buildRoad->setEnabled(false);
        buildCamp->setEnabled(false);
        buildPasture->setEnabled(false);
        buildFishBoat->setEnabled(false);
        buildOilWell->setEnabled(false);
        buildQuarry->setEnabled(false);

        attackUnit->setEnabled(false);
        attackCity->setEnabled(false);
        rangeAttack->setEnabled(false);
        fortifyUnit->setEnabled(false);
        */

        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);

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

                t = map->GetNeighborsRange(map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex()), 2);
                foreach(Tile* ti, t)
                {
                    if(ti->SeenByUnits == 1 && !ti->CanAlwaysBeSeen)
                    {
                        ti->SeenByUnits = 0;
                        ti->IsSeenByPlayer = false;
                        viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), HIDDEN});
                    }
                    else
                    {
                        if(ti->SeenByUnits == 0) { ti->SeenByUnits = 0; }
                        else { ti->SeenByUnits--; }
                    }
                }
            }

            uc->MoveUnit(civList.at(currentTurn)->GetUnitAt(i), map);

            if(currentTurn == 0)
            {
                tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

                t = map->GetNeighborsRange(map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex()), 2);

                foreach(Tile* ti, t)
                {
                    if(ti->SeenByUnits == 0)
                    {
                        ti->SeenByUnits = 1;
                        if(!ti->DiscoveredByPlayer)
                        {
                            ti->DiscoveredByPlayer = true;
                            ti->IsSeenByPlayer = true;
                            viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), DISCOVERED});

                            if(ti->GetControllingCivListIndex() > 0)
                            {
                                if(!civList.at(ti->GetControllingCivListIndex())->HasCivMetPlayer())
                                {
                                    civList.at(ti->GetControllingCivListIndex())->MeetPlayer();
                                    diplo->MeetPlayer(ti->GetControllingCivListIndex());
                                    QMessageBox *mbox = new QMessageBox();
                                    mbox->setText(QString("You have met %1").arg(civList.at(ti->GetControllingCivListIndex())->GetLeaderName()));
                                    mbox->setStyleSheet(warStyle);
                                    mbox->setWindowFlags(Qt::FramelessWindowHint);
                                    mbox->exec();
                                    delete mbox;
                                }
                            }
                        }
                        else if(!ti->IsSeenByPlayer)
                        {
                            ti->IsSeenByPlayer = true;
                            viewUpdateTiles->enqueue(ViewData{ti->GetTileIndex(), VISIBLE});
                        }
                    }
                    else
                    {
                        ti->SeenByUnits++;
                    }

                    renderer->SetTileTooltip(ti->GetTileIndex(), *ti->GetYield(), ti->GetControllingCiv(), ti->GetTileIDString());
                }
            }
            else if(map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex())->SeenByUnits > 0)
            {
                if(!civList.at(currentTurn)->HasCivMetPlayer())
                {
                    civList.at(currentTurn)->MeetPlayer();
                    diplo->MeetPlayer(currentTurn);
                    QMessageBox *mbox = new QMessageBox();
                    mbox->setText(QString("You have met %1").arg(civList.at(currentTurn)->GetLeaderName()));
                    mbox->setStyleSheet(warStyle);
                    mbox->setWindowFlags(Qt::FramelessWindowHint);
                    mbox->exec();
                    delete mbox;
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
//            map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex())->ContainsUnit = false;
//            map->GetTileAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex())->SetOccupyingCivListIndex(-1);
//            civList.at(currentTurn)->GetUnitAt(i)->GetUnitTile()->SetOccupyingUnit(NULL);
            map->SetOccupantDataAt(civList.at(currentTurn)->GetUnitAt(i)->GetTileIndex(), DEFAULT_OCCUPANT);
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
 * Updated version of updateTileData().
 *
 * Primary Action:
 *  1) Tile is clicked
 *  2) If tile has unit, select tile, and load unit action buttons
 *  3) If tile does not have unit, deselect previous tile (if one exists)
 *
 * Secondary Action:
 *  1) If move unit, then check if clicked tile has no unit
 *      1.a) If tile has unit, cancel and return to primary action state
 *      1.b) If tile has no unit, check if tile is controlled
 *          1.b.1) If tile is controlled, do war check. If not at war, ask user to continue.
 *          1.b.2) If user cancels, return to primary action state, else invade.
 *  2) If attack unit, check if clicked tile has unit.
 *      2.a) Do war check on unit, if not at war, ask user to continue.
 *      2.b) If user cancels, return to primary action state, else declare war and attack.
 *  3) If attack city, check if tile has citty. If no city, return to primary action state.
 *      3.a) Do war check on city, if not at war, ask user to continue.
 *      3.b) If user cancels, return to primary action state, else delcare war and attack.
 */
void GameManager::NewUpdateTileData()
{
    if(processedData.newData)
    {
#ifdef DEBUG
        qDebug() << "Primary action";
#endif
        if(inPrimaryState) // Primary Action State
        {
            clickedData = map->GetDataFromCoord(processedData.column, processedData.row);

            // Occupant == NO_NATION means tile is empty
            if (clickedData->od.OccupantNation == NO_NATION)
            {
#ifdef DEBUG
                qDebug() << "  Tile Empty";
                qDebug() << "  lastData null:" << (lastData == NULL);
#endif
                if(lastData != NULL)
                {
                    if(lastData->tile != NULL)
                    {
                        lastData->tile->Selected = false;
                        selectedTileQueue->enqueue(SelectData{lastData->tile->GetTileIndex(), false, false});
                        this->redrawTile = true;
                        this->setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
                    }
                }
            }
            else if(clickedData->od.OccupantNation == civList.at(currentTurn)->getCiv())
            {
#ifdef DEBUG
                qDebug() << "  Unit Found";
                qDebug() << "  lastData null:" << (lastData == NULL);
                if(lastData != NULL)
                {
                    qDebug() << "    lastData tile null:" << (lastData->tile == NULL);
                }
#endif
                if ((lastData != NULL) && (lastData->tile != NULL))
                {
#ifdef DEBUG
                    qDebug() << "    Deselecting last tile";
#endif
                    lastData->tile->Selected = false;
                    selectedTileQueue->enqueue(SelectData{lastData->tile->GetTileIndex(), false, false});
                    //this->redrawTile = true;
                    this->setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
                }

                unitToMove = civList.at(clickedData->od.civIndex)->GetUnitAt(clickedData->od.unitIndex);

                // This shouldn't be null, but check just to be sure
                if(unitToMove != NULL)
                {
#ifdef DEBUG
                    qDebug() << "    Unit Type:" << unitToMove->GetName();
#endif
                    if(unitToMove->GetUnitType() == WORKER)
                        setUnitButtonVisibility(clickedData->tile->GetWorkerButtons());
                    else if(unitToMove->GetUnitType() == SETTLER)
                        setUnitButtonVisibility(SETTLER_BUTTONS_VISIBLE);
                    else if(!unitToMove->isNonCombat())
                    {
                        if(uc->UnitInRange(map, clickedData, unitToMove->GetRange(), civList.at(currentTurn)->getCiv()))
                        {
                            if(unitToMove->isMelee)
                                setUnitButtonVisibility(MELEE_BUTTONS_VISIBLE);
                            else
                                setUnitButtonVisibility(RANGE_BUTTONS_VISIBLE);
                        }
                        else
                            setUnitButtonVisibility(BASIC_BUTTONS_VISIBLE);
                    }

                    this->redrawTile = true;
                    selectedTileQueue->enqueue(SelectData{clickedData->tile->GetTileIndex(), true, false});
                    tileModifiedQueue->enqueue(SelectData{clickedData->tile->GetTileIndex(), false, false});

                }
            }
        }
        else // Secondary Action State
        {
#ifdef DEBUG
            qDebug() << "Secondary action";
#endif
            targetData = map->GetDataFromCoord(processedData.column, processedData.row);

            switch(state)
            {
            case MOVE_UNIT:
                if(targetData->od.OccupantNation == NO_NATION)
                {
                    if(!this->WarCheck(targetData))
                    {
                        if(targetData->od.OccupantNation == NO_NATION)
                        {
                            statusMessage = "--------<< You cannot move there >>--------";
                        }
                        else
                        {
                            unitToMove->SetUnitTargetTile(targetData->tile->GetTileID().column, targetData->tile->GetTileID().row);
                            unitToMove->SetUnitTargetTileIndex(targetData->tile->GetTileIndex());

                            if(unitToMove->isFortified)
                            {
                                unitToMove->isFortified = false;
                                renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
                            }

                            uc->FindPath(clickedData->tile, targetData->tile, map, unitToMove, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()});

                            relocateUnit = false;
                            processedData.relocateOrderGiven = false;
                            clickedData->tile->Selected = false;
                            renderer->SetUnitNeedsOrders(clickedData->tile->GetTileIndex(), false);
                            selectedTileQueue->enqueue(SelectData{clickedData->tile->GetTileIndex(), false, false});

                            unitControlButtonsVisible = NO_BUTTONS_VISIBLE;
                            setUnitButtonVisibility(unitControlButtonsVisible);

                            this->redrawTile = true;

                            state = IDLE;
                        }
                    }
                }

                inPrimaryState = true;
                break;

            case ATTACK_MELEE:
            case ATTACK_RANGE:
                if(WarCheck(targetData))
                {
                    if((targetData->od.OccupantNation == NO_NATION) || (targetData->od.civIndex == currentTurn))
                    {
                        inPrimaryState = true;
                        break;
                    }

                    ProcessAttackUnit();
                }

                inPrimaryState = true;
                break;

            case ATTACK_CITY:
                targetCity = targetData->tile->GetGoverningCity();

                if(currentTurn == 0)
                {
                    selectedTileQueue->enqueue(SelectData{clickedData->tile->GetTileIndex(), false, false});
                    selectedTileQueue->enqueue(SelectData{targetData->tile->GetTileIndex(), false, false});

                    setUnitButtonVisibility(BASIC_BUTTONS_VISIBLE);
                }

                if(WarCheck(targetData))
                {
                    uc->AttackCity(unitToMove, targetCity);

                    //City Conquering Logic
                    if(targetCity->GetCityHealth() <= 0 && unitToMove->isMelee)
                    {
                        if(currentTurn == 0)
                            statusMessage = QString("--------<< %1 Has Been Conquered! >>--------").arg(targetCity->GetName());

                        int targetIndex = targetData->tile->GetControllingCivListIndex();

                        ProcessCityConquer(targetCity, civList.at(currentTurn), civList.at(targetIndex));

                        if(civList.at(targetIndex)->GetCityList().size() == 0)
                        {
                            if(!civList.at(targetIndex)->GetUnitList().isEmpty())
                            {
                                foreach(Unit* unit, civList.at(targetIndex)->GetUnitList())
                                {
//                                    unit->GetUnitTile()->SetOccupyingUnit(NULL);
                                    renderer->RemoveUnit(unit, gameView);
                                }

                                for(int i = 0; i < civList.at(targetIndex)->GetUnitList().size(); i++)
                                {
                                    civList.at(targetIndex)->RemoveUnit(i);
                                }
                            }

                            playersAliveCount--;
                        }
                    }

                    renderer->UpdateUnits(map, gameView, unitToMove, false);
                    renderer->UpdateCityHealthBar(targetCity, gameView);
                    renderer->SetUnitNeedsOrders(clickedData->tile->GetTileIndex(), false);
                    this->redrawTile = true;
                }

                inPrimaryState = true;
                break;

            case CONQUER:
            case INVADE:
            case FOUND_CITY:
            case AI_FOUND_CITY:
            case FIND_CITY:
            case IDLE:
            default:
                inPrimaryState = true;
                break;
            }
        }

        lastData = clickedData;
    }
}


// TODO: Rewrite UpdateTileData()
/*
 * UpdateTileData() is the function that handles all of the tile selection, unit combat,
 * unit-to-city combat, and Declarations of War done by the player as well as handling
 * city founding and city conquering for both the player and the AI.
 */
void GameManager::UpdateTileData()
{
    /*
    if(!processedData.relocateOrderGiven && state == IDLE)
    {
        unitTile = map->GetTileFromCoord(processedData.column, processedData.row);

        if(unitTile->Selected)
        {
            unitTile->Selected = false;
            selectedTileQueue->enqueue(SelectData{unitTile->GetTileIndex(), false, false});
            this->redrawTile = true;
        }

        if(unitTile->ContainsUnit())
        {
            state = FIND_UNIT;
        }
    }
    else if(state == ATTACK_MELEE || processedData.relocateOrderGiven || state == ATTACK_RANGE || state == ATTACK_CITY)
    {
        targetTile = map->GetTileFromCoord(processedData.column, processedData.row);

        if((targetTile->ContainsUnit() || targetTile->HasCity) && (targetTile->GetControllingCivListIndex() != 0) && (targetTile->GetControllingCivListIndex() != -1))
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

        if(!targetTile->ContainsUnit() && (state == ATTACK_MELEE || state == ATTACK_RANGE))
        {
            QList<Tile*> neighbors = map->GetNeighbors(targetTile);

            foreach(Tile* tile, neighbors)
            {
                if(tile->ContainsUnit() && !tile->HasCity)
                {
                    if(tile->GetOccupyingUnit()->GetOwningCivIndex() != currentTurn)
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

        unitToMove = unitTile->GetOccupyingUnit(); //uc->FindUnitAtTile(unitTile, civList.at(currentTurn)->GetUnitList());

#ifdef DEBUG
      qDebug() << "unitToMove null: " << (unitToMove == NULL);
#endif

        if(unitToMove != NULL)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), true, false});
            tileModifiedQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

            if(unitToMove->GetOwner() == civList.at(currentTurn)->getCiv() && (unitToMove->RequiresOrders || unitToMove->isFortified))
            {
                if(unitToMove->isFortified)
                {
                    renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
                    unitToMove->isFortified = false;
                }

                //map->GetTileAt(unitToMove->GetTileIndex())->Selected = true;
                unitTile->Selected = true;
                moveUnit->setEnabled(true);
                this->redrawTile = true;

                if(unitToMove->isNonCombat())
                {
                    if(unitToMove->GetUnitType() == SETTLER)
                    {
                        //if(!map->GetTileAt(unitToMove->GetTileIndex())->HasCity)
                        if(!unitTile->HasCity)
                        {
                            unitControlButtonsVisible = SETTLER_BUTTONS_VISIBLE;
                            setUnitButtonVisibility(unitControlButtonsVisible);
    //                        foundCity->setEnabled(true);

    //                        buildFarm->setEnabled(false);
    //                        buildMine->setEnabled(false);
    //                        buildPlantation->setEnabled(false);
    //                        buildTradePost->setEnabled(false);
    //                        buildRoad->setEnabled(false);
    //                        buildCamp->setEnabled(false);
    //                        buildPasture->setEnabled(false);
    //                        buildFishBoat->setEnabled(false);
    //                        buildOilWell->setEnabled(false);
    //                        buildQuarry->setEnabled(false);
                        }
                    }
                    else if (unitToMove->GetUnitType() == WORKER)
                    {
                        //if(map->GetTileAt(unitToMove->GetTileIndex())->GetControllingCiv() == civList.at(currentTurn)->getCiv())
                        if(unitTile->GetControllingCivListIndex() == currentTurn)
                        {
                            unitControlButtonsVisible = unitTile->GetWorkerButtons();
    /*
                            //if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveFarm)
                            if(!unitTile->CanHaveFarm)
                                unitControlButtonsVisible &= 0xF7FFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveMine)
                            if(!unitTile->CanHaveMine)
                                unitControlButtonsVisible &= 0xFBFFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHavePlantation)
                            if(!unitTile->CanHavePlantation)
                                unitControlButtonsVisible &= 0xFEFFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveTrade)
                            if(!unitTile->CanHaveTrade)
                                unitControlButtonsVisible &= 0xFDFFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveCamp)
                            if(!unitTile->CanHaveCamp)
                                unitControlButtonsVisible &= 0xFFEFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveFishBoat)
                            if(!unitTile->CanHaveFishBoat)
                                unitControlButtonsVisible &= 0xFFDFF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveOilWell)
                            if(!unitTile->CanHaveOilWell)
                                unitControlButtonsVisible &= 0xFFF7F;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHavePasture)
                            if(!unitTile->CanHavePasture)
                                unitControlButtonsVisible &= 0xFFFBF;

    //                        if(!map->GetTileAt(unitToMove->GetTileIndex())->CanHaveQuarry)
                            if(!unitTile->CanHaveQuarry)
                                unitControlButtonsVisible &= 0xFFBFF;

                        }

                        setUnitButtonVisibility(unitControlButtonsVisible);
                    }
                }
                else //Combat Unit button controls
                {
                    if(unitToMove->isMelee)
                        unitControlButtonsVisible = MELEE_BUTTONS_VISIBLE;
                    else if(!unitToMove->isMelee)
                        unitControlButtonsVisible = RANGE_BUTTONS_VISIBLE;

                    setUnitButtonVisibility(unitControlButtonsVisible);

                    QList<Tile*> tiles = map->GetNeighborsRange(unitTile, unitToMove->GetRange());

                    foreach(Tile *tile, tiles)
                    {
                        if(((tile->GetOccupyingCivListIndex() > 0) || (tile->GetControllingCivListIndex() > 0)) && (tile->HasCity || tile->ContainsUnit()))
                        {
                            int tileIndex = tile->GetTileIndex();

                            if(tile->HasCity && tile->GetControllingCivListIndex() != 0)
                            {
                                qDebug() << "   Tile:" << tile->GetTileIDString() << "controlled by:" << tile->GetControllingCivListIndex() << "occupied by" << tile->GetOccupyingUnit()->GetOwningCivIndex();
                                selectedTileQueue->enqueue(SelectData {tileIndex, false, true});
                                tileModifiedQueue->enqueue(SelectData {tileIndex, false, false});
                                attackCity->setEnabled(true);
                            }
                            else if(tile->ContainsUnit() && !tile->HasCity && tile->GetOccupyingCivListIndex() != 0)
                            {
                                selectedTileQueue->enqueue(SelectData {tileIndex, false, true});
                                tileModifiedQueue->enqueue(SelectData {tileIndex, false, false});

                                if(unitToMove->isMelee)
                                {
                                    unitControlButtonsVisible = MELEE_BUTTONS_VISIBLE;
                                    setUnitButtonVisibility(unitControlButtonsVisible);
                                }
                                else if(!unitToMove->isMelee)
                                {
                                    unitControlButtonsVisible = RANGE_BUTTONS_VISIBLE;
                                    setUnitButtonVisibility(unitControlButtonsVisible);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                unitControlButtonsVisible = NO_BUTTONS_VISIBLE;
                setUnitButtonVisibility(unitControlButtonsVisible);

                if(currentTurn == 0)
                {
                    if(unitToMove->GetOwner() != civList.at(0)->getCiv())
                        statusMessage = "--------<< You do not own that unit >>--------";
                    else
                        statusMessage = "--------<< You cannot give orders to this unit >>-------";
                }

                selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            }
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

            unitControlButtonsVisible |= 0x00008;
            setUnitButtonVisibility(unitControlButtonsVisible);
        }

        if(!uc->AtPeaceWith(targetCity->GetCityTile(), WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()}))
        {
            uc->AttackCity(unitToMove, targetCity);

            //City Conquering Logic
            if(targetCity->GetCityHealth() <= 0 && unitToMove->isMelee)
            {
                if(currentTurn == 0)
                    statusMessage = QString("--------<< %1 Has Been Conquered! >>--------").arg(targetCity->GetName());

                int targetIndex = targetTile->GetControllingCivListIndex();

                ProcessCityConquer(targetCity, civList.at(currentTurn), civList.at(targetIndex));

                if(civList.at(targetIndex)->GetCityList().size() == 0)
                {
                    if(!civList.at(targetIndex)->GetUnitList().isEmpty())
                    {
                        foreach(Unit* unit, civList.at(targetIndex)->GetUnitList())
                        {
                            //map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
                            //map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);
                            unitTile->SetOccupyingUnit(NULL);
                            renderer->RemoveUnit(unit, gameView);
                        }

                        for(int i = 0; i < civList.at(targetIndex)->GetUnitList().size(); i++)
                        {
                            civList.at(targetIndex)->RemoveUnit(i);
                        }
                    }

                    playersAliveCount--;
                }
            }

            renderer->UpdateUnits(map, gameView, unitToMove, false);
            renderer->UpdateCityHealthBar(targetCity, gameView);
            renderer->SetUnitNeedsOrders(unitTile->GetTileIndex(), false);
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
                if((targetTile->GetOccupyingUnit() == NULL)
                        && (targetTile->GetControllingCivListIndex() != currentTurn)
                         && (targetTile->GetControllingCivListIndex() != -1))
                    warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(targetTile->GetControllingCivListIndex())->GetLeaderName()));
                else if (targetTile->GetControllingCivListIndex() != -1)
                    warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(targetTile->GetOccupyingUnit()->GetOwningCivIndex())->GetLeaderName()));

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
            if(targetTile->ContainsUnit())
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
//            map->GetTileAt(unitToMove->GetTileIndex())->Selected = false;
            unitTile->Selected = false;
            renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            this->redrawTile = true;
        }

        unitControlButtonsVisible = NO_BUTTONS_VISIBLE;
        setUnitButtonVisibility(unitControlButtonsVisible);

//        foundCity->setEnabled(false);

//        buildFarm->setEnabled(false);
//        buildMine->setEnabled(false);
//        buildPlantation->setEnabled(false);
//        buildTradePost->setEnabled(false);
//        buildRoad->setEnabled(false);
//        buildCamp->setEnabled(false);
//        buildPasture->setEnabled(false);
//        buildFishBoat->setEnabled(false);
//        buildOilWell->setEnabled(false);
//        buildQuarry->setEnabled(false);

//        attackUnit->setEnabled(false);
//        attackCity->setEnabled(false);
//        rangeAttack->setEnabled(false);
//        fortifyUnit->setEnabled(false);
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

        qDebug() << "   Checking spawn location";
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
                        tile->SetControllingCivListIndex(-1);
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

        qDebug() << "City spawn valid";
        city->loadBuildings("Assets/Buildings/BuildingList.txt");
        city->loadUnits("Assets/Units/UnitList.txt");
        civList.at(currentTurn)->AddCity(city);
        map->GetTileAt(foundCityIndex)->SetGoverningCity(city, currentTurn);

        QList<Tile*> cityMEB = map->GetNeighborsRange(city->GetCityTile(), 4);
        city->SetMaximumExpansionBorderTiles(cityMEB);
        city->FilterMEBList();

        map->GetTileQueue(city);
        map->DefineCityBordersNew(city);
//        city->SortTileQueue();

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

        //map->GetTileAt(foundCityIndex)->ContainsUnit = false;
        //map->GetTileAt(foundCityIndex)->HasCity=true;
        map->GetTileAt(foundCityIndex)->SetControllingCivListIndex(currentTurn);

        renderer->RemoveUnit(unitToMove, gameView);
        civList.at(currentTurn)->RemoveUnit(unitToMove->GetUnitListIndex());

        if(currentTurn == 0)
        {
            clv->addItem(city->GetName());
            selectedTileQueue->enqueue(SelectData{foundCityIndex, false, false});
            unitControlButtonsVisible = NO_BUTTONS_VISIBLE;
            setUnitButtonVisibility(unitControlButtonsVisible);
        }
    }

    if((map->GetTileFromCoord(processedData.column, processedData.row)->Selected == false) && state != AI_FOUND_CITY)
    {
        if(unitToMove != NULL)
        {
            selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
            if(!tileModifiedQueue->isEmpty())
            {
                renderer->UpdateScene(map, gameView, tileModifiedQueue);
            }
        }

        unitControlButtonsVisible = NO_BUTTONS_VISIBLE;
        setUnitButtonVisibility(unitControlButtonsVisible);

//        attackCity->setEnabled(false);
//        attackUnit->setEnabled(false);
//        rangeAttack->setEnabled(false);
//        buildFarm->setEnabled(false);
//        buildMine->setEnabled(false);
//        buildPlantation->setEnabled(false);
//        buildTradePost->setEnabled(false);
//        buildRoad->setEnabled(false);
//        buildCamp->setEnabled(false);
//        buildPasture->setEnabled(false);
//        buildFishBoat->setEnabled(false);
//        buildOilWell->setEnabled(false);
//        buildQuarry->setEnabled(false);
//        moveUnit->setEnabled(false);
//        fortifyUnit->setEnabled(false);
        redrawTile = true;
    }
    */
}

void GameManager::InitVariables(bool fullscreen)
{
    gameView = new GameView(this);
    ac = new AI_Controller();
    uc = new UnitController();
    clv = new QListWidget(this);
    ns = new NotificationSystem(this);
    about = new About();
    diplo = new Diplomacy(this);
    unitButtonsWidget = new QWidget(this);
    playerButtonsWidget = new QWidget(this);
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

    //cityScreen = new CityScreen();
    //cityScreen->hide();
    cityScreen = NULL;

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
//    unitTile = NULL;
//    targetTile = NULL;
    state = IDLE;

    cityScreenVisible = false;
    techTreeVisible = false;
    diploVisible = false;
    toggleOn = false;
    devModeOn = false;
    relocateUnit = false;
    turnEnded = false;
    turnStarted = true;
    countTime = false;
    citySelected = false;
    updateFoW = false;
    focusChanged = false;
    fortify = false;

    currentProductionName = "No Production Selected";

    if(!fullscreen)
    {
        this->setFixedSize(1400, 700);
        this->setWindowTitle("Project Hiawatha");
        this->setWindowFlags(Qt::WindowMinimizeButtonHint);
    }
    else
    {
        gameView->setMaximumWidth(widget.screenGeometry(0).width());
        this->setWindowState(Qt::WindowFullScreen);
    }

    gameView->ConfigureGraphicsView();

    LoadJsonData();

    updateTimer = new QTimer();
    updateTimer->setInterval(17);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTiles()));
    updateTimer->start();

    qsrand(QTime::currentTime().msec());
    this->InitButtons();
    this->InitLayouts();
    this->setLayout(vLayout);
    #ifdef __APPLE__
        playerInfoRect = new QRect(0, 0, this->width(), 20);
        gameStatusRect = new QRect(0, this->height() - 20, this->width(), 20);
    #endif
    this->show();
    ns->hide();
}

void GameManager::LoadJsonData()
{
    QFile buildExport("Assets/Data/buildings.json");

    if(!buildExport.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open buildExport save file");
    }
    else
    {
        QByteArray data = buildExport.readAll();
        QJsonDocument build = QJsonDocument::fromJson(data);
        BuildingData = build.array();
    }

    QFile unitExport("Assets/Data/units.json");

    if(!unitExport.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open unitExport save file");
    }
    else
    {
        QByteArray data = unitExport.readAll();
        QJsonDocument unit = QJsonDocument::fromJson(data);
        UnitData = unit.array();

    }

    QFile techExport("Assets/Data/techList.json");

    if(!techExport.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open techExport save file");
    }
    else
    {
        QByteArray data = techExport.readAll();
        QJsonDocument tech = QJsonDocument::fromJson(data);
        TechData = tech.array();
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
    moveUnit->setShortcut(QKeySequence(Qt::Key_X));

    endTurn = new QPushButton("End Turn");
    connect(endTurn, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));
    endTurn->setShortcut(QKeySequence(Qt::Key_Return));

    buildFarm = new QPushButton("Build Farm");
    connect(buildFarm, SIGNAL(clicked(bool)), this, SLOT(buildNewFarm()));
    buildFarm->setShortcut(QKeySequence(Qt::Key_F));

    buildMine = new QPushButton("Build Mine");
    connect(buildMine, SIGNAL(clicked(bool)), this, SLOT(buildNewMine()));
    buildMine->setShortcut(QKeySequence(Qt::Key_S));

    buildPlantation = new QPushButton("Build Plantation");
    connect(buildPlantation, SIGNAL(clicked(bool)), this, SLOT(buildNewPlantation()));
    buildPlantation->setShortcut(QKeySequence(Qt::Key_D));

    buildTradePost = new QPushButton ("Build Trading Post");
    connect(buildTradePost, SIGNAL(clicked(bool)), this, SLOT(buildNewTradePost()));
    buildTradePost->setShortcut(QKeySequence(Qt::Key_G));

    buildRoad = new QPushButton("Build Road");
    connect(buildRoad, SIGNAL(clicked(bool)), this, SLOT(buildNewRoad()));
    buildRoad->setShortcut(QKeySequence(Qt::Key_H));

    buildCamp = new QPushButton("Build Camp");
    connect(buildCamp, SIGNAL(clicked(bool)), this, SLOT(buildNewCamp()));
    // Shortcut

    buildPasture = new QPushButton("Build Pasture");
    connect(buildPasture, SIGNAL(clicked(bool)), this, SLOT(buildNewPasture()));
    // Shortcut

    buildFishBoat = new QPushButton("Build Fishing Boat");
    connect(buildFishBoat, SIGNAL(clicked(bool)), this, SLOT(buildNewFishBoat()));
    // Shortcut

    buildQuarry = new QPushButton("Build Quarry");
    connect(buildQuarry, SIGNAL(clicked(bool)), this, SLOT(buildNewQuarry()));
    // Shortcut

    buildOilWell = new QPushButton("Build Oil Well");
    connect(buildOilWell, SIGNAL(clicked(bool)), this, SLOT(buildNewOilWell()));
    // Shortcut

    foundCity = new QPushButton("Found City");
    connect(foundCity, SIGNAL(clicked(bool)), this, SLOT(foundNewCity()));
    foundCity->setShortcut(QKeySequence(Qt::Key_Q));

    attackUnit = new QPushButton("Attack");
    connect(attackUnit, SIGNAL(clicked(bool)), this, SLOT(attackMelee()));
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
    attackCity->setShortcut(QKeySequence(Qt::Key_E));

    rangeAttack = new QPushButton("Range Attack");
    connect(rangeAttack, SIGNAL(clicked(bool)), this, SLOT(RangeAttack()));
    rangeAttack->setShortcut(QKeySequence(Qt::Key_R));

    fortifyUnit = new QPushButton("Fortify");
    connect(fortifyUnit, SIGNAL(clicked(bool)), this, SLOT(Fortify()));
    fortifyUnit->setShortcut(QKeySequence(Qt::Key_A));

    help = new QPushButton("Help");
    connect(help, SIGNAL(clicked(bool)), this, SLOT(OpenHelp()));
    help->setShortcut(QKeySequence(Qt::Key_Z));

    toggleFoW = new QPushButton("Toggle FoW");
    connect(toggleFoW, SIGNAL(clicked(bool)), this, SLOT(toggleFog()));

    devMode = new QShortcut(QKeySequence(Qt::Key_Home), this);
    connect(devMode, SIGNAL(activated()), this, SLOT(enterDevMode()));

    int spacerSize = (this->height() - 280);
    spacer = new QSpacerItem(100, spacerSize, QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void GameManager::InitLayouts()
{
    vLayout->setMargin(2);

    unitControlButtons->addWidget(showTechTreeButton);
    unitControlButtons->addWidget(showDiplomacy);
    unitControlButtons->addSpacerItem(spacer);
    unitControlButtons->addWidget(attackCity);
    unitControlButtons->addWidget(rangeAttack);
    unitControlButtons->addWidget(attackUnit);
    unitControlButtons->addWidget(fortifyUnit);
    unitControlButtons->addWidget(foundCity);
    unitControlButtons->addWidget(buildFarm);
    unitControlButtons->addWidget(buildMine);
    unitControlButtons->addWidget(buildPlantation);
    unitControlButtons->addWidget(buildTradePost);
    unitControlButtons->addWidget(buildCamp);
    unitControlButtons->addWidget(buildOilWell);
    unitControlButtons->addWidget(buildPasture);
    unitControlButtons->addWidget(buildQuarry);
    unitControlButtons->addWidget(buildFishBoat);
    unitControlButtons->addWidget(buildRoad);
    unitControlButtons->addWidget(moveUnit);
    unitControlButtons->setGeometry(QRect(0, 20, 100, this->height() - 20));
    //unitControlButtons->setSizeConstraint(QVBoxLayout::SetFixedSize);

//    unitControlButtons->setGeometry(QRect(0, 20, 100, 20));
//    gameLayout->addLayout(unitControlButtons);

    unitButtonsWidget->setLayout(unitControlButtons);
    unitButtonsWidget->setGeometry(QRect(0, 25, 100, this->height()- 40));

#ifdef DEBUG
    unitButtonsWidget->setStyleSheet("QWidget { background-color: transparent; border: 1px solid red; } QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; max-width: 100px; }");
#else
    unitButtonsWidget->setStyleSheet("QWidget { background-color: transparent; } QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; max-width: 100px; }");
#endif

    gameLayout->addWidget(gameView);

    techTree->setGeometry(150, 20, techTree->width(), techTree->height());
    techTree->setVisible(false);
    ns->setGeometry(this->width() - 170, 20, 50, this->height() - 40);

    diplo->setGeometry(150, 25, gameView->width(), gameView->height());

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
//    playerControlButtons->addWidget(techText);
#ifdef DEBUG
    playerControlButtons->addWidget(toggleFoW);
#endif
    playerControlButtons->addWidget(goldFocus);
    playerControlButtons->addWidget(productionFocus);
    playerControlButtons->addWidget(scienceFocus);
    playerControlButtons->addWidget(foodFocus);
    playerControlButtons->addWidget(cultureFocus);
    playerControlButtons->addWidget(endTurn);
//    playerControlButtons->setGeometry(QRect(this->width() - 100, 20, 100, this->height() - 40));

    playerButtonsWidget->setLayout(playerControlButtons);
    playerButtonsWidget->setGeometry(QRect(this->width() - 120, 20, 120, this->height() - 40));
    QString playerButtonStyle = "QWidget { background-color: transparent }";
    playerButtonStyle += "QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; max-width: 150px; }";
    playerButtonStyle += "QListWidget { background-color: grey; color: white; border: 3px inset black; }";
    playerButtonStyle += "QLabel { color: black; max-width: 170px; }";
    playerButtonsWidget->setStyleSheet(playerButtonStyle);
//    gameLayout->addLayout(playerControlButtons);

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

#ifdef __APPLE__
QDir bin(QCoreApplication::applicationDirPath());
bin.cdUp();
bin.cdUp();
bin.cdUp();
QDir::setCurrent(bin.absolutePath());
#endif

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

    Yieldinfo->addSpacerItem(new QSpacerItem(1000, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    vLayout->insertLayout(0, Yieldinfo);
}

void GameManager::InitRenderData()
{
    renderer->DrawHexScene(map, gameView);
    InitYieldDisplay();

    for(int i = 0; i < civList.size(); i++)
    {
        renderer->LoadCities(civList.at(i)->GetCityList(), gameView);

        if(i == 0)
        {
            foreach(City* ci, civList.at(i)->GetCityList())
            {
                foreach(Tile* n, map->GetNeighborsRange(ci->GetCityTile(), 3))
                {
                    OccupantData tod = map->GetODFromTileAt(n->GetTileIndex());
                    if(n->DiscoveredByPlayer)
                    {
                        viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), DISCOVERED});
                        renderer->SetTileTooltip(n->GetTileIndex(), *n->GetYield(), n->GetControllingCiv(), n->GetTileIDString());

                        if(!n->IsSeenByPlayer && (tod.OccupantNation == NO_NATION))
                        {
                            viewUpdateTiles->enqueue(ViewData{n->GetTileIndex(), HIDDEN});
                        }
                    }
                    else
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

                }
            }

            endGameText = new QString("Capitals Controlled:");
            endGameText->append(QString("\nYou\t1/%1").arg(civList.size()));
        }
        else
        {
            if(civList.at(i)->HasCivMetPlayer())
                endGameText->append(QString("\n%1     1/%2").arg(civList.at(i)->GetLeaderName()).arg(civList.size()));
            else
                endGameText->append(QString("\nUnmet Player %1\t1/%2").arg(i).arg(civList.size()));
        }

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
    }

    endGameProgress->setText(*endGameText);
    endGameProgress->setAlignment(Qt::AlignRight);
    endGameProgress->setStyleSheet("QLabel { background-color: #145e88; border: 1px solid #f6b300; color: white; max-width: 130px; }");

    ////Keep this statement. I need it at different points
    /// in the debugging process. -Port
#ifdef DEBUG
    //renderer->DrawGridLines(gameView);
    //renderer->DrawGuiText(map, stringData, gameView);
#endif
    zoomScale = 1;

    for(int i = 0; i < 3; i++)
    {
        zoomIn();
    }

    gameView->centerOn(civList.at(0)->GetCityAt(0)->GetCityTile()->GetCenter());
#ifndef __APPLE__
    playerInfoRect = new QRect(0, 0, this->width(), 20);
    gameStatusRect = new QRect(0, this->height() - 20, this->width(), 20);
#endif
    statusMessage = " ";

    setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    techLabel->setText(QString("%1\n %2/%3").arg(civList.at(0)->getCurrentTech()->getName()).arg(civList.at(0)->getAccumulatedScience()).arg(civList.at(0)->getCurrentTech()->getCost()));
    techLabel->setStyleSheet("QLabel { background-color: #145e88; border: 1px solid #f6b300; color: white; }");
    diplo->UpdateLeader(0);
}

void GameManager::DeinitRenderer()
{
    delete renderer;
}

void GameManager::SaveGame()
{
    if(!QDir("Saves").exists())
    {
        QDir().mkdir("Saves");
    }

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
        qWarning("Couldn't open diplo save file");
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
    gmObj["year"] = year;
    gmObj["civlistsize"] = civList.size();
    gmObj["playersleftalive"] = playersAliveCount;

    doc.setObject(gmObj);
    managerSaveFile.write(doc.toJson());
    managerSaveFile.flush();
    managerSaveFile.close();
}

void GameManager::toggleWidgetVisiblityAll()
{
    clv->setVisible(!clv->isVisible());
    exitGame->setVisible(!exitGame->isVisible());
    endTurn->setVisible(!endTurn->isVisible());
    showTechTreeButton->setVisible(!showTechTreeButton->isVisible());
    showDiplomacy->setVisible(!showDiplomacy->isVisible());
    help->setVisible(!help->isVisible());
#ifdef DEBUG
    toggleFoW->setVisible(!toggleFoW->isVisible());
#endif
    goldFocus->setVisible(!goldFocus->isVisible());
    productionFocus->setVisible(!productionFocus->isVisible());
    scienceFocus->setVisible(!scienceFocus->isVisible());
    foodFocus->setVisible(!foodFocus->isVisible());
    cultureFocus->setVisible(!cultureFocus->isVisible());
}

void GameManager::setUnitButtonVisibility(unsigned int visibleButtons)
{
#ifdef DEBUG
    qDebug() << "Setting unit buttons: 0b" << hex << visibleButtons;
#endif

    if(visibleButtons & 0x08000)
    {
        if(this->unitControlButtons->indexOf(buildFarm) == -1)
        {
            buildFarm->setVisible(true);
            this->unitControlButtons->addWidget(buildFarm);
        }
    }
    else if(this->unitControlButtons->indexOf(buildFarm) > -1)
    {
        buildFarm->setVisible(false);
        this->unitControlButtons->removeWidget(buildFarm);
    }

    if(visibleButtons & 0x04000)
    {
        if(this->unitControlButtons->indexOf(buildMine) == -1)
        {
            buildMine->setVisible(true);
            this->unitControlButtons->addWidget(buildMine);
        }
    }
    else if(this->unitControlButtons->indexOf(buildMine) > -1)
    {
        buildMine->setVisible(false);
        this->unitControlButtons->removeWidget(buildMine);
    }

    if(visibleButtons & 0x02000)
    {
        if(this->unitControlButtons->indexOf(buildTradePost) == -1)
        {
            buildTradePost->setVisible(true);
            this->unitControlButtons->addWidget(buildTradePost);
        }
    }
    else if(this->unitControlButtons->indexOf(buildTradePost) > -1)
    {
        buildTradePost->setVisible(false);
        this->unitControlButtons->removeWidget(buildTradePost);
    }

    if(visibleButtons & 0x01000)
    {
        if(this->unitControlButtons->indexOf(buildPlantation) == -1)
        {
            buildPlantation->setVisible(true);
            this->unitControlButtons->addWidget(buildPlantation);
        }
    }
    else if(this->unitControlButtons->indexOf(buildPlantation) > -1)
    {
        buildPlantation->setVisible(false);
        this->unitControlButtons->removeWidget(buildPlantation);
    }

    if(visibleButtons & 0x00800)
    {
        if(this->unitControlButtons->indexOf(buildRoad) == -1)
        {
            buildRoad->setVisible(true);
            this->unitControlButtons->addWidget(buildRoad);
        }
    }
    else if(this->unitControlButtons->indexOf(buildRoad) > -1)
    {
        buildRoad->setVisible(false);
        this->unitControlButtons->removeWidget(buildRoad);
    }

    if(visibleButtons & 0x00400)
    {
        if(this->unitControlButtons->indexOf(buildQuarry) == -1)
        {
            buildQuarry->setVisible(true);
            this->unitControlButtons->addWidget(buildQuarry);
        }
    }
    else if(this->unitControlButtons->indexOf(buildQuarry) > -1)
    {
        buildQuarry->setVisible(false);
        this->unitControlButtons->removeWidget(buildQuarry);
    }

    if(visibleButtons & 0x00200)
    {
        if(this->unitControlButtons->indexOf(buildFishBoat) == -1)
        {
            buildFishBoat->setVisible(true);
            this->unitControlButtons->addWidget(buildFishBoat);
        }
    }
    else if(this->unitControlButtons->indexOf(buildFishBoat) > -1)
    {
        buildFishBoat->setVisible(false);
        this->unitControlButtons->removeWidget(buildFishBoat);
    }

    if(visibleButtons & 0x00100)
    {
        if(this->unitControlButtons->indexOf(buildCamp) == -1)
        {
            buildCamp->setVisible(true);
            this->unitControlButtons->addWidget(buildCamp);
        }
    }
    else if(this->unitControlButtons->indexOf(buildCamp) > -1)
    {
        buildCamp->setVisible(false);
        this->unitControlButtons->removeWidget(buildCamp);
    }

    if(visibleButtons & 0x00080)
    {
        if(this->unitControlButtons->indexOf(buildOilWell) == -1)
        {
            buildOilWell->setVisible(true);
            this->unitControlButtons->addWidget(buildOilWell);
        }
    }
    else if(this->unitControlButtons->indexOf(buildOilWell) > -1)
    {
        buildOilWell->setVisible(false);
        this->unitControlButtons->removeWidget(buildOilWell);
    }

    if(visibleButtons & 0x00040)
    {
        if(this->unitControlButtons->indexOf(buildPasture) == -1)
        {
            buildPasture->setVisible(true);
            this->unitControlButtons->addWidget(buildPasture);
        }
    }
    else if(this->unitControlButtons->indexOf(buildPasture) > -1)
    {
        buildPasture->setVisible(false);
        this->unitControlButtons->removeWidget(buildPasture);
    }

    if(visibleButtons & 0x00020)
    {
        if(this->unitControlButtons->indexOf(foundCity) == -1)
        {
            foundCity->setVisible(true);
            this->unitControlButtons->addWidget(foundCity);
        }
    }
    else if(this->unitControlButtons->indexOf(foundCity) > -1)
    {
        foundCity->setVisible(false);
        this->unitControlButtons->removeWidget(foundCity);
    }

    if(visibleButtons & 0x00010)
    {
        if(this->unitControlButtons->indexOf(attackUnit) == -1)
        {
            attackUnit->setVisible(true);
            this->unitControlButtons->addWidget(attackUnit);
        }
    }
    else if(this->unitControlButtons->indexOf(attackUnit) > -1)
    {
        attackUnit->setVisible(false);
        this->unitControlButtons->removeWidget(attackUnit);
    }

    if(visibleButtons & 0x00008)
    {
        if(this->unitControlButtons->indexOf(attackCity) == -1)
        {
            attackCity->setVisible(true);
            this->unitControlButtons->addWidget(attackCity);
        }
    }
    else if(this->unitControlButtons->indexOf(attackCity) > -1)
    {
        attackCity->setVisible(false);
        this->unitControlButtons->removeWidget(attackCity);
    }

    if(visibleButtons & 0x00004)
    {
        if(this->unitControlButtons->indexOf(rangeAttack) == -1)
        {
            rangeAttack->setVisible(true);
            this->unitControlButtons->addWidget(rangeAttack);
        }
    }
    else if(this->unitControlButtons->indexOf(rangeAttack) > -1)
    {
        rangeAttack->setVisible(false);
        this->unitControlButtons->removeWidget(rangeAttack);
    }

    if(visibleButtons & 0x00002)
    {
        if(this->unitControlButtons->indexOf(fortifyUnit) == -1)
        {
            fortifyUnit->setVisible(true);
            this->unitControlButtons->addWidget(fortifyUnit);
        }
    }
    else if(this->unitControlButtons->indexOf(fortifyUnit) > -1)
    {
        fortifyUnit->setVisible(false);
        this->unitControlButtons->removeWidget(fortifyUnit);
    }

    if(visibleButtons & 0x00001)
    {
        if(this->unitControlButtons->indexOf(moveUnit) == -1)
        {
            moveUnit->setVisible(true);
            this->unitControlButtons->addWidget(moveUnit);
        }
    }
    else if(this->unitControlButtons->indexOf(moveUnit) > -1)
    {
        moveUnit->setVisible(false);
        this->unitControlButtons->removeWidget(moveUnit);
    }

    //spacer->changeSize(100, this->height() - (unitControlButtons->count() * 10), QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
}

bool GameManager::WarCheck(MapData *md) //Tile *t)
{
    playerToWar = false;
    if(((md->od.OccupantNation != NO_NATION) || md->tile->HasCity) && (md->tile->GetControllingCivListIndex() != currentTurn) && (md->tile->GetControllingCivListIndex() != -1))
    {
        if(uc->AtPeaceWith(md, WarData{civList.at(currentTurn)->isAtWar(), civList.at(currentTurn)->GetCivListIndexAtWar()}))
        {
            if(gameTurn == 1)
            {
                statusMessage = "--------<< You cannot declare war on the first turn. >>--------";
                state = IDLE;
            }
            else
            {
                warbox->setText(QString("You are not at war with %1.\nIf you continue, this will be a declaration of war. \nContinue?").arg(civList.at(md->od.civIndex)->GetLeaderName()));

                warbox->setWindowFlags(Qt::FramelessWindowHint);

                connect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarDeclared()));
                connect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));

                warbox->exec();

                disconnect(warbox->button(QMessageBox::Ok), SIGNAL(clicked(bool)), this, SLOT(WarDeclared()));
                disconnect(warbox->button(QMessageBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(WarAvoided()));
            }
        }
    }

    return playerToWar;
}

void GameManager::ProcessCityConquer(City *tCity, Civilization *aCiv, Civilization *tCiv)
{
//    City* city = new City(-1);

    tCity->SetCityID(100*aCiv->getCivIndex() + aCiv->GetCityList().size());
    tCity->SetControllingCiv(aCiv->getCiv());
    tCity->SetCityHealth(100);
    tCity->SetCityFocus(Yield::GOLD);
    tCity->SetCitizenCount(tCity->GetCitizenCount() / 2);

    if(tCity->GetCitizenCount() < 1)
    {
        tCity->SetCitizenCount(1);
    }

    tCity->resetAccumulatedProduction();
    tCity->setCurrentProductionCost(0);
    tCity->setProductionName("No Current Production");
    tCity->setProductionIndex(0);
    tCity->setProductionFinished(false);
    tCity->GetCityTile()->SetControllingCivListIndex(aCiv->getCivIndex());

    //QJsonObject cityInfo;
    //tCity->WriteCitySaveData(cityInfo);

    //city->ReadCitySaveData(cityInfo);
    //city->SetCityTile(map->GetTileAt(city->loadedCityTileIndex));

    if(tCity->IsCityCaptial())
    {
        if(tCity->IsOriginalCapital())
        {
            ns->PostNotification(Notification{0, QString("%1 has lost their Capital! %2 Now controls %3 capitals.").arg(tCiv->GetLeaderName()).arg(aCiv->GetLeaderName()).arg(aCiv->GetCapitalsControlled())});
            aCiv->IncrementCapitalsControlled();
            tCiv->SetCaptialsControlled(0);
            tCity->SetCityAsCapital(false, true);
        }
        else
        {
            tCity->SetCityAsCapital(false, false);
            statusMessage = QString("--------<< %1 have conquered the city of %2 >>--------").arg(aCiv->GetLeaderName()).arg(tCity->GetName());
        }
    }
    else
    {
         statusMessage = QString("--------<< %1 have conquered the city of %2 >>--------").arg(aCiv->GetLeaderName()).arg(tCity->GetName());
    }


/*
    foreach(int i, city->controlledTilesIndex)
    {
        Tile* tile = map->GetTileAt(i);

        if(tile->IsWorked)
        {
            tile->IsWorked = false;
        }

        city->AddControlledTile(tile);
    }

    map->GetTileQueue(city);

    city->SortControlledTiles();

    city->loadUnits("Assets/Units/UnitList.txt");
    city->loadBuildings("Assets/Buildings/BuildingList.txt");

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

    city->UpdateCityStatus();
    city->UpdateCityYield();

    city->SetCityIndex(aCiv->GetCityList().size());

    renderer->RemoveCity(tCity, gameView);
    tCiv->RemoveCity(tCity->GetCityIndex());

    city->InitializeCity();
    renderer->AddCity(city, gameView, true);
    aCiv->AddCity(city);
*/
    tCity->UpdateCityStatus();
    tCity->UpdateCityYield();

    if(civList.at(currentTurn)->isCivAI())
    {
        if(aCiv->getCiv() == civList.at(currentTurn)->getCiv())
            clv->addItem(tCity->GetName());
        else if(tCiv->getCiv() == civList.at(currentTurn)->getCiv())
            clv->takeItem(tCity->GetCityIndex());
    }

    tCiv->RemoveCity(tCity->GetCityIndex());
    aCiv->AddCity(tCity);
}

void GameManager::ProcessAttackUnit()
{
    state = IDLE;

    if(unitToMove->isFortified)
    {
        unitToMove->isFortified = false;
        renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
    }

    targetUnit = civList.at(targetData->od.civIndex)->GetUnitAt(targetData->od.unitIndex);

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});

    selectedTileQueue->enqueue(SelectData{targetUnit->GetTileIndex(), false, false});

    attackUnit->setEnabled(false);
    rangeAttack->setEnabled(false);

    if(map->GetTileAt(unitToMove->GetTileIndex())->GetTileType() == WATER)
        uc->Attack(unitToMove, targetUnit, true);
    else
        uc->Attack(unitToMove, targetUnit, false);


    if(unitToMove->GetHealth() <= 0)
    {
        if(unitToMove->GetOwner() == civList.at(0)->getCiv())
        {
            ns->PostNotification(Notification{1, QString("Your %1 has been killed!").arg(unitToMove->GetName())});

            QMediaPlayer *musicPlayer = new QMediaPlayer();
            musicPlayer->setMedia(QUrl::fromLocalFile("Assets/Sound/notificationunitkilled.wav"));
            musicPlayer->setVolume(50);
            musicPlayer->play();
        }

        //These statements handle removing a unit from the game if it is killed.
        renderer->SetFortifyIcon(unitToMove->GetTileIndex(), true);
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(), false);
        int listIndex = unitToMove->GetOwningCivIndex();
        //map->GetTileAt(unitToMove->GetTileIndex())->SetOccupyingCivListIndex(-1);
        //map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
//        unitToMove->GetUnitTile()->SetOccupyingUnit(NULL);
        map->SetOccupantDataAt(unitToMove->GetTileIndex(), DEFAULT_OCCUPANT);
        renderer->RemoveUnit(unitToMove, gameView);
        civList.at(listIndex)->RemoveUnit(unitToMove->GetUnitListIndex());
    }

    if(targetUnit->GetHealth() <= 0)
    {
        if(targetUnit->GetOwner() == civList.at(0)->getCiv())
        {
            ns->PostNotification(Notification{1, QString("Your %1 has been killed!").arg(targetUnit->GetName())});

            QMediaPlayer *musicPlayer = new QMediaPlayer();
            musicPlayer->setMedia(QUrl::fromLocalFile("Assets/Sound/notificationunitkilled.wav"));
            musicPlayer->setVolume(50);
            musicPlayer->play();
        }

        //These statements handle removing a unit from the game if it is killed.
        renderer->SetFortifyIcon(targetUnit->GetTileIndex(), true);
        renderer->SetUnitNeedsOrders(targetUnit->GetTileIndex(), false);
        int listIndex = unitToMove->GetOwningCivIndex();
        //map->GetTileAt(unitToMove->GetTileIndex())->SetOccupyingCivListIndex(-1);
        //map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
//        unitToMove->GetUnitTile()->SetOccupyingUnit(NULL);
        map->SetOccupantDataAt(targetUnit->GetTileIndex(), DEFAULT_OCCUPANT);
        renderer->RemoveUnit(targetUnit, gameView);
        civList.at(listIndex)->RemoveUnit(targetUnit->GetUnitListIndex());
    }

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
                OccupantData tod = map->GetODFromTileAt(tile->GetTileIndex());
                if((tod.OccupantNation != NO_NATION) && (tod.civIndex == 0))
                {
                    Unit *unit = uc->FindUnitAtTile(tile, civList.at(currentTurn)->GetUnitList());

                    foreach(Tile* outside, city->tileQueue)
                    {
                        OccupantData outsideOd = map->GetODFromTileAt(outside->GetTileIndex());
                        if(outsideOd.OccupantNation == NO_NATION)
                        {
//                            map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
//                            map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);
//                            unit->GetUnitTile()->SetOccupyingUnit(NULL);
                            map->SetOccupantDataAt(unit->GetTileIndex(), DEFAULT_OCCUPANT);

//                            if(map->GetTileAt(unit->GetTileIndex())->Selected)
//                                    map->GetTileAt(unit->GetTileIndex())->Selected = false;

                            //update the unit's position
//                            unit->SetPositionIndex(outside->GetTileIndex());
                            unit->SetPosition(tile);
//                            map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(currentTurn);

                            // Set the data for the unit's new tile
//                            map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

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
                        aiUnit->UpdatePath(map);
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
                OccupantData tod = map->GetODFromTileAt(tile->GetTileIndex());
                if((tod.OccupantNation != NO_NATION) && (tod.civIndex == makePeaceWithIndex))
                {
                    Unit *unit = uc->FindUnitAtTile(tile, civList.at(makePeaceWithIndex)->GetUnitList());

                    if(unit->isPathEmpty())
                    {
                        foreach(Tile* outside, city->tileQueue)
                        {
                            OccupantData outsideOd = map->GetODFromTileAt(outside->GetTileIndex());
                            if(outsideOd.OccupantNation == NO_NATION)
                            {
//                                map->GetTileAt(unit->GetTileIndex())->ContainsUnit = false;
//                                map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(-1);
//                                unit->GetUnitTile()->SetOccupyingUnit(NULL);
                                map->SetOccupantDataAt(unit->GetTileIndex(), DEFAULT_OCCUPANT);

    //                            if(map->GetTileAt(unit->GetTileIndex())->Selected)
    //                                    map->GetTileAt(unit->GetTileIndex())->Selected = false;

                                //update the unit's position
    //                            unit->SetPositionIndex(outside->GetTileIndex());
                                unit->SetPosition(tile);
    //                            map->GetTileAt(unit->GetTileIndex())->SetOccupyingCivListIndex(currentTurn);

                                // Set the data for the unit's new tile
    //                            map->GetTileAt(unit->GetTileIndex())->ContainsUnit = true;

                                unit->RequiresOrders = true;
                                renderer->UpdateUnits(map, gameView, unit, true);
                                break;
                            }
                        }
                    }
                    else
                    {
                        uc->MoveUnit(unit, map);
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

void GameManager::Victory()
{
    QMessageBox* mBox = new QMessageBox();
    mBox->setWindowFlags(Qt::FramelessWindowHint);
    mBox->setFixedSize(200, 500);
    mBox->setText("You Win!");
    mBox->exec();
    this->playersAliveCount = 0;
    this->close();
    renderer->PrepareForDelete(gameView);
}

void GameManager::Defeat()
{
    QMessageBox* mBox = new QMessageBox();
    mBox->setWindowFlags(Qt::FramelessWindowHint);
    mBox->setFixedSize(200, 500);
    mBox->setText("You Lose!");
    mBox->exec();
    this->playersAliveCount = 0;
    //Defeat screen?
    this->close();
    renderer->PrepareForDelete(gameView);
}

void GameManager::closeGame()
{
    this->close();
#ifndef __APPLE__
    QFuture<void> save = QtConcurrent::run(this, GameManager::SaveGame);
#else
    this->SaveGame();
#endif
    renderer->PrepareForDelete(gameView);

#ifndef __APPLE__
    save.waitForFinished();
#endif
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
    if(!cityScreenVisible && (city != NULL))
    {
#ifdef DEBUG
        qDebug() << "Opening City screen";
#endif

        if(cityScreen != NULL)
        {
            delete cityScreen;
        }

        cityScreen = new CityScreen(this);
        cityScreen->loadBuildings("Assets/Data/buildings.json");
        cityScreen->loadUnits("Assets/Data/units.json");
        cityScreen->getCityInfo(city);
        cityScreen->getCivInfo(civList.at(0));
        cityScreen->getGameView(gameView);
        cityScreen->getMapInfo(map);
        cityScreen->getRenderer(renderer);
        cityScreen->getGold(civList.at(0)->GetTotalGold());
        cityScreen->updateList(city->getNumberOfBuildings(), civList.at(0)->getCurrentTech()->getIndex() - 1);
        cityScreen->updateWidget();

        gameView->centerOn(city->GetCityTile()->GetCenter());
        gameView->SetCityScreenZoom();

        cityScreen->setGeometry(gameView->pos().x() + 2, gameView->pos().y() + 2, gameView->width(), gameView->height());

        toggleWidgetVisiblityAll();
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);

        gameView->setDragMode(QGraphicsView::NoDrag);

        cityScreen->show();
        cityScreenVisible = true;
    }
    else
    {
#ifdef DEBUG
        qDebug() << "Closing City Screen";
#endif
        this->goldFocus->setEnabled(false);
        this->productionFocus->setEnabled(false);
        this->scienceFocus->setEnabled(false);
        this->cultureFocus->setEnabled(false);
        this->foodFocus->setEnabled(false);
        this->showTechTreeButton->setEnabled(true);

        gameView->setDragMode(QGraphicsView::ScrollHandDrag);

        toggleWidgetVisiblityAll();
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);

        // deleting the city screen here causes a crash
        // (pure virtual method called)
//        if(cityScreen != NULL)
//            delete cityScreen;

        cityScreen = NULL;
        cityScreenVisible = false;
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

    //if(processedData.newData || state == FOUND_CITY)
    //{
        this->NewUpdateTileData();
    //}

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
}

void GameManager::moveUnitTo()
{
#ifdef DEBUG
    qDebug() << "MoveUnitTo()";
#endif

    if(moveUnit->isEnabled())
    {
        state = MOVE_UNIT;
        relocateUnit = true;
        inPrimaryState = false;
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
        techTree->setGeometry(150, 25, gameView->width() - 6, gameView->height() - 150);
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
        diplo->setGeometry(gameView->pos().x() + 5, gameView->pos().y() + 2, gameView->width(), gameView->height());
        diplo->show();
        diploVisible = true;
    }
    else
    {
        diplo->hide();
        diplo->UpdateLeader(0);
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
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
}

void GameManager::buildNewFarm()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),FARM))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(FARM, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewPlantation()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),PLANTATION))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(PLANTATION, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewTradePost()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),TRADE_POST))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(TRADE_POST, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }
    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewMine()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),MINE))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(MINE, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewCamp()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),CAMP))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(CAMP, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewPasture()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),PASTURE))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(PASTURE, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewOilWell()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),OIL_WELL))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(OIL_WELL, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewFishBoat()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),FISHING_BOAT))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(FISHING_BOAT, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::buildNewQuarry()
{
    if(uc->BuildImprovement(unitToMove,map->GetTileAt(unitToMove->GetTileIndex()),civList.at(currentTurn),QUARRY))
    {
//        map->GetTileAt(unitToMove->GetTileIndex())->ContainsUnit = false;
        renderer->SetUnitNeedsOrders(unitToMove->GetTileIndex(),false);
        renderer->RemoveUnit(unitToMove,gameView);
        renderer->SetTileImprovement(QUARRY, map->GetTileAt(unitToMove->GetTileIndex()), gameView);
        setUnitButtonVisibility(NO_BUTTONS_VISIBLE);
    }

    selectedTileQueue->enqueue(SelectData{unitToMove->GetTileIndex(), false, false});
    this->redrawTile = true;
}

void GameManager::attackMelee()
{
#ifdef DEBUG
    qDebug() << "attackMelee()";
#endif

    state = ATTACK_MELEE;
    inPrimaryState = false;
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
#ifdef DEBUG
    qDebug() << "AttackCity()";
#endif

    state = ATTACK_CITY;
    inPrimaryState = false;
}

void GameManager::RangeAttack()
{
#ifdef DEBUG
    qDebug() << "RangeAttack()";
#endif

    state = ATTACK_RANGE;
    inPrimaryState = false;
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
void GameManager::WarDeclared(int targetCivIndex)
{
    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetCivIndex)->GetLeaderName())});

//    if(targetTile->HasCity)
//    {
//        state = FIND_CITY;
//    }
//    else if(targetTile->ContainsUnit())
//    {
//        state = IDLE;
//        ProcessAttackUnit();
//    }

    civList.at(currentTurn)->SetAtWar(targetCivIndex);
    civList.at(targetCivIndex)->SetAtWar(currentTurn);

    diplo->DeclareWarOn(civList.at(targetCivIndex)->getCiv(), targetCivIndex, civList.at(currentTurn)->getCiv(), currentTurn);
    playerToWar = true;
}

void GameManager::WarAvoided()
{
    state = IDLE;
    relocateUnit = false;
    playerToWar = false;
}

/*
 * WarByInvasion is used when the player moves into
 * territory controlled by an AI.
 */
void GameManager::WarByInvasion(int targetCivIndex)
{
    ns->PostNotification(Notification{5, QString("%1 has declared war on %2!").arg(civList.at(currentTurn)->GetLeaderName()).arg(civList.at(targetCivIndex)->GetLeaderName())});

    state = INVADE;
    civList.at(currentTurn)->SetAtWar(targetCivIndex);
    civList.at(targetCivIndex)->SetAtWar(currentTurn);

    diplo->DeclareWarOn(civList.at(targetCivIndex)->getCiv(), targetCivIndex, civList.at(0)->getCiv(), 0);
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
				if(devModeOn && map->GetTileAt(i)->CanAlwaysBeSeen)
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

void GameManager::enterDevMode()
{
    if(!devModeOn)
    {
        statusMessage = "--------<< Loading Developer Mode >>--------";
        renderer->EnableDevMode(gameView);
        toggleFog();
        devModeOn = true;
    }
    else
    {
        statusMessage = "--------<< Leaving Developer Mode >>--------";
        toggleFog();

        for(int i = 0; i < map->GetBoardSize(); i++)
            if(!map->GetTileAt(i)->DiscoveredByPlayer)
                renderer->DisableDevMode(i, gameView);

        devModeOn = false;
    }

    statusMessage = "";
}
