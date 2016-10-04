#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>
#include <QPainter>

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

private:
    Ui::CityScreen *ui;
};

#endif // CITYSCREEN_H
