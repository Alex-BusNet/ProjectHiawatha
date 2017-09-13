#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "gamescene.h"
#include "map.h"

#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QPushButton>
#include <QWheelEvent>

class GameView : public QGraphicsView
{

public:
    GameView(QWidget *parent = 0);
    ~GameView();

    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon);
    QGraphicsPolygonItem* addPolygon(const QPolygonF &polygon, const QPen &pen);
    QGraphicsPixmapItem* addPixmap(const QPixmap &pixmap);
    QGraphicsProxyWidget* addWidget(QWidget *widget);
    QGraphicsEllipseItem* addEllipse(const QRect &rect, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    QGraphicsRectItem* addRect(QRect *rect, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    QGraphicsRectItem* addRect(int x, int y, int width, int height, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    QGraphicsTextItem* addText(QString text);
    QGraphicsLineItem* addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());

    void removeItem(QGraphicsItem* item);
    void removeRect(QGraphicsRectItem* rect);
    void addItem(QGraphicsItem *item);
    GameScene *GetScene();
    void ConfigureGraphicsView();
    void SetCityScreenZoom();

    virtual void wheelEvent(QWheelEvent *e);

private:
    GameScene *game;

    QPointF clickedPos;

    int zoomScale;

protected:

public slots:
    void closeGame();
    void zoomIn();
    void zoomOut();
};

#endif // GAMEVIEW_H
