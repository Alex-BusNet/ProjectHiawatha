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

    renderPlusOne = new QPushButton("Render Scale++");
    connect(renderPlusOne, SIGNAL(clicked(bool)), this, SLOT(renderPlus()));

    renderMinusOne = new QPushButton("Render Scale--");
    connect(renderMinusOne, SIGNAL(clicked(bool)), this, SLOT(renderMinus()));

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

//    gameView.installEventFilter(this);
    gameView.setMouseTracking(true);
    gameView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
        tilePixmap.at(i)->setScale(0.32f * (*(map->GetTileAt(i))).GetRenderScale());
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }

    proxy.push_back(game->addWidget(exitGame));
    proxy.push_back(game->addWidget(renderPlusOne));
    proxy.push_back(game->addWidget(renderMinusOne));
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

void GameWindow::wheelEvent(QGraphicsSceneWheelEvent *e)
{
        qDebug() << "Wheel Event: " << e->delta();

//        for(int i = 0; i < map->GetBoardSize(); i++)
//        {
//            map->GetTileAt(i)->SetRenderScale(e->delta());
//        }
}

bool GameWindow::eventFilter(QObject *watched, QEvent *event)
{
//    qDebug() << "Event filter object: " << watched->metaObject()->className();
//    qDebug() << "Event type: " << event->type();

    if((watched->metaObject()->className() == gameView.metaObject()->className()) && event->type() == QEvent::Wheel)
    {
//        for(int i = 0; i < map->GetBoardSize(); i++)
//        {
//            map->GetTileAt(i)->SetRenderScale(2);
//        }

        return true;
    }
    else if((watched->metaObject()->className() == gameView.metaObject()->className()) && event->type() == QEvent::MouseMove)
    {
        qDebug() << "Mouse Move Event Filter";
    }

    return false;
}

void GameWindow::closeGame()
{
    gameView.hide();
}

void GameWindow::renderPlus()
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        map->GetTileAt(i)->IncreaseRenderScale(1);
    }

    qDebug() << "Render Scale: " << map->GetTileAt(0)->GetRenderScale();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tilePixmap.at(i)->setScale(0.32f * (*(map->GetTileAt(i))).GetRenderScale());
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }
}

void GameWindow::renderMinus()
{
    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        map->GetTileAt(i)->IncreaseRenderScale(-1);
    }

    qDebug() << "Render Scale: " << map->GetTileAt(0)->GetRenderScale();

    for(int i = 0; i < map->GetBoardSize(); i++)
    {
        tilePixmap.at(i)->setScale(0.32f * (*(map->GetTileAt(i))).GetRenderScale());
        tilePixmap.at(i)->setPos(map->GetTileAt(i)->GetTexturePoint());
    }
}

//void GameWindow::updateGameWindow()
//{
//    gameView.scene()->update(gameView.sceneRect());
//}




