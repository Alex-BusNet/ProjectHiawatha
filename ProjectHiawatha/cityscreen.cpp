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
#include <math.h>
#include <QJsonDocument>
#include <QDir>
#include <QCoreApplication>


CityScreen::CityScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CityScreen)
{
#ifdef __APPLE__
    QDir bin(QCoreApplication::applicationDirPath());
    qDebug()<<"cityscreen";
    bin.cdUp();
    bin.cdUp();
    bin.cdUp();
    QDir::setCurrent(bin.absolutePath());
#endif
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
    qDebug() << "   CityScreen Dec'tor called";
//    foreach(Building* b, buildings)
//    {
//        if(b != NULL)
//            delete b;
//    }

//    foreach(Unit* u, initialUnitList)
//    {
//        if(u != NULL)
//            delete u;
//    }

    delete ui;

    qDebug() << "   --CityScreen Deconstructed";
}



void CityScreen::loadBuildings(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
//       QTextStream in(&inputFile);
//       while (!in.atEnd())
//       {
//          QString line = in.readLine();
//          QStringList buildingInfo = line.split(",");
//          QString name = buildingInfo[0];
//          QString description = buildingInfo[1];
//          int cost = buildingInfo[2].toInt();
//          int bonusType = buildingInfo[3].toInt();
//          int bonusValue = buildingInfo[4].toInt();
//          int maintainanceCost = buildingInfo[5].toInt();
//          int unlocked = buildingInfo[6].toInt();
//          int techIndex = buildingInfo[7].toInt();

//          Building* building = new Building(name, description, cost, bonusType, bonusValue, maintainanceCost, unlocked, techIndex);
//          buildings.push_back(building);
//       }

       QByteArray buildingDat = inputFile.readAll();
       QJsonDocument buildDoc = QJsonDocument::fromJson(buildingDat);
       buildingList = buildDoc.array();
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
//       QTextStream in(&inputFile);
//       while (!in.atEnd())
//       {
//          QString line = in.readLine();
//          QStringList unitInfo = line.split(",");
//          int cost = unitInfo[1].toInt();
//          int strength = unitInfo[2].toInt();
//          int rangeStrength = unitInfo[3].toInt();
//          int movement = unitInfo[4].toInt();
//          int range = unitInfo[5].toInt();
//          int unlocked = unitInfo[6].toInt();
//          int enumValue = unitInfo[7].toInt();
//          int techIndex = unitInfo[8].toInt();
//          UnitType type = static_cast<UnitType>(enumValue);
//          Unit* tempUnit = new Unit(0);
//          tempUnit->SetName(unitInfo[0]);
//          tempUnit->SetCost(cost);
//          tempUnit->SetMovementPoints(movement);
//          tempUnit->SetStrength(strength);
//          tempUnit->SetRange(range);
//          tempUnit->SetRangeStrength(rangeStrength);
//          tempUnit->setUnlocked(unlocked);
//          tempUnit->SetTechIndex(techIndex);
//          tempUnit->SetUnitIcon(type);
//          initialUnitList.push_back(tempUnit);
//       }

        QByteArray unitData = inputFile.readAll();
        QJsonDocument unitDoc = QJsonDocument::fromJson(unitData);
        unitList = unitDoc.array();

       inputFile.close();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();

    }
}

