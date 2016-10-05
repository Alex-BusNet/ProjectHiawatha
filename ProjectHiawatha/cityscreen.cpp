#include "cityscreen.h"
#include "ui_cityscreen.h"
#include <QString>
#include <fstream>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

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

void CityScreen::loadBuildings(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList buildingInfo = line.split(",");
          qDebug()<<"BUilding Name: "<<buildingInfo[0];
          int x = buildingInfo[2].toInt();
          int y = buildingInfo[3].toInt();
          int z = buildingInfo[5].toInt();
          int temp = buildingInfo[6].toInt();
          qDebug()<<"Production Cost: "<<x;
          //Building building(buildingInfo[0],buildingInfo[1],x,y,buildingInfo[4],z,temp,buildingInfo[7]);
          //buildings.push_back(building);
       }
       inputFile.close();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";
        this->showMinimized();
    }
}
