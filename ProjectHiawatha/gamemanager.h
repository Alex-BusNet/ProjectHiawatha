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
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPushButton>

class GameManager : public QWidget
{
    Q_OBJECT
public:
    explicit GameManager(QWidget *parent = 0, bool fullscreen = false, int mapSizeX = 33, int mapSizeY  = 42, Nation player = India, int numAI = 2);

private:
    Renderer *renderer;
    Map *map;
    UnitController *uc;
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
    //=============
    //Dev Buttons
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;
    QPushButton *showDummyCityScreen;
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

    QPixmap *goldPix, *prodPix, *sciPix, *foodPix, *culPix;

    Unit* unitToMove;

    QString *YieldDisplay;


    int zoomScale;
    int currentTurn, gameTurn;
    float year, yearPerTurn;

    bool cityScreenVisible;
    bool techTreeVisible;
    bool relocateUnit;
    bool updateUnitPos;
    bool turnEnded;
//    bool findUnit;

    Nation playerCiv;

    void InitCivs(Nation player, int numAI);
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);

    void TurnController();
    void StartTurn();
    void EndTurn();

    void InitButtons();
    void InitLayouts();
    void InitYieldDisplay();

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
    void showCity();
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
};

#endif // GAMEWINDOW_H
