#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gamewindow.h"
#include "renderer.h"
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

private:
    Ui::MainWindow *ui;
    GameWindow *game;
};

#endif // MAINWINDOW_H
