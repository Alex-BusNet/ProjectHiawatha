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

    //=============
    //Dev Buttons
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;
    QPushButton *showDummyCityScreen;
    QPushButton *moveUnit;
    //=============

    CityScreen *cityScreen;

    QHBoxLayout *hLayout;
    QHBoxLayout *gameLayout;
    QVBoxLayout *unitControlButtons;
    QVBoxLayout *playerControlButtons;
    QVBoxLayout *vLayout;

    QString *YieldDisplay;

    int zoomScale;
    int currentTurn;

    bool cityScreenVisible;
    bool relocateUnit;
    bool updateUnitPos;
    bool turnEnded;

    void InitCivs(Nation player, int numAI);
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);

    void TurnController();
    void StartTurn();
    void EndTurn();

    void InitButtons();
    void InitLayouts();

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
    void showCity();
    void updateTiles();
    void moveUnitTo();
    void nextTurn();
};

#endif // GAMEWINDOW_H
