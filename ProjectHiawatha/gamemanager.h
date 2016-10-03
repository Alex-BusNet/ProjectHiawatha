#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
#include "cityscreen.h"
#include "gameview.h"
#include "map.h"
#include "renderer.h"
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
    explicit GameManager(QWidget *parent = 0, bool fullscreen = false, int mapSizeX = 33, int mapSizeY  = 42);

private:
    Renderer *renderer;
    Map *map;
    QTimer *updateTimer;

    GameView *gameView;

    QVector<QGraphicsPolygonItem*> tile;

    QVector<QGraphicsRectItem*> guiRects;
    QVector<QGraphicsTextItem*> stringData;
    QVector<QGraphicsProxyWidget*> proxy;

    QPushButton *exitGame;
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;
    QPushButton *showDummyCityScreen;

    CityScreen *cityScreen;

    QHBoxLayout *hLayout;
    QHBoxLayout *gameLayout;
    QVBoxLayout *vLayout;

    QString *YieldDisplay;

    int zoomScale;
    bool cityScreenVisible;

    void paintEvent(QPaintEvent *event);

    void mouseReleaseEvent(QMouseEvent *e);

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
    void showCity();
    void updateTiles();
};

#endif // GAMEWINDOW_H
