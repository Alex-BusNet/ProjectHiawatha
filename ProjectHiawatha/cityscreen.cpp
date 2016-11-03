#include "cityscreen.h"
#include "ui_cityscreen.h"
#include "unit.h"
#include <QString>
#include <fstream>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QListWidgetItem>


CityScreen::CityScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CityScreen)
{
    if(str.isEmpty())
    {
        str =  "No Current Production";
    }
    ui->setupUi(this);
    QPixmap pic("../ProjectHiawatha/Assets/Buildings/walls.png");
    ui->label->setPixmap(pic);
    ui->label_2->setText("Walls");
    ui->label_3->setText("+5000 Defense");
    ui->label_6->setText(str);
    ui->progressBar->setMinimum(0);


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
          bool flag;
          QString str = buildingInfo[7];
          if(str.contains("true", Qt::CaseInsensitive))
          {
              flag = true;
          }else
          {
              flag = false;
          }
          Building* building = new Building(buildingInfo[0],buildingInfo[1],x,y,buildingInfo[4],z,temp,flag);
          buildings.push_back(building);
          qDebug()<<buildings.at(0)->isUnlocked();
       }
       inputFile.close();
       qDebug()<<buildings.at(1)->getName();
       qDebug()<<buildings.size();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";
        this->showMinimized();
    }
}

void CityScreen::loadUnits(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList unitInfo = line.split(",");
          qDebug()<<"Unit Name: "<<unitInfo[0];
          int cost = unitInfo[1].toInt();
          int strength = unitInfo[2].toInt();
          int rangeStrength = unitInfo[3].toInt();
          int movement = unitInfo[4].toInt();
          int range = unitInfo[5].toInt();
          int unlocked = unitInfo[6].toInt();
          Unit* tempUnit = new Unit(0);
          tempUnit->SetName(unitInfo[0]);
          tempUnit->SetCost(cost);
          tempUnit->SetMovementPoints(movement);
          tempUnit->SetStrength(strength);
          tempUnit->SetRange(range);
          tempUnit->SetRangeStrength(rangeStrength);
          tempUnit->setUnlocked(unlocked);
          initialUnitList.push_back(tempUnit);
          qDebug()<<initialUnitList.at(0)->GetName();

       }
       inputFile.close();
       qDebug()<<initialUnitList.at(1)->GetName();
       qDebug()<<initialUnitList.at(10)->GetName();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}

void CityScreen::updateList()
{
    int index = 0;
    for(int i = 0;i<initialUnitList.size();i++)
    {
        ui->listWidget->addItem(initialUnitList.at(i)->GetName());
        index++;
        if(initialUnitList.at(i)->isUnlocked()==0)
        {
               ui->listWidget->item(i)->setHidden(true);
        }

    }
    for(int j = 0;j<buildings.size();j++)
    {
        ui->listWidget->addItem(buildings.at(j)->getName());
        if(buildings.at(j)->isUnlocked()==false)
        {
               ui->listWidget->item(index)->setHidden(true);
        }

    }
    update();
}

void CityScreen::on_listWidget_itemSelectionChanged()
{
    QString str = "+";
    QString str2;
    str2 = str2.number(buildings.at(ui->listWidget->currentRow())->getBonusValue());
    qDebug()<<"Bonus Value: "<<buildings.at(ui->listWidget->currentRow())->getBonusValue();
    QString str3 = buildings.at(ui->listWidget->currentRow())->getbonusType();
    str2.append(str3);
    qDebug()<<"str2: "<<str2;
    str.append(str2);
    qDebug()<<"str: "<<str;
    ui->label_2->setText(str);
    ui->label_3->setText(buildings.at(ui->listWidget->currentRow())->getDescription());
    update();
}

void CityScreen::on_pushButton_clicked()
{
    str = (ui->label_6->text());
    this->close();
}

void CityScreen::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->label_6->setText(item->text());
    ui->progressBar->setMaximum(buildings.at(ui->listWidget->currentRow())->getBuildingMaintenanceCost());
    ui->progressBar->setValue(10);
    update();

}
