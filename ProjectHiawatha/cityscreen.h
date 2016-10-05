#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>
#include <QPainter>
#include <building.h>
#include <QVector>
#include <QString>

namespace Ui {
class CityScreen;
}

class CityScreen : public QWidget
{
    Q_OBJECT

public:
    explicit CityScreen(QWidget *parent = 0);
    ~CityScreen();
    void paintEvent(QPaintEvent*e);
    void loadBuildings(QString filename);
private:
    Ui::CityScreen *ui;
    QVector<Building> buildings;

};

#endif // CITYSCREEN_H
