#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    QWidget::setFixedSize(1200, 900);

//    updateTimer = new QTimer();
//    updateTimer->setInterval(500);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    renderer->InitHexMap();
//    renderer->InitMap();

    qDebug() << "Done." << endl;
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
//    renderer->DrawMap(paint);
    renderer->DrawHex(paint);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->pos().x() > 1150)
    {
        //Scroll to the right (continuous)
    }
    else if(event->pos().y() > 850)
    {
        //Scroll down to bottom of map
    }
    else if(event->pos().x() < 50)
    {
        //Scroll to the left(continuous)
    }
    else if(event->pos().y() < 50)
    {
        //Scroll up to top of map
    }
}

//void GameWindow::updateGameWindow()
//{
//    QPainter paint(this);
//    renderer->DrawHex(paint);
//}




