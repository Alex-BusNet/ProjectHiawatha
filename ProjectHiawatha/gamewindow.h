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

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = 0, bool fullscreen = false);

private:
    Renderer *renderer;
    Map *map;
    QTimer *updateTimer;

    // These event handlers most likely are going to be removed from here. -Port
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *e);

//    QGraphicsScene *game;
//    QGraphicsView gameView;
    GameView gameView;

    QVector<QGraphicsPolygonItem*> tile;
    QVector<QGraphicsPixmapItem*> tilePixmap;
    QVector<QGraphicsPixmapItem*> unitPixmap;
    QVector<QGraphicsPixmapItem*> cityPixmap;

    QPushButton *exitGame;
    QPushButton *renderPlusOne;
    QPushButton *renderMinusOne;

    QVector<QGraphicsProxyWidget*> proxy;

    int zoomScale;

signals:

public slots:
    void closeGame();

    void zoomIn();
    void zoomOut();
    void updateGameWindow();
};

#endif // GAMEWINDOW_H
