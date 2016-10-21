//Each Civ is based here (Player and AI)

/*
 *  Holds all the overlap between player and AI classes
 *      City and units owned / locations
 *      Borders
 *      Data such as Tech progress, Yields, etc will be read through here (stored elsewhere)
 *
 */

#include "civilization.h"
#include <QDebug>
#include <fstream>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QStringList>

Civilization::Civilization()
{

}

Civilization::Civilization(Nation name, bool isAI)
{
    this->name=name;
    //set color
    //spawn a city, and initialize (based on nation)
    //call updatecityyield and updatecivyield to initialize
    this->totalCivYield = new Yield(0, 0, 0, 0, 0);
    this->happiness=0;

    if(isAI)
    {
        this->isAIPlayer = true;
    }
}





//Accessors and Mutators
Nation Civilization::getCiv()
{
    return this->name;
}

Civilization *Civilization::GetCivObject()
{
    return this;
}

void Civilization::UpdateCivYield()
{
    qDebug() << "   Civ controls" << currentCityList.size() << "cities";

    int newGold = 0, newProd = 0, newSci = 0, newFood = 0, newCul = 0;

    foreach(City *city, currentCityList)
    {
        newGold += city->getCityYield()->GetGoldYield();
        newProd += city->getCityYield()->GetProductionYield();
        newSci += city->getCityYield()->GetScienceYield();
        newFood += city->getCityYield()->GetFoodYield();
        newCul += city->getCityYield()->GetCultureYield();
    }

    this->totalCivYield->ChangeYield(newGold, newProd, newSci, newFood, newCul);
}

Yield *Civilization::getCivYield()
{
    return this->totalCivYield;
}

int Civilization::getHappiness()
{
    return this->happiness;
}

QVector<City *> Civilization::GetCityList()
{
    return this->currentCityList;
}

QVector<Unit *> Civilization::GetUnitList()
{
    return this->UnitList;
}

QVector<City *> Civilization::GetInitialCityList()
{
    return this->initialCityList;
}

bool Civilization::isCivAI()
{
    return this->isAIPlayer;
}

void Civilization::loadTechs(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList techInfo = line.split(",");
          qDebug()<<"Tech Name: "<<techInfo[0];
          int x = techInfo[1].toInt();
          int y = techInfo[2].toInt();
          qDebug()<<"Tech Cost: "<<x;

          Technology* tech = new Technology(techInfo[0],x,y);
          techList.push_back(tech);
          qDebug()<<techList.at(0)->getName();
       }
       inputFile.close();
       qDebug()<<techList.at(1)->getName();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}

Technology *Civilization::getCurrentTech()
{
    return currentTech;
}

Technology *Civilization::getNextTech()
{
    return nextTech;
}

void Civilization::setCurrentTech(Technology *tech)
{
    currentTech = tech;
}

void Civilization::setNextTech(Technology *tech)
{
    nextTech = tech;
}

void Civilization::loadCities(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList cityInfo = line.split(",");
          qDebug()<<"City Name: "<<cityInfo[0];

          City* city = new City();
          city->SetName(cityInfo[0]);
          initialCityList.push_back(city);
          qDebug()<<initialCityList.at(0)->GetName();

       }
       inputFile.close();
       qDebug()<<initialCityList.at(1)->GetName();
       qDebug()<<initialCityList.at(10)->GetName();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}

void Civilization::AddCity(City *city)
{
    this->currentCityList.push_back(city);
}

void Civilization::AddUnit(Unit *unit)
{
    this->UnitList.push_back(unit);
}

void Civilization::SetUnitList(QVector<Unit *> list)
{
    int i = 0;
    foreach (Unit* unit, list)
    {
        this->UnitList.replace(i, unit);
        i++;
    }
}

void Civilization::SetCityList(QVector<City *> list)
{
    int i = 0;
    foreach (City* city, list)
    {
        this->currentCityList.replace(i, city);
        i++;
    }
}

void Civilization::SetCivObj(Civilization *civ)
{
    SetCityList(civ->GetCityList());
    SetUnitList(civ->GetUnitList());
}

void Civilization::SetHappiness(int happiness)
{
    this->happiness=happiness;
}

City* Civilization::GetCityAt(int index)
{
    if(index < this->currentCityList.size())
    {
        return this->currentCityList.at(index);
    }
    else
    {
        //If the index is too large, return the capital
        return this->currentCityList.at(0);
    }
}

Unit* Civilization::GetUnitAt(int index)
{

    if(index < this->UnitList.size())
    {
        return this->UnitList.at(index);


    }
    else
    {
        qDebug() << "UnitList -- Index out of range";
        return NULL;
    }
}



