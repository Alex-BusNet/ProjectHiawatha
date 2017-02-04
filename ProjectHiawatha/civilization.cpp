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
#include <math.h>
#include <QJsonArray>

Civilization::Civilization()
{

}

Civilization::Civilization(Nation name, bool isAI, QString leaderName)
{
    this->name=name;
    this->LeaderName = leaderName;
    //set color
    //spawn a city, and initialize (based on nation)
    //call updatecityyield and updatecivyield to initialize
    this->totalCivYield = new Yield(0, 0, 0, 0, 0);
    this->cityIndex = 0;
    this->totalGold = 0;
    this->totalScience = 0;
    this->totalCulture = 0;
    this->accumulatedScience = 0;
    this->techIndex = 0;
    this->cityFounded = false;
    this->alive = true;
    this->capitalsControlled = 1;
    this->atWar = false;
    this->militaryStrength = 0;

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

/*
 * UpdateCivYield is used to update how much
 * of each yield type a civ generates per turn
 */
void Civilization::UpdateCivYield()
{
    //Reset the civs YPT
    int oldGold = this->totalCivYield->GetGoldYield() * -1,
            oldProd = this->totalCivYield->GetProductionYield() * -1,
            oldSci = this->totalCivYield->GetScienceYield() * -1,
            oldFood = this->totalCivYield->GetFoodYield() * -1,
            oldCul = this->totalCivYield->GetCultureYield() * -1;

    this->totalCivYield->ChangeYield(oldGold, oldProd, oldSci, oldFood, oldCul);

    int newGold = 0, newProd = 0, newSci = 0, newFood = 0, newCul = 0;

    //Recalculate the civs YPT.
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

/*
 * UpdateProgrss handles all the updating for
 * every city a civ object controls. If a city
 * has one, or multiple, of the events occur,
 * then the flag is set in redraw Update_t struct
 * which is returned to the GameManager once the
 * is finished.
 */
Update_t Civilization::UpdateProgress()
{
    static int turn = 0;
    turn++;

    Update_t redraw{false, false, false, false};
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
            redraw.updateCitizens = true;
        }

        if(cityProgress.productionFinished)
        {
            redraw.productionFinished = true;
        }

        if(cityProgress.cityHealed)
        {
            redraw.cityHealed = true;
        }
    }

    this->UpdateCivYield();

    this->totalScience += this->getCivYield()->GetScienceYield();
    this->totalCulture += this->getCivYield()->GetCultureYield();

    /*
     * cost = (n * b*(1 + g*m)/100)^(1 + g / d)
     * n = number of units
     * b = base cost of units
     * m = mulitplier (1)
     * d = divisor (1)
     * g = game progress factor
     *      g = currentTurn / approx. endTurn
     */
    double gpf =  turn / 500.0;

    int maintenance = pow((this->UnitList.size() * 200 * (1 + gpf)) / 100, (1 + gpf));

    this->totalCivYield->ChangeYield(Yield::GOLD, maintenance * -1);
    this->totalGold += this->getCivYield()->GetGoldYield();
//    this->totalGold -= maintenance;

    if(this->totalGold < 0)
    {
        losingGold = true;
    }
    else
    {
        losingGold = false;
    }


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
          int x = techInfo[1].toInt();
          int y = techInfo[2].toInt();

          Technology* tech = new Technology(techInfo[0],x,y);
          techList.push_back(tech);
       }
       inputFile.close();
    }
    else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
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

void Civilization::SetCaptialsControlled(int cc)
{
    this->capitalsControlled = cc;
}

void Civilization::IncrementCapitalsControlled()
{
    this->capitalsControlled++;
}

void Civilization::SetAtWar(int enemyCivListIndex)
{
    this->atWarWithCivListIndex.push_back(enemyCivListIndex);
    this->atWar = true;
}

