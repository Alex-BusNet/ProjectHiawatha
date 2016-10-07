#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>
#include <QPainter>
#include <building.h>
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
    void updateList();
private slots:
    void on_listWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::CityScreen *ui;
    QVector<Building*> buildings;
    QString str;

};

#endif // CITYSCREEN_H
