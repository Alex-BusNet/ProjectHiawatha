#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game = NULL;
    options = NULL;
    fullscreen = false;
    QPixmap *bkgnd = new QPixmap("../ProjectHiawatha/Assets/Menu/Background/dom_hiawatha.png");
    QPixmap *buttonBorder = new QPixmap("../ProjectHiawatha/Assets/Menu/mainMenuBackground.png");
    ui->mainBackground->setPixmap(*bkgnd);
    ui->menuButtonBorder->setPixmap(*buttonBorder);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
    delete options;
}

void MainWindow::on_newMap_clicked()
{
    if(game != NULL)
    {
        delete game;
    }
    if(options != NULL)
    {
        delete options;
    }

    options = new GameOptions(0,fullscreen);
    options->show();
}


void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_fullscreen_clicked()
{
    this->fullscreen = !(this->fullscreen);
}
