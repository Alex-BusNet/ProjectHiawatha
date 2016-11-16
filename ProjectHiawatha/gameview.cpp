#include "gameview.h"
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QScreen>
#include <QScrollBar>
#include "qmath.h"
#include <QApplication>

GameView::GameView(QWidget *parent, bool fullscreen) : QGraphicsView(parent)
{
    game = new GameScene(this);
    zoomScale = 1;
}

QGraphicsPolygonItem *GameView::addPolygon(const QPolygonF &polygon)
{
    return this->game->addPolygon(polygon);
}

QGraphicsPolygonItem *GameView::addPolygon(const QPolygonF &polygon, const QPen &pen)
{
    return this->game->addPolygon(polygon, pen);
}

QGraphicsPixmapItem *GameView::addPixmap(const QPixmap &pixmap)
{
    return this->game->addPixmap(pixmap);
}

QGraphicsProxyWidget *GameView::addWidget(QWidget *widget)
{
    return this->game->addWidget(widget);
}

QGraphicsEllipseItem *GameView::addEllipse(const QRect &rect, const QPen &pen, const QBrush &brush)
{
    return this->game->addEllipse(rect, pen, brush);
}

QGraphicsRectItem *GameView::addRect(QRect *rect, const QPen &pen, const QBrush &brush)
{
    return this->game->addRect(*rect, pen, brush);
}

QGraphicsRectItem *GameView::addRect(int x, int y, int width, int height, const QPen &pen, const QBrush &brush)
{
    return this->game->addRect(x, y, width, height, pen, brush);
}

QGraphicsTextItem *GameView::addText(QString text)
{
    return this->game->addText(text);
}

void GameView::removeItem(QGraphicsItem *item)
{
    this->game->removeItem(item);
}

void GameView::removeRect(QGraphicsRectItem *rect)
{
    this->game->removeItem(rect);
}

GameScene *GameView::GetScene()
{
    return this->game;
}

void GameView::ConfigureGraphicsView()
{
    this->setScene(game);
    this->setMouseTracking(true);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}

//GameView paintEvent only gets called when the scene is dragged.
void GameView::paintEvent(QPaintEvent *e)
{
    QGraphicsView::paintEvent(e);
}

void GameView::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
    {
        zoomIn();
    }
    else if (e->delta() < 0)
    {
        zoomOut();
    }

    e->accept();
}

void GameView::SceneProcess(QPainter *paint)
{
//    if(game->isTileSelected)
//    {
//        game->ProcessTile(map);
//        paint->setPen(game->GetSelectedTile()->GetTilePen());
//        paint->drawPolygon(game->GetSelectedTile()->GetTilePolygon());
//    }
}

void GameView::closeGame()
{
    this->gameView.hide();
}

void GameView::zoomIn()
{
    if(zoomScale < 10)
    {
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        this->scale(1.2, 1.2);
        zoomScale++;
    }
}

void GameView::zoomOut()
{
    if(zoomScale > -1)
    {
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        this->scale(1/ 1.2, 1 / 1.2);
        zoomScale--;
    }
}
