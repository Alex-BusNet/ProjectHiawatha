#include "gameview.h"
#include <QDebug>

GameView::GameView(QWidget *widget, bool fullscreen)
{
    game = new QGraphicsScene(this);
    gameView.setScene(game);

    qDebug() << "Done. Setting gameView settings.";
    gameView.installEventFilter(this);
    gameView.setMouseTracking(true);
    gameView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gameView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView.setDragMode(QGraphicsView::ScrollHandDrag);

    qDebug() <<"Done.\nSetting screen size.";
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

    gameView.activateWindow();

    zoomScale = 1;
}

QGraphicsPolygonItem *GameView::addPolygon(const QPolygonF &polygon)
{
    return this->game->addPolygon(polygon);
}

QGraphicsPixmapItem *GameView::addPixmap(const QPixmap &pixmap)
{
    return this->game->addPixmap(pixmap);
}

QGraphicsProxyWidget *GameView::addWidget(QWidget *widget)
{
    return this->game->addWidget(widget);
}

bool GameView::eventFilter(QObject *watched, QEvent *event)
{
//    qDebug() << "GameView event filter";

    if((watched->metaObject()->className() == this->metaObject()->className()) && event->type() == QEvent::Wheel)
    {
        this->wheelEvent(static_cast<QWheelEvent*>(event));
        return true;
    }
    else if((watched->metaObject()->className() == this->metaObject()->className()) && event->type() == QEvent::MouseMove)
    {
        qDebug() << "Mouse Move Event Filter";
        return true;
    }

    return false;
}

void GameView::wheelEvent(QWheelEvent *e)
{
    qDebug() << "GameView Wheel event. ZoomScale: " << zoomScale;
    if(e->delta() > 0)
    {
        zoomIn();
    }
    else if (e->delta() < 0)
    {
        zoomOut();
    }
}

void GameView::mouseMoveEvent(QMouseEvent *event)
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

void GameView::scrollContentsBy(int dx, int dy)
{
    //NOOP
}

void GameView::closeGame()
{
    this->gameView.hide();
}

void GameView::zoomIn()
{
    if(zoomScale < 7)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1.2, 1.2);
        zoomScale++;
    }
}

void GameView::zoomOut()
{
    if(zoomScale > 1)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1/ 1.2, 1 / 1.2);
        zoomScale--;
    }
}
