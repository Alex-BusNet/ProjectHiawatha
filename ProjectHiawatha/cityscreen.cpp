#include "cityscreen.h"
#include "ui_cityscreen.h"

CityScreen::CityScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CityScreen)
{
    ui->setupUi(this);
}

CityScreen::~CityScreen()
{
    delete ui;
}
