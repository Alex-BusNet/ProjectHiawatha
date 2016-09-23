#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game = NULL;
    fullscreen = false;
    QPixmap *bkgnd = new QPixmap("../ProjectHiawatha/Assets/Menu/Background/stolenInternet.jpg");
    QPixmap *buttonBorder = new QPixmap("../ProjectHiawatha/Assets/Menu/mainMenuBackground.png");
    ui->mainBackground->setPixmap(*bkgnd);
    ui->menuButtonBorder->setPixmap(*buttonBorder);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
}

void MainWindow::on_newMap_clicked()
{
    if(game != NULL)
    {
        delete game;
    }

    game = new GameWindow(0, fullscreen);
//    game->show();
}


void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_fullscreen_clicked()
{
    this->fullscreen = !(this->fullscreen);
}
