#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
#include "datatypes.h"
#include "cityscreen.h"
#include "gameview.h"
//#include "map.h"
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
#include <QMessageBox>
#include <QPushButton>
#include <QQueue>
#include "notificationsystem.h"
#include <chrono>
#include "about.h"
#include "diplomacy.h"
#include <QShortcut>
#include <QDesktopWidget>

class GameManager : public QWidget
{
    Q_OBJECT
public:
    explicit GameManager(QWidget *parent = 0, bool fullscreen = false, int mapSizeX = 33, int mapSizeY  = 42, Nation player = India, int numAI = 2);
    GameManager(QWidget *parent = 0, bool fullscreen = false, bool loadLatest = true);
    ~GameManager();
private:

    Renderer *renderer;
    Map *map;
    UnitController *uc;
    QListWidget *clv;
    NotificationSystem *ns;
    About *about;
    Diplomacy *diplo;
    QDesktopWidget widget;
    QWidget *unitButtonsWidget;
    QWidget *playerButtonsWidget;
    QTimer *updateTimer;

    AI_Controller *ac;
    GameView *gameView;

    QVector<QGraphicsTextItem*> stringData;

    QVector<Civilization*> civList;

    QPushButton *exitGame;
    QPushButton *endTurn;
    QPushButton *moveUnit;
    QPushButton *showTechTreeButton;
    QPushButton *showDiplomacy;
    QPushButton *buildFarm;
    QPushButton *buildMine;
    QPushButton *buildTradePost;
    QPushButton *buildPlantation;
    QPushButton *buildRoad;
    QPushButton *buildQuarry;
    QPushButton *buildFishBoat;
    QPushButton *buildCamp;
    QPushButton *buildOilWell;
    QPushButton *buildPasture;
    QPushButton *foundCity;
    QPushButton *attackUnit;
    QPushButton *attackCity;
    QPushButton *rangeAttack;
    QPushButton *fortifyUnit;
    QPushButton *help;
    QPushButton *toggleFoW;
    QShortcut *devMode;

    QFuture<void> civInit, mapInit;

    QPushButton *goldFocus;
    QPushButton *productionFocus;
    QPushButton *scienceFocus;
    QPushButton *foodFocus;
    QPushButton *cultureFocus;

    CityScreen *cityScreen;
    TechTree *techTree;

    QHBoxLayout *hLayout;
    QHBoxLayout *gameLayout;
    QVBoxLayout *unitControlButtons;
    QVBoxLayout *playerControlButtons;
    QVBoxLayout *vLayout;
    QHBoxLayout *Yieldinfo;

    QSpacerItem *spacer;

    QLabel *goldLabel, *goldText;
    QLabel *prodLabel, *prodText;
    QLabel *sciLabel, *sciText;
    QLabel *foodLabel, *foodText;
    QLabel *culLabel, *culText;
    QLabel *techLabel, *techText;
    QLabel *endGameProgress;
    QString *endGameText;
    QMessageBox *warbox;

    QPixmap *goldPix, *prodPix, *sciPix, *foodPix, *culPix;
    QRect *playerInfoRect, *gameStatusRect;

    Unit* unitToMove, *targetUnit;
    City* targetCity;

    QString statusMessage;

    QQueue<SelectData> *selectedTileQueue, *tileModifiedQueue;
    QQueue<ViewData> *viewUpdateTiles;

    QJsonArray BuildingData;
    QJsonArray UnitData;
    QJsonArray TechData;

    //Tile *unitTile, *targetTile;

    int zoomScale;
    int currentTurn, gameTurn;
    int foundCityIndex;
    int playersAliveCount;
    float year, yearPerTurn;

    unsigned int unitControlButtonsVisible = 0xEFFF; // 15 items to toggle visibility.

    QString currentProductionName;
    bool cityScreenVisible;
    bool techTreeVisible;
    bool diploVisible;
    bool relocateUnit, fortify;
    bool updateUnitPos;
    bool turnEnded, turnStarted;
    bool countTime;
    bool citySelected;
    bool redrawTile;
    bool isTileSelected;
    bool cityTileSelected;
    bool updateFoW;
    bool focusChanged;
    bool toggleOn, devModeOn;
    bool civLoadFailed;
    bool renderInitFinished;
    bool playerToWar; // Used for return value from WarCheck();

    Nation playerCiv;
    ActionState state;
    bool inPrimaryState; // true = Primary action state (select), false = Secondary action state (Move, attack)
    MapData *clickedData, *targetData, *lastData;

    void InitCivs(Nation player, int numAI);
    void LoadCivs();
    void paintEvent(QPaintEvent *event);

    void TurnController();
    void StartTurn();
    void EndTurn();

    void NewUpdateTileData();
    void UpdateTileData();

    void InitVariables(bool fullscreen);
    void LoadJsonData();
    void InitButtons();
    void InitLayouts();
    void InitYieldDisplay();
    void InitRenderData();
    void DeinitRenderer();
    void SaveGame();

    void toggleWidgetVisiblityAll();
    void setUnitButtonVisibility(unsigned int visibleButtons);

    bool WarCheck(MapData *md); //Tile* t);
    void ProcessCityConquer(City* tCity, Civilization* aCiv, Civilization* tCiv);
    void ProcessAttackUnit();
    void ProcessPeace(int makePeaceWithIndex);

    bool AcceptsPeace(Civilization* ai);

    void Victory();
    void Defeat();

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
    void toggleDiplomacy();
    void foundNewCity();
    void buildNewRoad();
    void buildNewFarm();
    void buildNewPlantation();
    void buildNewTradePost();
    void buildNewMine();
    void buildNewCamp();
    void buildNewPasture();
    void buildNewOilWell();
    void buildNewFishBoat();
    void buildNewQuarry();
    void attackMelee();
    void SetGoldFocus();
    void SetProdFocus();
    void SetScienceFocus();
    void SetFoodFocus();
    void SetCultureFocus();
    void AttackCity();
    void RangeAttack();
    void Fortify();
    void WarDeclared(int targetCivIndex);
    void WarAvoided();
    void WarByInvasion(int targetCivIndex);
    void WarByDiplomacy();
    void MakePeace();
    void OpenHelp();

    void parseItem();

private slots:
    void toggleFog();
    void enterDevMode();
};

#endif // GAMEWINDOW_H
