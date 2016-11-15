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

    ui->setupUi(this);
    QPixmap pic("../ProjectHiawatha/Assets/Buildings/walls.png");
    ui->picture->setPixmap(pic);
    ui->city_name->setText("Walls");
    ui->Bonus->setText("+5000 Defense");
    ui->tabWidget->setTabText(0, "Buildings");
    ui->tabWidget->setTabText(1, "Units");
    ui->tabWidget->setTabText(2, "Completed Buildings");
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
          qDebug()<<"Building Name: "<<buildingInfo[0];
          QString name = buildingInfo[0];
          QString description = buildingInfo[1];
          int cost = buildingInfo[2].toInt();
          int bonusType = buildingInfo[3].toInt();
          int bonusValue = buildingInfo[4].toInt();
          int maintainanceCost = buildingInfo[5].toInt();
          int unlocked = buildingInfo[6].toInt();
          int techIndex = buildingInfo[7].toInt();

          Building* building = new Building(name, description, cost, bonusType, bonusValue, maintainanceCost, unlocked, techIndex);
          buildings.push_back(building);
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
          int enumValue = unitInfo[7].toInt();
          int techIndex = unitInfo[8].toInt();
          UnitType type = static_cast<UnitType>(enumValue);
          Unit* tempUnit = new Unit(0);
          tempUnit->SetName(unitInfo[0]);
          tempUnit->SetCost(cost);
          tempUnit->SetMovementPoints(movement);
          tempUnit->SetStrength(strength);
          tempUnit->SetRange(range);
          tempUnit->SetRangeStrength(rangeStrength);
          tempUnit->setUnlocked(unlocked);
          tempUnit->SetTechIndex(techIndex);
          qDebug()<<"TYPE: "<<type;
          tempUnit->SetUnitIcon(type);
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

void CityScreen::updateList(int currentBuildingCount)
{
    for(int i = 0;i<initialUnitList.size();i++)
    {
        ui->listWidget_2->addItem(initialUnitList.at(i)->GetName());
        if(currentCity->getInitialUnitList().at(i)->isUnlocked() == 0)
        {
               ui->listWidget_2->item(i)->setHidden(true);
        }

    }
    for(int j = 0;j<buildings.size();j++)
    {
        ui->listWidget->addItem(buildings.at(j)->getName());
        if(currentCity->getInitialBuildingList().at(j)->isUnlocked() == 0)
        {
               ui->listWidget->item(j)->setHidden(true);
        }

    }

    if(currentBuildingCount > 0)
    {
        for(int k = 0;k<currentCity->getCurrentBuildingList().size();k++)
        {
            ui->listWidget_3->addItem(currentCity->getCurrentBuildingList().at(k)->getName());
        }
    }

    update();
}

void CityScreen::getCityInfo(City *city)
{
    currentCity = city;
    ui->city_name->setText(currentCity->GetName());
    productionYield = currentCity->getCityYield()->GetProductionYield();
}

void CityScreen::updateWidget()
{
    ui->current_production_name->setText(currentCity->getProductionName());
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
    for(int i = 0;i<buildings.size();i++)
    {
       for(int j = 0; j<currentCity->getCurrentBuildingList().size();j++)
       {
           if(buildings.at(i)->getName() == currentCity->getCurrentBuildingList().at(j)->getName())
           {
                ui->listWidget->item(i)->setHidden(true);
           }
       }
    }

}


void CityScreen::on_listWidget_itemSelectionChanged()
{
    QString str = "+";
    QString str2;
    str2 = str2.number(buildings.at(ui->listWidget->currentRow())->getBonusValue());
    QString bonusType;
    switch(buildings.at(ui->listWidget->currentRow())->getbonusType())
    {
    case 0:
        bonusType = " Gold";
        break;
    case 1:
        bonusType = " Production";
        break;
    case 2:
        bonusType = " Science";
        break;
    case 3:
        bonusType = " Food";
        break;
    case 4:
        bonusType = " Culture";
        break;
    case 5:
        bonusType = " Defense";
        break;
    default:
        bonusType = " Gold";

    }

    str2.append(bonusType);
    str.append(str2);
    ui->Bonus->setText(str);
    ui->description->setText(buildings.at(ui->listWidget->currentRow())->getDescription());
    update();
}

void CityScreen::on_pushButton_clicked()
{
    str = (ui->current_production_name->text());
    qDebug()<<"STR: "<<str;
    this->hide();
}

void CityScreen::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->current_production_name->setText(item->text());
    currentCity->setProductionName(item->text());
    ui->progressBar->setMaximum(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    currentCity->setCurrentProductionCost(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    currentCity->setProductionIndex(ui->listWidget->currentRow());
    currentCity->setIsUnit(false);
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
    update();

}

void CityScreen::on_listWidget_2_itemSelectionChanged()
{
    ui->description->setText(initialUnitList.at(ui->listWidget_2->currentRow())->GetName());

}

void CityScreen::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
    ui->current_production_name->setText(item->text());
    currentCity->setProductionName(item->text());
    ui->progressBar->setMaximum(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    currentCity->setCurrentProductionCost(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    currentCity->setProductionIndex(ui->listWidget_2->currentRow());
    currentCity->setIsUnit(true);
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
    update();
}
