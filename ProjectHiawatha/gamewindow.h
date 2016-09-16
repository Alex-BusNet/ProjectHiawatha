#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
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
//    QTimer *updateTimer;

    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);

    QGraphicsScene *game;
    QGraphicsView gameView;

    QVector<QGraphicsPolygonItem*> tile;
    QVector<QGraphicsPixmapItem*> tilePixmap;

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
//    void updateGameWindow();
};

#endif // GAMEWINDOW_H
