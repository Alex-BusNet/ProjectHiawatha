#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent, bool fullscreen) : QWidget(parent)
{
    qDebug() << "Game Window c'tor called";

    if(!fullscreen)
    {
        QWidget::setFixedSize(1200, 900);
    }
    else
    {
        QWidget::setFixedSize(1920, 1080);
    }

    exitGame = new QPushButton("Exit To Menu");
    connect(exitGame, SIGNAL(clicked(bool)), this, SLOT(closeGame()));
    exitGame->setShortcut(QKeySequence(Qt::Key_Escape));

    renderPlusOne = new QPushButton("Zoom in");
    connect(renderPlusOne, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    renderPlusOne->setShortcut(QKeySequence(Qt::Key_Up));

    renderMinusOne = new QPushButton("Zoom out");
    connect(renderMinusOne, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));
    renderMinusOne->setShortcut(QKeySequence(Qt::Key_Down));

//    updateTimer = new QTimer();
//    updateTimer->setInterval(50);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

//    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    map = new Map();
    map->InitHexMap();
    map->InitTerrain();

    qDebug() << "Done." << endl;

    /*=================================================================
     * Some of this needs to be moved to Renderer::DrawHexScene()
     * the function parameters also need to be adjusted. -Port
    */
    game = new QGraphicsScene(this);
    gameView.setScene(game);

    exitGame->setGeometry(this->width() - 100, this->height() - 50, 90, 30);
    renderPlusOne->setGeometry(this->width() - 100, this->height() - 100, 90, 30);
    renderMinusOne->setGeometry(this->width() - 100, this->height() - 150, 90, 30);

    gameView.installEventFilter(this);
    gameView.setMouseTracking(true);
    gameView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gameView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView.setDragMode(QGraphicsView::ScrollHandDrag);

    if(!fullscreen)
    {
        gameView.setMinimumWidth(1200);
        gameView.setMinimumHeight(900);

        gameView.setMaximumHeight(900);
        gameView.setMaximumWidth(1200);
    }
    else
    {
        gameView.setWindowState(Qt::WindowFullScreen);
    }

    gameView.setWindowFlags(Qt::FramelessWindowHint);

    gameView.show();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tile.push_back(game->addPolygon(map->GetTileAt(i)->GetTilePolygon()));

        // This is going to change. I have an idea of how
        // I may be able to make this work. -Port
        tilePixmap.push_back(game->addPixmap((*(map->GetTilePixmap(i)))));
        tilePixmap.at(i)->setScale(0.32f);
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }

    proxy.push_back(game->addWidget(exitGame));
    proxy.push_back(game->addWidget(renderPlusOne));
    proxy.push_back(game->addWidget(renderMinusOne));

    zoomScale = 1;
    //==================================================================
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Move Event";

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

void GameWindow::wheelEvent(QWheelEvent *e)
{
        qDebug() << "Wheel Event: " << e->delta();
        if(e->delta() > 0)
        {
            zoomIn();
        }
        else if (e->delta() < 0)
        {
            zoomOut();
        }
}

bool GameWindow::eventFilter(QObject *watched, QEvent *event)
{
    qDebug() << "Event Filter type: " << event->type();

    if((watched->metaObject()->className() == gameView.metaObject()->className()) && event->type() == QEvent::Wheel)
    {
        this->wheelEvent(static_cast<QWheelEvent*>(event));
        return true;
    }
    else if((watched->metaObject()->className() == gameView.metaObject()->className()) && event->type() == QEvent::MouseMove)
    {
        qDebug() << "Mouse Move Event Filter";
        return true;
    }

    return false;
}

void GameWindow::closeGame()
{
    gameView.hide();
}

void GameWindow::zoomIn()
{
    if(zoomScale < 7)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1.2, 1.2);
        zoomScale++;
    }
}

void GameWindow::zoomOut()
{
    if(zoomScale > 1)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1/ 1.2, 1 / 1.2);
        zoomScale--;
    }
}

//void GameWindow::updateGameWindow()
//{
//    gameView.scene()->update(gameView.sceneRect());
//}




