#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
#include "cityscreen.h"
#include "gameview.h"
#include "map.h"
#include "renderer.h"
#include "unitcontroller.h"
#include "ai_controller.h"
#include "techtree.h"
#include <QFuture>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QQueue>
#include "notificationsystem.h"
#include "tiledata.h"
#include <chrono>
#include "about.h"

class GameManager : public QWidget
{
    Q_OBJECT
public:
    explicit GameManager(QWidget *parent = 0, bool fullscreen = false, int mapSizeX = 33, int mapSizeY  = 42, Nation player = India, int numAI = 2);

private:

    Renderer *renderer;
    Map *map;
    UnitController *uc;
    QListWidget *clv;
    NotificationSystem *ns;
    About *about;

    QTimer *updateTimer;

    AI_Controller *ac;
    Civilization* currentCiv;
    GameView *gameView;

    QVector<QGraphicsPolygonItem*> tile;
    QVector<QGraphicsRectItem*> guiRects;
    QVector<QGraphicsTextItem*> stringData;
    QVector<QGraphicsProxyWidget*> proxy;

    QVector<Civilization*> civList;

    QVector<Unit*> currentUnitList;
    QVector<City*> currentCityList;

    QPushButton *exitGame;
    QPushButton *endTurn;
    QPushButton *moveUnit;
    QPushButton *showTechTreeButton;
    QPushButton *buildFarm;
    QPushButton *buildMine;
    QPushButton *buildTradePost;
    QPushButton *buildPlantation;
    QPushButton *buildRoad;
    QPushButton *foundCity;
    QPushButton *attackUnit;
    QPushButton *attackCity;
    QPushButton *rangeAttack;
    QPushButton *fortifyUnit;
    QPushButton *help;

    QFuture<void> civInit, mapInit;

    //=============
    //Dev Buttons
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;
    QPushButton *showDummyCityScreen;
    QPushButton *goldFocus;
    QPushButton *productionFocus;
    QPushButton *scienceFocus;
    QPushButton *foodFocus;
    QPushButton *cultureFocus;
    //=============

    CityScreen *cityScreen;
    TechTree *techTree;

    QHBoxLayout *hLayout;
    QHBoxLayout *gameLayout;
    QVBoxLayout *unitControlButtons;
    QVBoxLayout *playerControlButtons;
    QVBoxLayout *vLayout;
    QHBoxLayout *Yieldinfo;

    QLabel *goldLabel, *goldText;
    QLabel *prodLabel, *prodText;
    QLabel *sciLabel, *sciText;
    QLabel *foodLabel, *foodText;
    QLabel *culLabel, *culText;
    QLabel *techLabel, *techText;
    QLabel *endGameProgress;
    QString *endGameText;

    QPixmap *goldPix, *prodPix, *sciPix, *foodPix, *culPix;
    QRect *playerInfoRect, *gameStatusRect;

    Unit* unitToMove, *targetUnit;
    City* targetCity;

    QString *YieldDisplay, statusMessage;

    QQueue<SelectData> *selectedTileQueue, *tileModifiedQueue;

    Tile *unitTile, *targetTile;

    int zoomScale;
    int currentTurn, gameTurn;
    int foundCityIndex;
    int playersAliveCount;
    float year, yearPerTurn;

    QString currentProductionName;
    bool cityScreenVisible;
    bool techTreeVisible;
    bool relocateUnit, fortify;
    bool updateUnitPos;
    bool turnEnded, turnStarted;
    bool countTime;
    bool citySelected, redrawTile, isTileSelected, cityTileSelected;
    bool findUnit, findCity, foundACity, aiFoundCity;
    bool attackNearby, attackRange, attackEnemyCity;
    bool focusChanged;
    bool invade;

    Nation playerCiv;
    ActionState state;

    void InitCivs(Nation player, int numAI);
    void paintEvent(QPaintEvent *event);

    void TurnController();
    void StartTurn();
    void EndTurn();

    void UpdateTileData();

    void InitButtons();
    void InitLayouts();
    void InitYieldDisplay();

    void ProcessCityConquer(City* tCity, Civilization* aCiv, Civilization* tCiv);

    TileData processedData;

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
    void showCity(City* city);
    void updateTiles();
    void moveUnitTo();
    void nextTurn();
    void showTechTree();
    void foundNewCity();
    void buildNewRoad();
    void buildNewFarm();
    void buildNewPlantation();
    void buildNewTradePost();
    void buildNewMine();
    void attackMelee();
    void SetGoldFocus();
    void SetProdFocus();
    void SetScienceFocus();
    void SetFoodFocus();
    void SetCultureFocus();
    void AttackCity();
    void RangeAttack();
    void Fortify();
    void WarDeclared();
    void WarAvoided();
    void WarByInvasion();
    void OpenHelp();

    void parseItem(QListWidgetItem *item);
};

#endif // GAMEWINDOW_H
