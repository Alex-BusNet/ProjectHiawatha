#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>
#include <building.h>
#include "unit.h"
#include "city.h"
#include <QVector>
#include <QString>
#include <QListWidgetItem>
#include <QJsonArray>
#include <QJsonObject>
#include "map.h"
#include "gameview.h"
#include "renderer.h"

namespace Ui {
class CityScreen;
}

class CityScreen : public QWidget
{
    Q_OBJECT

public:
    explicit CityScreen(QWidget *parent = 0);
    ~CityScreen();
    //Loads initial building and unit data in order to compare to the city's data
    void    loadBuildings(QString filename);
    void    loadUnits(QString filename);
    //Checks to see if any additional units or buildings have been unlocked
    void    updateList(int currentBuildingCount, int techIndex);
    //Gets info about the city so it knows what to display
    void    getCityInfo(City* city);
    void    getCivInfo(Civilization* civ);
    void    getMapInfo(Map* m);
    void    getGameView(GameView* gv);
    void    getRenderer(Renderer* render);
    //Checks which buildings have been completed and adds them to the completed buildings list
    void    updateWidget();
    //
    void    getGold(int gold);
private slots:
    void on_listWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_2_itemSelectionChanged();

    void on_listWidget_2_itemDoubleClicked(QListWidgetItem *item);

    void on_purchase_clicked();

private:
    Ui::CityScreen *ui;
    //Initial Building List
//    QVector<Building*> buildings;
    QJsonArray buildingList;
    //Initial Unit List
//    QVector<Unit*> initialUnitList;
    QJsonArray unitList;
    //Pointer to city object allows cityscreen to know what to display
    City* currentCity;
    //
    Civilization* playerCiv;
    Map* map;
    GameView* view;
    Renderer* renderer;
    int totalGold;
    int w, h;

};

#endif // CITYSCREEN_H
