#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <QWidget>
#include "gamemanager.h"
#include <QListWidgetItem>
#include "nation.h"


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



    void on_listWidget_itemSelectionChanged();

private:
    Ui::GameOptions *ui;
    GameManager *game;
    bool FullScreen;
    Nation player;
};

#endif // GAMEOPTIONS_H
