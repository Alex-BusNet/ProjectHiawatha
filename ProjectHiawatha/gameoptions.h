#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <QWidget>
#include "gamemanager.h"

namespace Ui {
class GameOptions;
}

class GameOptions : public QWidget
{
    Q_OBJECT

public:
    explicit GameOptions(QWidget *parent = 0,bool fullscreen = false);
    ~GameOptions();
    void paintEvent(QPaintEvent*e);

private slots:
    void on_pushButton_clicked();

private:
    Ui::GameOptions *ui;
    GameManager *game;
    bool FullScreen;
};

#endif // GAMEOPTIONS_H