void Civilization::MakePeace(int enemyCivListIndex)
{
    for(int i = 0; i < this->atWarWithCivListIndex.size(); i++)
    {
        if(this->atWarWithCivListIndex[i] == enemyCivListIndex)
        {
            this->atWarWithCivListIndex.remove(i);
            break;
        }
    }

    if(this->atWarWithCivListIndex.isEmpty())
    {
        this->atWar = false;
    }
}

void Civilization::WriteData(QJsonObject &obj) const
{
    obj["nation"] = name;
    obj["leader"] = LeaderName;
    obj["civindex"] = civIndex;

    QJsonObject yo;
    totalCivYield->WriteYieldSaveData(yo);
    obj["totalcivyield"] = yo;

    obj["totalgold"] = totalGold;
    obj["losinggold"] = losingGold;
    obj["totalscience"] = totalScience;
    obj["totalculture"] = totalCulture;
    obj["accumulatedscience"] = accumulatedScience;
    obj["capitalscontrolled"] = capitalsControlled;
    obj["militarystrength"] = militaryStrength;

    obj["atwar"] = atWar;

    QJsonArray units;
    foreach(Unit* u, this->UnitList)
    {
        QJsonObject unitObject;
        u->WriteUnitSaveData(unitObject);
        units.push_back(unitObject);
    }
    obj["units"] = units;

    QJsonArray cityArray;
    foreach(City *c, this->currentCityList)
    {
        QJsonObject cityJson;
        c->WriteCitySaveData(cityJson);
        cityArray.push_back(cityJson);
    }

    obj["cities"] = cityArray;

    obj["isaiplayer"] = isAIPlayer;
    QJsonObject curTech;
    currentTech->WriteTechSaveData(curTech);
    obj["currenttech"] = curTech;

    QJsonObject nTech;
    nextTech->WriteTechSaveData(nTech);
    obj["nexttech"] = nTech;

//    QJsonArray techArray;
//    foreach(Technology *t, techList)
//    {
//        QJsonObject to;
//        t->WriteTechSaveData(to);
//        techArray.push_back(to);
//    }

//    obj["techlist"] = techArray;

    if(isAIPlayer)
    {
        QJsonArray lowThreatArray;
        foreach(Unit* u, lowThreats)
        {
            QJsonObject ltu;
            u->WriteUnitSaveData(ltu);
            lowThreatArray.push_back(ltu);
        }

        obj["lowthreats"] = lowThreatArray;

        QJsonArray midThreatArray;
        foreach(Unit* u, midThreats)
        {
            QJsonObject mtu;
            u->WriteUnitSaveData(mtu);
            lowThreatArray.push_back(mtu);
        }

        obj["midthreats"] = midThreatArray;

        QJsonArray highThreatArray;
        foreach(Unit* u, highThreats)
        {
            QJsonObject htu;
            u->WriteUnitSaveData(htu);
            lowThreatArray.push_back(htu);
        }

        obj["highthreats"] = highThreatArray;
    }
}

void Civilization::ReadData(const QJsonObject &obj)
{
    name = static_cast<Nation>(obj["nation"].toInt());
    LeaderName = obj["leader"].toString();
    civIndex = obj["civindex"].toInt();

    totalCivYield->ReadYieldSaveData(obj["totalcivyield"].toObject());

    totalGold = obj["totalgold"].toInt();
    totalScience = obj["totalscience"].toInt();
    totalCulture = obj["totalculture"].toInt();
    accumulatedScience = obj["accumulatedscience"].toInt();
    capitalsControlled = obj["capitalscontrolled"].toInt();
    militaryStrength = obj["militarystrength"].toInt();
    atWar = obj["atwar"].toBool();

    QJsonArray uArray = obj["units"].toArray();
    for(int i = 0; i < uArray.size(); i++)
    {
        Unit* unit = new Unit();
        unit->ReadUnitSaveData(uArray.at(i).toObject());

        UnitList.push_back(unit);
    }

    QJsonArray cArray = obj["cities"].toArray();
    for(int i = 0; i < cArray.size(); i++)
    {
        City* city = new City();
        city->ReadCitySaveData(cArray.at(i).toObject());
    }

    currentTech->ReadTechSaveData(obj["currenttech"].toObject());
    nextTech->ReadTechSaveData(obj["nexttech"].toObject());

    isAIPlayer = obj["isaiplayer"].toBool();

    if(isAIPlayer)
    {
//        QJsonArray ltArray = obj["lowthreats"].toArray();
        //Load threat vectors as vector<int> and process later to find
        //proper unit. the stored int should be the units currentTile.
    }
}

