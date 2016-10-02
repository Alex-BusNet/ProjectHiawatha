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
    game = new QGraphicsScene(this);
    this->setScene(game);
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
    this->setMouseTracking(true);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
}


void GameView::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if(QLineF(e->screenPos(), e->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance())
    {
        return;
    }

    QDrag *drag = new QDrag(e->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

void GameView::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    this->setCursor(Qt::OpenHandCursor);

    qDebug() << "Press point: " << clickedPos << " Release point: " << e->pos();
}

void GameView::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug() << "Mouse Pressed";
    this->setCursor(Qt::ClosedHandCursor);
    clickedPos = e->pos();
}

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

void GameView::SetGameMap(Map *map)
{
    this->map = map;
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
    if(zoomScale > 1)
    {
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        this->scale(1/ 1.2, 1 / 1.2);
        zoomScale--;
    }
}
