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
    QPixmap pic("Assets/Buildings/walls.png");
    ui->picture->setPixmap(pic);
    ui->picture->setScaledContents(true);
    ui->city_name->setText(" ");
    ui->Bonus->setText("No Building Selected");
    ui->tabWidget->setTabText(0, "Buildings");
    ui->tabWidget->setTabText(1, "Units");
    ui->tabWidget->setTabText(2, "Completed Buildings");
    ui->progressBar->setMinimum(0);
    ui->label->setText("Cost: ");
    ui->ItemBG->setPixmap(QPixmap("Assets/UI/CityScreenBackground_alt2.png"));

}

CityScreen::~CityScreen()
{
    delete ui;
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
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
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
          tempUnit->SetUnitIcon(type);
          initialUnitList.push_back(tempUnit);

       }
       inputFile.close();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();

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
    ui->GoldYield->setText(QString("%1 per turn").arg(city->getCityYield()->GetGoldYield()));
    ui->ProdYield->setText(QString("%1 per turn").arg(city->getCityYield()->GetProductionYield()));
    ui->SciYield->setText(QString("%1 per turn").arg(city->getCityYield()->GetScienceYield()));
    ui->FoodYield->setText(QString("%1 per turn").arg(city->getCityYield()->GetFoodYield()));
    ui->CultureYield->setText(QString("%1 per turn").arg(city->getCityYield()->GetCultureYield()));
    ui->citizenCountDown->setText(QString("%1").arg(city->GetTurnsToNewCitizen()));
    ui->citizenCount->setText(QString("%1").arg(city->GetCitizenCount()));
    ui->cityStrength->setText(QString("%1").arg(city->GetCityStrength()));
    ui->borderGrowth->setText(QString("%1").arg(city->GetTurnsToBorderGrowth()));
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
    QString tempString = "Assets/Buildings/";
    QString extension = ".png";
    QString name = buildings.at(ui->listWidget->currentRow())->getName();
    tempString += name;
    tempString += extension;
    QPixmap pic(tempString);
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
    QString temp = "Cost: ";
    QString cost = QString::number(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    QString turns = QString::number(ceil(static_cast<double>(buildings.at(ui->listWidget->currentRow())->getProductionCost()) / currentCity->getCityYield()->GetProductionYield()));
    temp += cost;
    temp += " (";
    temp += turns;
    temp += " turns)";
    ui->description->setText(buildings.at(ui->listWidget->currentRow())->getDescription());
    ui->picture->setPixmap(pic);
    ui->label->setText(temp);
    update();
}

void CityScreen::on_pushButton_clicked()
{
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
    QString tempString = "Assets/Units/";
    QString extension = ".png";
    QString name = initialUnitList.at(ui->listWidget_2->currentRow())->GetName();
    tempString += name;
    tempString += extension;
    QPixmap pic(tempString);
    QString temp = "Cost: ";
    QString cost = QString::number(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    QString turns = QString::number(ceil(static_cast<double>(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost()) / currentCity->getCityYield()->GetProductionYield()));
    temp += cost;
    temp += " (";
    temp += turns;
    temp += " turns)";
    ui->description->setText(initialUnitList.at(ui->listWidget_2->currentRow())->GetName());
    ui->picture->setPixmap(pic);
    ui->label->setText(temp);
    ui->Bonus->setText(" ");

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
