#include "mainwindow.h"
#include "ui_mainwindow.h"

QString MainStyle = "QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; max-width: 201px; min-width: 75; min-height: 23px; max-height: 61px; } QPushButton:pressed { background-color: #77adcb; }";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game = NULL;
    options = NULL;
    about = NULL;
    fullscreen = false;
    QPixmap *bkgnd = new QPixmap("/Assets/Menu/Background/dom_hiawatha.png");
    QPixmap *buttonBorder = new QPixmap("Assets/Menu/mainMenuBackground.png");
    ui->mainBackground->setPixmap(*bkgnd);
    ui->menuButtonBorder->setPixmap(*buttonBorder);
    this->setStyleSheet(MainStyle);
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

void MainWindow::on_controls_clicked()
{
    if(about != NULL)
    {
        delete about;
    }

    about = new About();
    about->show();
}
