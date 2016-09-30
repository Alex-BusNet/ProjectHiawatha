#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
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
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsPixmapItem*> cityPixmap;
    QVector<QGraphicsRectItem*> guiRects;
    QVector<QGraphicsTextItem*> stringData;

    QPushButton *exitGame;
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;

    QVector<QGraphicsProxyWidget*> proxy;

    QRect *YieldDisplay;

    int zoomScale;

    void setScene(QGraphicsScene *scene);

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
//    void updateGameWindow();
};

#endif // GAMEWINDOW_H
