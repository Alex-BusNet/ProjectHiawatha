#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gamemanager.h"
#include "renderer.h"
#include "gameoptions.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_newMap_clicked();

    void on_exit_clicked();

    void on_fullscreen_clicked();

private:
    Ui::MainWindow *ui;
    GameManager *game;
    GameOptions *options;
    bool fullscreen;
};

#endif // MAINWINDOW_H
