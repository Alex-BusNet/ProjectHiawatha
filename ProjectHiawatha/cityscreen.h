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
    void    paintEvent(QPaintEvent*e);
    //Loads initial building and unit data in order to compare to the city's data
    void    loadBuildings(QString filename);
    void    loadUnits(QString filename);
    //Checks to see if any additional units or buildings have been unlocked
    void    updateList(int currentBuildingCount);
    //Gets info about the city so it knows what to display
    void    getCityInfo(City* city);
    //Checks which buildings have been completed and adds them to the completed buildings list
    void    updateWidget();
private slots:
    void on_listWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_2_itemSelectionChanged();

    void on_listWidget_2_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::CityScreen *ui;
    //Initial Building List
    QVector<Building*> buildings;
    //Initial Unit List
    QVector<Unit*> initialUnitList;
    //Pointer to city object allows cityscreen to know what to display
    City* currentCity;


};

#endif // CITYSCREEN_H
