#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#include <QWidget>

namespace Ui {
class CityScreen;
}

class CityScreen : public QWidget
{
    Q_OBJECT

public:
    explicit CityScreen(QWidget *parent = 0);
    ~CityScreen();

private:
    Ui::CityScreen *ui;
};

#endif // CITYSCREEN_H
