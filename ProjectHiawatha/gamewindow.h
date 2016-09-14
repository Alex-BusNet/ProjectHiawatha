#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qevent.h>
#include "renderer.h"
#include <QGraphicsItem>
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
    QGraphicsPixmapItem *tilePixmap;
    QGraphicsPixmapItem *pixmap;
    QGraphicsItem *item;
    QGraphicsTextItem *text;
signals:

public slots:
//    void updateGameWindow();
};

#endif // GAMEWINDOW_H
