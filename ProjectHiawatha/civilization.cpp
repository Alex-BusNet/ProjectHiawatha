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
    this->cityIndex = 0;
    this->totalGold = 0;
    this->totalScience = 0;
    this->totalCulture = 0;
    this->accumulatedScience = 0;
    this->techIndex = 0;
    this->cityFounded = false;

    if(isAI)
    {
        this->isAIPlayer = true;
        this->provoked=false;
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

int Civilization::GetTotalGold()
{
    return this->totalGold;
}

int Civilization::GetTotalScience()
{
    return this->totalScience;
}

int Civilization::GetTotalCulture()
{
    return this->totalCulture;
}

QVector<City *> Civilization::GetCityList()
{
    return this->currentCityList;
}

QVector<Unit *> Civilization::GetUnitList()
{
    return this->UnitList;
}

Update_t Civilization::UpdateProgress()
{
    static int turn = 0;
    turn++;

    this->totalGold += this->getCivYield()->GetGoldYield();
    this->totalScience += this->getCivYield()->GetScienceYield();
    this->totalCulture += this->getCivYield()->GetCultureYield();

    Update_t redraw{false, false, false};
    foreach(City* city, this->currentCityList)
    {
        Update_t cityProgress = city->UpdateProgress();

        if(cityProgress.updateBorders && !redraw.updateBorders)
        {
            redraw.updateBorders = true;
        }
        else if(!redraw.updateBorders)
        {
            redraw.updateBorders = false;
        }

        if(cityProgress.updateCitizens)
        {
            qDebug() << "--------Updating citizens";
            this->UpdateCivYield();
            redraw.updateCitizens = true;
        }

        if(cityProgress.productionFinished)
        {
            qDebug() << "---------Production Finished";
            redraw.productionFinished = true;
        }

        if(cityProgress.cityHealed)
        {
            qDebug() << "---------City Healed";
            redraw.cityHealed = true;
        }
    }

    /*cost = (n * b*(1 + g*m)/100)^(1 + g / d)
     * n = number of units
     * b = base cost of units
     * m = mulitplier (1)
     * d = divisor (1)
     * g = game progress factor
     *      g = currentTurn / endTurn
    */
    double gpf =  turn / 500.0;

    int maintenance = pow((this->UnitList.size() * 200 * (1 + gpf)) / 100, (1 + gpf));
    qDebug() << "   --Maintenance cost:" << pow((this->UnitList.size() * 200 * (1 + gpf)) / 100, (1 + gpf));

    this->totalGold -= maintenance;

    return redraw;
}

QVector<QString> Civilization::GetInitialCityList()
{
    return this->initialCityList;
}


QVector<Technology *> Civilization::GetTechList()
{
    return this->techList;
}

QString Civilization::GetNextCityName()
{
    //We need to increment the cityIndex before getting the value since
    // the funtion won't run after the return line. Therefore, the value
    // in passed to at( ) needs to be cityIndex - 1 to get the string we
    // actually need.

    cityIndex++;
    return this->initialCityList.at(cityIndex - 1);

}

int Civilization::getCityIndex()
{
    return this->cityIndex;
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
//          qDebug()<<"Tech Name: "<<techInfo[0];
          int x = techInfo[1].toInt();
          int y = techInfo[2].toInt();
//          qDebug()<<"Tech Cost: "<<x;

          Technology* tech = new Technology(techInfo[0],x,y);
          techList.push_back(tech);
//          qDebug()<<techList.at(0)->getName();
       }
       inputFile.close();
//       qDebug()<<techList.at(1)->getName();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}

void Civilization::setAccumulatedScience(int science)
{
    this->accumulatedScience += science;
}

int Civilization::getAccumulatedScience(void)
{
    return this->accumulatedScience;
}

void Civilization::resetAccumulatedScience()
{
    this->accumulatedScience = 0;
}

int Civilization::getTechIndex()
{
    return this->techIndex;
}

void Civilization::setTechIndex()
{
    this->techIndex++;
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
//          qDebug()<<"City Name: "<<cityInfo[0];

          initialCityList.push_back(cityInfo[0]);
//          qDebug()<<initialCityList.at(0);

       }
       inputFile.close();
//       qDebug()<<initialCityList.at(1);
//       qDebug()<<initialCityList.at(10);
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}


void Civilization::setLowThreats(QVector<Unit *> lowThreats)
{
    this->lowThreats=lowThreats;
}

void Civilization::setMidThreats(QVector<Unit *> midThreats)
{
    this->midThreats=midThreats;
}

void Civilization::setHighThreats(QVector<Unit *> highThreats)
{
    this->highThreats=highThreats;
}

void Civilization::clearThreats()
{
    this->highThreats.clear();
    this->midThreats.clear();
    this->lowThreats.clear();
}

QVector<Unit *> Civilization::getLowThreats()
{
    return this->lowThreats;
}

QVector<Unit *> Civilization::getMidThreats()
{
    return this->midThreats;
}

QVector<Unit *> Civilization::getHighThreats()
{
    return this->highThreats;
}

void Civilization::setProvoked(bool provoked)
{
    this->provoked=provoked;
}

bool Civilization::getProvoked()
{
    return provoked;
}

void Civilization::setCivIndex(int index)
{
    this->civIndex=index;
}

int Civilization::getCivIndex()
{
    return civIndex;
}

void Civilization::setCityFounding(Unit* unit)
{
    this->cityFounding.enqueue(unit);
}

QQueue<Unit *> Civilization::getCityFounding()
{
    return cityFounding;
}

Unit *Civilization::dequeue()
{
    return this->cityFounding.dequeue();
}

bool Civilization::isEmpty()
{
    return this->cityFounding.isEmpty();
}

int Civilization::queueSize()
{
    return this->cityFounding.size();
}

void Civilization::AddCity(City *city)
{
    this->currentCityList.push_back(city);
}

void Civilization::AddUnit(Unit *unit)
{
    this->UnitList.push_back(unit);
}

void Civilization::RemoveCity(int cityIndex)
{
    this->currentCityList.removeAt(cityIndex);
}

void Civilization::RemoveUnit(int unitIndex)
{
    this->UnitList.removeAt(unitIndex);
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

void Civilization::SetCityIndex(int index)
{
    this->cityIndex = index;
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
        qDebug() << "CityList -- Index out of range";
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
        return this->UnitList.at(0);
    }
}



