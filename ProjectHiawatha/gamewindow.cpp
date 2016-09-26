#include "gamewindow.h"
#include <QDebug>

QPen gmPen(Qt::black);
QBrush gmBrush(Qt::black);

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
//    updateTimer->setInterval(1);
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGameWindow()));
//    updateTimer->start();

//    QWidget::setMouseTracking(true);

    qDebug() << "Creating new Renderer";

    renderer = new Renderer();

    qDebug() << "Done.\nInitializing Map";

    map = new Map();
    map->InitHexMap();
//    map->InitTerrain();

    qDebug() << "Done.\nSetting up Scene.";

    /*=================================================================
     * Some of this needs to be moved to Renderer::DrawHexScene()
     * the function parameters also need to be adjusted. -Port
    */

    qDebug() << "Done.\nAdding buttons to screen.";
    exitGame->setGeometry(this->width() - 100, this->height() - 50, 90, 30);
    renderPlusOne->setGeometry(this->width() - 100, this->height() - 100, 90, 30);
    renderMinusOne->setGeometry(this->width() - 100, this->height() - 150, 90, 30);
    YieldDisplay = new QRect(0,0, 500, 20);

    proxy.push_back(gameView.addWidget(exitGame));
    proxy.push_back(gameView.addWidget(renderPlusOne));
    proxy.push_back(gameView.addWidget(renderMinusOne));

    qDebug() << "Done.\nDrawing map.";
    renderer->DrawHexScene(map, tile, tilePixmap, &gameView);

    renderer->DrawTestUnits(map, unitPixmap, &gameView);

    renderer->DrawTestCities(map, cityPixmap, &gameView);

//    renderer->DrawGuiImages(game);
    guiRects.push_back(gameView.addRect(YieldDisplay, gmPen, gmBrush));

    //This is a placeholder, it will need to be re-adjusted once the player class is added.
    stringData.push_back(gameView.addText(QString("Gold: %1  Production: %2  Food: %3  Science: %4  Culture: %5")
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::GOLD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::PRODUCTION))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::FOOD))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::RESEARCH))
                                          .arg(map->GetTileAt(0)->GetYield().GetYield(Yield::CULTURE))));
    stringData.at(0)->setPos(5, 2);
    stringData.at(0)->setZValue(7);
    stringData.at(0)->setDefaultTextColor(Qt::white);

    for(int i = 0; i < proxy.size(); i++)
    {
        proxy.at(i)->setZValue(7);
    }

    for(int i = 0; i < guiRects.size(); i++)
    {
        guiRects.at(i)->setZValue(6);
    }

    zoomScale = 1;

    qDebug() << "Done.";
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
        qDebug() << "Wheel Event: " << e->delta() << "ZoomScale: " << zoomScale;
        if(e->delta() > 0)
        {
            if(zoomScale < 7)
            {
                gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                gameView.scale(1.2, 1.2);
                zoomScale++;
            }
        }
        else if (e->delta() < 0)
        {
            if(zoomScale > 1)
            {
                gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                gameView.scale(1 / 1.2, 1 / 1.2);
                zoomScale--;
            }
        }
}

void GameWindow::closeGame()
{
    gameView.closeGame();
}

void GameWindow::zoomIn()
{
    gameView.zoomIn();
}

void GameWindow::zoomOut()
{
    gameView.zoomOut();
}

void GameWindow::updateGameWindow()
{
    renderer->UpdateScene(&gameView);
}




