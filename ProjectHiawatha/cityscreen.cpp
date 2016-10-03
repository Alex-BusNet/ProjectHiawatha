#include "cityscreen.h"
#include "ui_cityscreen.h"

CityScreen::CityScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CityScreen)
{
    ui->setupUi(this);
    QPixmap pic("../ProjectHiawatha/Assets/Buildings/wall.jpg");
    ui->label->setPixmap(pic);
    ui->label_2->setText("Walls");
    ui->label_3->setText("+5000 Defense");
}

CityScreen::~CityScreen()
{
    delete ui;
}

void CityScreen::paintEvent(QPaintEvent *e)
{

}