void CityScreen::updateList(int currentBuildingCount, int techIndex)
{
//    for(int i = 0;i<initialUnitList.size();i++)
//    {
//        ui->listWidget_2->addItem(initialUnitList.at(i)->GetName());
//        if(currentCity->getInitialUnitList().at(i)->isUnlocked() == 0)
//        {
//               ui->listWidget_2->item(i)->setHidden(true);
//        }
//    }
//    for(int j = 0;j<buildings.size();j++)
//    {
//        ui->listWidget->addItem(buildings.at(j)->getName());
//        if(currentCity->getInitialBuildingList().at(j)->isUnlocked() == 0)
//        {
//               ui->listWidget->item(j)->setHidden(true);
//        }
//    }

    for(int i = 0; i < unitList.size(); i++)
    {
        QJsonObject obj = unitList.at(i).toObject();
        ui->listWidget_2->addItem(obj["name"].toString());
        if(obj["techindex"].toInt() > techIndex)
        {
            ui->listWidget_2->item(i)->setHidden(true);
        }
    }

    for(int j = 0; j < buildingList.size(); j++)
    {
        QJsonObject obj = buildingList.at(j).toObject();
        ui->listWidget->addItem(obj["name"].toString());
        if(obj["techindex"].toInt() > techIndex)
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

void CityScreen::getCivInfo(Civilization *civ)
{
    playerCiv = civ;
}

void CityScreen::getMapInfo(Map *m)
{
    map = m;
}

void CityScreen::getGameView(GameView *gv)
{
    view = gv;
}

void CityScreen::getRenderer(Renderer *render)
{
    renderer = render;
}

void CityScreen::updateWidget()
{
    ui->current_production_name->setText(currentCity->getProductionName());
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
//    for(int i = 0;i<buildings.size();i++)
//    {
//       for(int j = 0; j<currentCity->getCurrentBuildingList().size();j++)
//       {
//           if(buildings.at(i)->getName() == currentCity->getCurrentBuildingList().at(j)->getName())
//           {
//                ui->listWidget->item(i)->setHidden(true);
//           }
//       }
//    }

    for(int i = 0; i < buildingList.size(); i++)
    {
        for(int j = 0; j < currentCity->getCurrentBuildingList().size(); j++)
        {
            if(buildingList.at(i).toObject()["name"].toString() == currentCity->getCurrentBuildingList().at(j)->getName())
            {
                ui->listWidget->item(i)->setHidden(true);
            }
        }
    }

}

void CityScreen::getGold(int gold)
{
    this->totalGold = gold;
}


void CityScreen::on_listWidget_itemSelectionChanged()
{
    QJsonObject obj = buildingList.at(ui->listWidget->currentRow()).toObject();
    #ifdef __APPLE__
        QDir bin(QCoreApplication::applicationDirPath());
        qDebug()<<"cityscreen";
        bin.cdUp();
        bin.cdUp();
        bin.cdUp();
        QDir::setCurrent(bin.absolutePath());
    #endif
    QString tempString = "Assets/Buildings/";
    QString extension = ".png";
//    QString name = buildings.at(ui->listWidget->currentRow())->getName();
    QString name = obj["name"].toString();
    tempString += name;
    tempString += extension;
    QPixmap pic(tempString);
    QString str = "+";
    QString str2;
//    str2 = str2.number(buildings.at(ui->listWidget->currentRow())->getBonusValue());
    str2 = obj["bonus"].toString();
    QString bonusType;

//    switch(buildings.at(ui->listWidget->currentRow())->getbonusType())
    switch(obj["bonustype"].toInt())
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
//    QString cost = QString::number(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    QString cost = obj["productioncost"].toString();
//    QString turns = QString::number(ceil(static_cast<double>(buildings.at(ui->listWidget->currentRow())->getProductionCost()) / currentCity->getCityYield()->GetProductionYield()));

    QString turns = QString::number(ceil(obj["productioncost"].toDouble() / currentCity->getCityYield()->GetProductionYield()));
    temp += cost;
    temp += " (";
    temp += turns;
    temp += " turns)";
//    ui->description->setText(buildings.at(ui->listWidget->currentRow())->getDescription());
    ui->description->setText(obj["description"].toString());
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
    QJsonObject obj = buildingList.at(ui->listWidget->currentRow()).toObject();
    ui->current_production_name->setText(item->text());
    currentCity->setProductionName(item->text());
//    ui->progressBar->setMaximum(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    ui->progressBar->setMaximum(obj["productioncost"].toInt());
//    currentCity->setCurrentProductionCost(buildings.at(ui->listWidget->currentRow())->getProductionCost());
    currentCity->setCurrentProductionCost(obj["productioncost"].toInt());
    currentCity->setProductionIndex(ui->listWidget->currentRow());
    currentCity->setIsUnit(false);
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
    update();

}

void CityScreen::on_listWidget_2_itemSelectionChanged()
{
    QJsonObject obj = buildingList.at(ui->listWidget_2->currentRow()).toObject();
    #ifdef __APPLE__
        QDir bin(QCoreApplication::applicationDirPath());
        qDebug()<<"cityscreen";
        bin.cdUp();
        bin.cdUp();
        bin.cdUp();
        QDir::setCurrent(bin.absolutePath());
    #endif
    QString tempString = "Assets/Units/";
    QString extension = ".png";
//    QString name = initialUnitList.at(ui->listWidget_2->currentRow())->GetName();
    QString name = obj["name"].toString();
    tempString += name;
    tempString += extension;
    QPixmap pic(tempString);
    QString temp = "Cost: ";
//    QString cost = QString::number(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    QString cost = obj["cost"].toString();
//    QString turns = QString::number(ceil(static_cast<double>(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost()) / currentCity->getCityYield()->GetProductionYield()));
    QString turns = QString::number(ceil(obj["cost"].toDouble() / currentCity->getCityYield()->GetProductionYield()));
    temp += cost;
    temp += " (";
    temp += turns;
    temp += " turns)";
//    ui->description->setText(initialUnitList.at(ui->listWidget_2->currentRow())->GetName());
    ui->description->setText(name);
    ui->picture->setPixmap(pic);
    ui->label->setText(temp);
    ui->Bonus->setText(" ");

}

void CityScreen::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
    QJsonObject obj = unitList.at(ui->listWidget_2->currentRow()).toObject();
    ui->current_production_name->setText(item->text());
    currentCity->setProductionName(item->text());
//    ui->progressBar->setMaximum(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    ui->progressBar->setMaximum(obj["cost"].toInt());
//    currentCity->setCurrentProductionCost(initialUnitList.at(ui->listWidget_2->currentRow())->GetCost());
    currentCity->setCurrentProductionCost(obj["cost"].toInt());
    currentCity->setProductionIndex(ui->listWidget_2->currentRow());
    currentCity->setIsUnit(true);
    ui->progressBar->setValue(currentCity->getAccumulatedProduction());
    update();
}

void CityScreen::on_purchase_clicked()
{
    qDebug()<<"TOTAL GOLD: "<<totalGold;
    if(ui->tabWidget->currentIndex() == 0)
    {
//        if(totalGold > buildings.at(ui->listWidget->currentRow())->getProductionCost())
        if(totalGold > buildingList.at(ui->listWidget->currentRow()).toObject()["productioncost"].toInt())
        {
//            currentCity->addBuilding(buildings.at(ui->listWidget->currentRow()));
            Building* b = new Building();
            b->ReadBuildingSaveData(buildingList.at(ui->listWidget->currentRow()).toObject());
            currentCity->addBuilding(b);

        }else
        {
            QMessageBox* mBox = new QMessageBox();
            mBox->setText("Insufficient Funds");
            mBox->exec();
        }
    }else if(ui->tabWidget->currentIndex() == 1)
    {
//        if(totalGold > initialUnitList.at(ui->listWidget_2->currentRow())->GetCost())
        if(totalGold > unitList.at(ui->listWidget_2->currentRow()).toObject()["cost"].toInt())
        {
            Unit* unit = new Unit(0);
//            Unit* unitData = initialUnitList.at(ui->listWidget_2->currentRow());
            unit->ReadUnitSaveData(unitList.at(ui->listWidget_2->currentRow()).toObject());
//            unit->setUnitType(unitData->GetUnitType());
//            unit->SetName(unitData->GetName());
//            unit->SetCost(unitData->GetCost());
//            unit->SetMovementPoints(unitData->GetMovementPoints());
//            unit->SetStrength(unitData->GetStrength());
//            unit->SetRange(unitData->GetRange());
//            unit->SetRangeStrength(unitData->GetRangeStrength());
//            unit->SetUnitIcon(unitData->GetUnitType());
            unit->SetOwner(playerCiv->getCiv());
//            unit->SetUnitListIndex(playerCiv->GetUnitList().size());

            for(int j = 0; j < currentCity->GetControlledTiles().size();j++)
            {
                int tileIndex = currentCity->GetControlledTiles().at(j)->GetTileIndex();
                if(unit->isNaval())
                {
                    if(map->GetTileAt(tileIndex)->ContainsUnit  || !(map->GetTileTypeAt(tileIndex) == WATER)) { continue; }
                    else
                    {

                            unit->SetPositionIndex(tileIndex);
                            map->GetTileAt(tileIndex)->ContainsUnit = true;
                            break;

                    }
                }
                else
                {
                    if(map->GetTileAt(tileIndex)->ContainsUnit || !(map->GetTileAt(tileIndex)->Walkable) || (map->GetTileTypeAt(tileIndex) == WATER)) { continue; }
                    else
                    {

                            unit->SetPositionIndex(tileIndex);
                            map->GetTileAt(tileIndex)->ContainsUnit = true;
                            break;

                    }
                }
            }



        }else
        {
            QMessageBox* mBox = new QMessageBox();
            mBox->setText("Insufficient Funds");
            mBox->exec();
        }
    }

    //CHECK FOR SUFFICIENT FUNDS
    //IF ENOUGH FUNDS
    //THEN BUILD UNIT/BUILDING
    //ADD TO CITYS LIST
}
