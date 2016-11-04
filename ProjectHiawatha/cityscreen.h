#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>
#include <QPainter>
#include <building.h>
#include "unit.h"
#include "city.h"
#include <QVector>
#include <QString>
#include <QListWidgetItem>

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
    void loadUnits(QString filename);
    void updateList();
    void getCityInfo(City* currentCity);
private slots:
    void on_listWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);



    void on_listWidget_2_itemSelectionChanged();

private:
    Ui::CityScreen *ui;
    QVector<Building*> buildings;
    QVector<Unit*> initialUnitList;
    QString str;
    int productionYield;


};

#endif // CITYSCREEN_H
