#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
#include "renderer.h"
#include <QGraphicsView>

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = 0);

private:
    Renderer *renderer;
    void paintEvent(QPaintEvent *event);
    QTimer *updateTimer;
    void mouseMoveEvent(QMouseEvent *event);

    QGraphicsScene *game;
    QGraphicsView gameView;
    QGraphicsPolygonItem *tile;
    QGraphicsRectItem *tileRect;
signals:

public slots:
//    void updateGameWindow();
};

#endif // GAMEWINDOW_H