int Civilization::getTechIndex()
{
    return this->techIndex;
}

int Civilization::GetCapitalsControlled()
{
    return this->capitalsControlled;
}

QVector<int> Civilization::GetCivListIndexAtWar()
{
    return this->atWarWithCivListIndex;
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

/*
 * loadCities creates the list of city names that
 * a civ will use as it founds new cities in the game
 */
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

          initialCityList.push_back(cityInfo[0]);

       }
       inputFile.close();
    }
    else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();

    }
}

/*
 * The following threat related functions are for the AI's use.
 */
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

void Civilization::setCivIndex(int index)
{
    this->civIndex=index;
}

void Civilization::SetMilitaryStrength(int milStr)
{
    this->militaryStrength = milStr;
}

int Civilization::getCivIndex()
{
    return civIndex;
}

int Civilization::GetMilitaryStrength()
{
    return this->militaryStrength;
}

/*
 * setCityFounding is used by the AI to alert the
 * GameManager that it wants to found or conquer a
 * city. The remainder of the logic is carried out
 * by the GameManager. This function does not supply
 * any feedback message to the AI. The AI always assumes
 * the action was completed, and will not notice, nor
 * interfere with it's logic should the manager not allow
 * a particular request to be fully granted.
 */
void Civilization::setCityFounding(AIQueueData data)
{
    this->cityFounding.enqueue(data);
}

QQueue<AIQueueData> Civilization::getCityFounding()
{
    return cityFounding;
}

/*
 * dequeue, isEmpty, and queueSize are all functions
 * used by GameManager to retrieve requests made by the
 * AI or check if it needs to process any requests.
 */
AIQueueData Civilization::dequeue()
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

bool Civilization::isAtWar()
{
    return this->atWar;
}

void Civilization::AddCity(City *city)
{
    this->currentCityList.push_back(city);
}

void Civilization::AddUnit(Unit *unit)
{
    this->UnitList.push_back(unit);
}

/*
 * RemoveCity is used when a civ has a
 * city conquered. This is so all control
 * is relinquished to the conquering civ.
 */
void Civilization::RemoveCity(int cityIndex)
{
    this->currentCityList.removeAt(cityIndex);
    for(int i = 0; i < currentCityList.size(); i++)
    {
        currentCityList.at(i)->SetCityIndex(i);

        if(i == 0 && !currentCityList.at(i)->IsCityCaptial())
        {
            currentCityList.at(i)->SetCityAsCapital(true, false);
        }
    }

    if(currentCityList.size() == 0)
    {
        this->alive = false;
    }
}

/*
 * This function is used when a unit is killed,
 * when civ loses a city the unit is garrisoned in,
 * or the controlling civ is eliminated from the game.
 * This function also adjusts all of the UnitListIndex
 * attirbutes of the remaining units.
 */
void Civilization::RemoveUnit(int unitIndex)
{
    this->UnitList.removeAt(unitIndex);
    for(int i = 0; i < UnitList.size(); i++)
    {
        UnitList.at(i)->SetUnitListIndex(i);
    }
}

/*
 * SetCityIndex is used to keep track of what the
 * next city shall be called.
 */
void Civilization::SetCityIndex(int index)
{
    this->cityIndex = index;
}

void Civilization::SetLeaderName(QString name)
{
    this->LeaderName = name;
}

QString Civilization::GetLeaderName()
{
    return this->LeaderName;
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
        //If the index is too large, return the first unit
        // the civ owns.
        return this->UnitList.at(0);
    }
}



