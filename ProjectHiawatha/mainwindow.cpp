#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game = NULL;
    fullscreen = false;

    QPixmap bkgnd("../ProjectHiawatha/Assets/Menu/Background/stolenInternet.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    this->setFixedSize(this->width(), this->height());
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
