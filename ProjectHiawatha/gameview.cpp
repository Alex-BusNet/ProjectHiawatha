#include "gameview.h"
#include <QDebug>
#include <QScreen>
#include <QScrollBar>
#include "qmath.h"

GameView::GameView(QWidget *parent, bool fullscreen) : QGraphicsView(parent)
{
    game = new QGraphicsScene(this);

    qDebug() << "Done. Setting gameView settings.";


    qDebug() <<"Done.\nSetting screen size.";


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

QGraphicsRectItem *GameView::addRect(QRect *rect, QPen pen, QBrush brush)
{
    return this->game->addRect(*rect, pen, brush);
}

QGraphicsTextItem *GameView::addText(QString text)
{
    return this->game->addText(text);
}

QGraphicsScene *GameView::GetScene()
{
    return this->game;
}

void GameView::ConfigureGraphicsView()
{
    this->installEventFilter(this);
    this->setMouseTracking(true);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setAlignment(Qt::AlignLeft);
    this->setAlignment(Qt::AlignTop);
}

bool GameView::eventFilter(QObject *watched, QEvent *event)
{
//    qDebug() << "GameView event filter";

    if((watched->metaObject()->className() == this->metaObject()->className()) && event->type() == QEvent::Wheel)
    {
        qDebug() << "Calling wheelEvent";
        wheelEvent(static_cast<QWheelEvent*>(event));
        return true;
    }
    else if((watched->metaObject()->className() == this->metaObject()->className()) && event->type() == QEvent::MouseMove)
    {
        qDebug() << "Mouse Move Event Filter";
        return true;
    }
    else if((watched->metaObject()->className() == this->metaObject()->className() && event->type() == QEvent::MouseButtonRelease))
    {
        qDebug() << "GameView Mouse Release Event";
        return true;
    }

    return false;
}

void GameView::wheelEvent(QWheelEvent *e)
{
    qDebug() << "GameView Wheel event. ZoomScale: " << zoomScale;

    if(e->delta() > 0)
    {
        if(zoomScale < 10)
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
            gameView.scale(1/ 1.2, 1 / 1.2);
            zoomScale--;
        }
    }

    e->accept();
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

void GameView::mouseReleaseEvent(QMouseEvent *event)
{

}


void GameView::closeGame()
{
    this->gameView.hide();
}

void GameView::zoomIn()
{
    if(zoomScale < 10)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1.2, 1.2);
        zoomScale++;
    }
}

void GameView::zoomOut()
{
    if(zoomScale > 1)
    {
        gameView.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        gameView.scale(1/ 1.2, 1 / 1.2);
        zoomScale--;
    }
}
