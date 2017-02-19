#include "city.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <math.h>
#include <QJsonArray>
#include "queuedata.h"

City::City()
{
    this->cityTotalYield = new Yield(1,1,1,1,1);
    this->turnsToBorderGrowth = 0;
    this->accumulatedProduction = 0;
    this->currentProductionCost = 0;
    this->productionFinished = false;
    this->productionUnit = false;
    this->currentProductionName = "No Current Production";
    this->numberofBuildings = 0;
    this->cityFocus = Yield::GOLD;
    this->controllingCiv = NO_NATION;
    this->cityHealth = 200;
    this->maxHealth = 200;
    this->baseStrength = 10;
    this->citizens = 0;
    this->cityStrength = this->baseStrength + this->citizens;
    this->buildingStrength = 0;
    this->initialized = false;
    this->stagnant = false;
    this->isCaptial = false;
    this->isOriginalCapital = false;
    this->fullyExpanded = false;
    this->hasWorker = false;
    this->hasGarrison = false;
    this->cityID = -1;
    this->goldYield = 0;
    this->productionYield = 0;
    this->scienceYield = 0;

    this->StationedMilitary = NULL;
    this->StationedWorkers = NULL;
    this->producedUnit = NULL;
}


City::~City()
{
    foreach(Unit* unit, initialUnitList)
    {
        if(unit != NULL)
            delete unit;
    }

    foreach(Building* b, initialBuildingList)
    {
        if(b != NULL)
            delete b;
    }

    if(!cityControlledTiles.isEmpty())
        cityControlledTiles.clear();

    if(!producedBuildings.isEmpty())
        producedBuildings.clear();

    if(!maximumExpansionBorderTiles.isEmpty())
        maximumExpansionBorderTiles.clear();

    if(!borderTiles.isEmpty())
        borderTiles.clear();

    if(!tileQueue.isEmpty())
        tileQueue.clear();

    if(!borderQueue.isEmpty())
        borderQueue.clear();

    if(StationedMilitary != NULL)
        delete StationedMilitary;

    if(StationedWorkers != NULL)
        delete StationedWorkers;

    if(producedUnit != NULL)
        delete producedUnit;

    if(cityTotalYield != NULL)
        delete cityTotalYield;
}

QVector<Unit *> City::getInitialUnitList()
{
    return this->initialUnitList;
}

QVector<Building *> City::getInitialBuildingList()
{
    return this->initialBuildingList;
}

QVector<Building *> City::getCurrentBuildingList()
{
    return this->producedBuildings;
}

void City::SortControlledTiles()
{
    for(int i = 0; i < cityControlledTiles.size(); i++)
    {
        for(int j = i + 1; j < cityControlledTiles.size(); j++)
        {
            if(cityControlledTiles[j]->GetYield()->GetYield(cityFocus) > cityControlledTiles[i]->GetYield()->GetYield(cityFocus))
            {
                Tile* temp = cityControlledTiles[i];
                cityControlledTiles[i] = cityControlledTiles[j];
                cityControlledTiles[j] = temp;
            }
        }
    }

    for(int k = 0; k < this->cityControlledTiles.size(); k++)
    {
        if(k < this->citizens)
        {
            this->cityControlledTiles.at(k)->IsWorked = true;
        }
        else
        {
            break;
        }
    }
}

void City::SortTileQueue()
{
    // Select the next tile to be claimed by a city based on:
    //  -Is the tile already claimed by another civ?
    //  -The total yield of the tile
    //  -Resources on tile
    //  -Tile type

    //Assume the tile is in the MEB list until proven otherwise
    bool inMEB = true;
    for(int k = 0; k < tileQueue.size(); k++)
    {
        foreach(Tile* meb, maximumExpansionBorderTiles)
        {
            if(tileQueue.at(k)->GetTileID().column != meb->GetTileID().column)
            {
                if(tileQueue.at(k)->GetTileID().row != meb->GetTileID().row)
                {
                    inMEB = false;
                }
            }
            else if(tileQueue.at(k)->GetTileID().column == meb->GetTileID().column)
            {
                if(tileQueue.at(k)->GetTileID().row == meb->GetTileID().row)
                {
                    // If we find the tile in the MEB list, then stop searching the list
                    inMEB = true;
                    break;
                }
            }
        }

        if(!inMEB)
        {
            inMEB = true;
            tileQueue.remove(k);
        }
    }


    if(this->tileQueue.isEmpty())
    {
        this->fullyExpanded = true;
        return;
    }

    for(int i = 0; i < tileQueue.size(); i++)
    {
        for(int j = i + 1; j < tileQueue.size(); j++)
        {
            if(tileQueue[j]->GetYield()->GetYield(cityFocus) > tileQueue[i]->GetYield()->GetYield(cityFocus))
            {
                Tile* temp = tileQueue[i];
                tileQueue[i] = tileQueue[j];
                tileQueue[j] = temp;
            }
        }
    }
}

void City::FilterMEBList()
{
    int i = 0;
    foreach(Tile* tile, this->maximumExpansionBorderTiles)
    {
        if(tile->GetControllingCiv() != NO_NATION || tile->GetTileType() == ICE)
        {
            this->maximumExpansionBorderTiles.removeAt(i);
        }

        i++;
    }
}

int City::getNumberOfBuildings()
{
    return this->numberofBuildings;
}

int City::GetTurnsToBorderGrowth()
{
    return this->turnsToBorderGrowth;
}

void City::IncrementNumberOfBuildings()
{
    this->numberofBuildings++;
}

Update_t City::UpdateProgress()
{
    Update_t update{false, false, false, false};

    if(turnsToBorderGrowth == 0 && !fullyExpanded)
    {
        qDebug() << "       Update City Borders" << this->GetName();
        //Gets the first available tile from the tile queue, and adds it to cityControlledTiles.
        foreach(Tile* tile, this->tileQueue)
        {
            if(tileQueue.isEmpty())
            {
                this->fullyExpanded = true;
                this->turnsToBorderGrowth = 0;
                break;
            }

            if(tile->GetControllingCiv() == NO_NATION)
            {
                this->AddControlledTile(this->tileQueue.first());
                this->tileQueue.removeFirst();
                break;
            }
            else
            {
                this->tileQueue.removeFirst();
            }
        }

        //Calculate the number of turns until a new tile is grabbed by the city.
        this->turnsToBorderGrowth = floor((20 + (pow(10*(this->cityControlledTiles.size() - 1), 1.1))) / this->cityTotalYield->GetCultureYield());
        update.updateBorders = true;
    }
    else
    {
        this->turnsToBorderGrowth--;
    }

    this->turnsToNewCitizen--;

    if(turnsToNewCitizen == 0 && !this->stagnant)
    {
        //Increment the number of citizens in the city
        this->citizens++;

        // Calculate the number of turns until a new citizen is born
        this->growthCost = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));
        this->foodSurplus = this->cityTotalYield->GetFoodYield() - (2 * this->citizens);

        if(this->foodSurplus < 0)
        {
            this->foodSurplus *= -1;
        }
        else if(this->foodSurplus == 0)
        {
            this->foodSurplus = 1;
        }

        this->turnsToNewCitizen = 2 * (this->growthCost / this->foodSurplus);

        if(this->turnsToNewCitizen < 0)
        {
            stagnant = true;
        }

        if(this->citizens < this->GetControlledTiles().size())
        {
            foreach(Tile* tile, this->cityControlledTiles)
            {
                if(tile->GetTileType() != MOUNTAIN && !tile->IsWorked)
                {
                    tile->IsWorked = true;
                    update.updateCitizens = true;
                    break;
                }
            }
        }

        this->UpdateCityYield();
    }

    if(this->currentProductionCost > 0)
    {
        this->accumulatedProduction += this->cityTotalYield->GetProductionYield();
    }

    if(this->accumulatedProduction >= this->currentProductionCost && (this->currentProductionName != "No Current Production"))
    {
        this->accumulatedProduction = 0;
        this->currentProductionName = "No Current Production";
        this->currentProductionCost = 0;
        update.productionFinished = true;
        this->productionFinished = true;

        if(this->getIsUnit())
            QueueData::enqueue(CityProdData{this->cityIndex, this->productionIndex, true});
        else
            QueueData::enqueue(CityProdData{this->cityIndex, this->productionIndex, false});
    }

    if(this->cityHealth < this->maxHealth)
    {
        this->cityHealth += this->maxHealth * 0.15f;

        if(this->cityHealth > this->maxHealth)
            this->cityHealth = this->maxHealth;

        update.cityHealed = true;
    }

    this->cityStrength = this->baseStrength + this->citizens + this->buildingStrength;

    return update;
}

void City::SetCityFocus(Yield::YIELD focus)
{
    this->cityFocus = focus;
    foreach(Tile* tile, cityControlledTiles)
    {
        tile->IsWorked = false;
    }

    this->SortTileQueue();
    this->SortControlledTiles();
    this->UpdateCityYield();
}

Yield::YIELD City::GetCityFocus()
{
    return this->cityFocus;
}

// 0 = p, q, and r are colinear
// 1 = Clockwise
// 2 = Counterclockwise

int City::orientation(QPoint p, QPoint q, QPoint r)
{
    int val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());

    if(val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

//Accessor and Mutators
void City::SetCityAsCapital(bool capitalStatus, bool originalCapital)
{
    this->isCaptial = capitalStatus;
    this->isOriginalCapital = originalCapital;
}

void City::SetName(QString name)
{
    this->name = name;
}

void City::SetCityTile(Tile *tile)
{
    this->cityTile = tile;
    tile->SetGoverningCity(this->cityID);
    tile->CanAlwaysBeSeen = true;

    int x = tile->GetCenter().x(), y = tile->GetCenter().y();
    this->minimumSettleDistance << QPoint(x - 88, y - 147)
                                 << QPoint(x + 88, y - 147)
                                 << QPoint(x + 176, y)
                                 << QPoint(x + 88, y + 147)
                                 << QPoint(x - 88, y + 147)
                                 << QPoint(x - 176, y);

    this->maximumExpansionBorder << QPoint(x - 176, y - 292)
                                 << QPoint(x + 176, y - 292)
                                 << QPoint(x + 352, y)
                                 << QPoint(x + 176, y + 292)
                                 << QPoint(x - 176, y + 292)
                                 << QPoint(x - 352, y);
}

void City::SetControllingCiv(Nation owner)
{
    this->controllingCiv = owner;
}

void City::SetCityIndex(int index)
{
    this->cityIndex = index;
}

void City::UpdateCityYield()
{
    //Clear the current YPT
    int oldGold = this->cityTotalYield->GetGoldYield() * -1,
            oldProd = this->cityTotalYield->GetProductionYield() * -1,
            oldSci = this->cityTotalYield->GetScienceYield() * -1,
            oldFood = this->cityTotalYield->GetFoodYield() * -1,
            oldCul = this->cityTotalYield->GetCultureYield() * -1;

    this->cityTotalYield->ChangeYield(oldGold, oldProd, oldSci, oldFood, oldCul);

    //Recalculate the city's YPT
    int newGold = cityTile->GetYield()->GetGoldYield(),
            newProd = cityTile->GetYield()->GetProductionYield(),
            newSci = cityTile->GetYield()->GetScienceYield(),
            newFood = cityTile->GetYield()->GetFoodYield(),
            newCul = cityTile->GetYield()->GetCultureYield();

    foreach(Tile *tile, cityControlledTiles)
    {
        if(tile->IsWorked)
        {
            newGold += tile->GetYield()->GetGoldYield();
            newProd += tile->GetYield()->GetProductionYield();
            newSci += tile->GetYield()->GetScienceYield();
            newFood += tile->GetYield()->GetFoodYield();
            newCul += tile->GetYield()->GetCultureYield();
        }
    }

    if(this->stagnant && (newFood > (oldFood * -1)))
    {
        this->stagnant = false;
    }

    this->cityTotalYield->ChangeYield(newGold, newProd, newSci, newFood, newCul);
}

void City::UpdateCityStatus()
{
    // Border Growth Calculation
    if(!this->fullyExpanded)
        this->turnsToBorderGrowth = floor((20 + (pow(10*(this->cityControlledTiles.size() - 1), 1.1))) / this->cityTotalYield->GetCultureYield());

    // Citizen Growth Calculation
    this->turnsToNewCitizen = 2 * (this->growthCost / this->foodSurplus);

    if(this->turnsToNewCitizen < 0)
    {
        stagnant = true;
    }

    // City Strength calculation
    this->cityStrength = this->baseStrength + this->citizens + this->buildingStrength;

    // Set number of buildings in city
    this->numberofBuildings = this->producedBuildings.size();
}

void City::SetCityRenderIndex(int index)
{
    this->cityRenderIndex = index;
}

void City::SetCityID(int ID)
{
    this->cityID = ID;
}

void City::SetCityBorders(QPolygon borders)
{
    if(!this->cityBorder.isEmpty())
        this->cityBorder.clear();

    this->cityBorder = borders;
}

void City::SetCityHealth(float health)
{
    this->cityHealth = health;
}

void City::SetCityMaxHealth(int health)
{
    this->maxHealth = health;
}

void City::SetCityStrength(int strength)
{
    this->cityStrength = strength;
}

void City::SetCityBuildingStrength(int strength)
{
    this->buildingStrength = strength;
}

void City::SetBaseCityStrength(int strength)
{
    this->baseStrength += strength;
}

void City::GarrisonWorker(Unit *worker)
{
    if(worker->isNonCombat())
    {
        this->StationedWorkers = worker;
        this->hasWorker = true;
        worker->isGarrisoned=true;
    }
}

void City::RemoveGarrisonWorker()
{
    if(this->getHasWorker())
    {
        delete this->StationedWorkers;
        this->hasWorker = false;
    }
}

void City::GarrisonMilitary(Unit *military)
{
    if(!military->isNonCombat())
    {
        this->StationedMilitary = military;
        this->hasGarrison = true;
        military->isGarrisoned=true;
    }
}

void City::RemoveGarrisonMilitary()
{
    if(this->hasGarrison)
    {
        delete this->StationedMilitary;
        this->hasGarrison = false;
    }
}

void City::AddControlledTile(Tile *tile)
{
    tile->SetControllingCiv(this->controllingCiv, this->cityTile->GetControllingCivListIndex());
    tile->SetGoverningCity(this->cityID);
    this->cityControlledTiles.push_back(tile);
}

void City::SetMaximumExpansionBorderTiles(QList<Tile *> tileVect)
{
    foreach(Tile* tile, tileVect)
    {
        this->maximumExpansionBorderTiles.push_back(tile);
    }

}

QString City::GetName()
{
    return this->name;
}

Tile *City::GetCityTile()
{
    return this->cityTile;
}

bool City::IsCityCaptial()
{
    return this->isCaptial;
}

bool City::IsOriginalCapital()
{
    return this->isOriginalCapital;
}

void City::setCapitolConnection(bool flag)
{
    this->hasCapitolConnection=flag;
}

bool City::getCapitolConnection()
{
    return this->hasCapitolConnection;
}

bool City::IsInitialized()
{
    return this->initialized;
}

bool City::IsStagnant()
{
    return this->stagnant;
}

bool City::IsFullyExpanded()
{
    return this->fullyExpanded;
}

void City::InitializeCity()
{
    foreach(Tile* tile, this->cityControlledTiles)
    {
        tile->SetGoverningCity(this->cityID);
    }

    this->turnsToBorderGrowth = floor((20 + (10*pow(this->cityControlledTiles.size() - 1, 1.1))) / this->cityTotalYield->GetCultureYield());
    this->foodSurplus = this->cityTotalYield->GetFoodYield() - (2 * this->citizens);
    this->growthCost = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));
    this->turnsToNewCitizen = this->growthCost / this->foodSurplus;

    this->initialized = true;
}

void City::WriteCitySaveData(QJsonObject &obj) const
{
    obj["name"] = name;
    obj["citytileindex"] = cityTile->GetTileIndex();
    obj["cityid"] = cityID;
    obj["population"] = citizens;
    obj["cityindex"] = cityIndex;
    obj["controlledby"] = controllingCiv;
    obj["turnstobordergrowth"] = turnsToBorderGrowth;
    obj["turnstonewcitizen"] = turnsToNewCitizen;
    obj["hp"] = cityHealth;
    obj["maxhp"] = maxHealth;
    obj["focus"] = cityFocus;
    obj["iscapital"] = isCaptial;
    obj["isoriginalcapital"] = isOriginalCapital;
    obj["stagnant"] = stagnant;
    obj["fullyexpanded"] = fullyExpanded;
    obj["renderindex"] = cityRenderIndex;
    obj["currentproductionname"] = currentProductionName;
    obj["currentproductioncost"] = currentProductionCost;
    obj["accumulatedproduction"] = accumulatedProduction;
    obj["productionindex"] = productionIndex;
    obj["productionyield"] = productionYield;

    QJsonArray buildingArray;
    foreach(Building *b, producedBuildings)
    {
        QJsonObject bo;
        b->WriteBuildingSaveData(bo);
        buildingArray.append(bo);
    }

    obj["completedbuildings"] = buildingArray;
    obj["buildingstrength"] = buildingStrength;
    obj["basestrength"] = baseStrength;
    obj["citystrength"] = cityStrength;

    QJsonArray controlledArray;
    foreach(Tile* t, cityControlledTiles)
    {
        controlledArray.append(t->GetTileIndex());
    }

    obj["controlledtiles"] = controlledArray;

    obj["goldyield"] = goldYield;
    obj["productionyield"] = productionYield;
    obj["scienceyield"] = scienceYield;
    obj["growthcost"] = growthCost;
    obj["foodsurplus"] = foodSurplus;

    obj["hasworker"] = hasWorker;
    obj["hasgarrison"] = hasGarrison;
}

void City::ReadCitySaveData(const QJsonObject &obj)
{
    name = obj["name"].toString();
    cityID = obj["cityid"].toInt();
    citizens = obj["population"].toInt();
    cityIndex = obj["cityindex"].toInt();
    controllingCiv = static_cast<Nation>(obj["controlledby"].toInt());
    turnsToBorderGrowth = obj["turnstobordergrowth"].toInt();
    turnsToNewCitizen = obj["turnstonewcitizen"].toInt();
    cityHealth = obj["hp"].toDouble();
    maxHealth = obj["maxhp"].toDouble();
    cityFocus = static_cast<Yield::YIELD>(obj["focus"].toInt());
    isCaptial = obj["iscapital"].toBool();
    isOriginalCapital = obj["isoriginalcapital"].toBool();
    stagnant = obj["stagnant"].toBool();
    fullyExpanded = obj["fullyexpanded"].toBool();
    cityRenderIndex = obj["renderindex"].toInt();
    currentProductionName = obj["currentproductionname"].toString();
    currentProductionCost = obj["currentproductioncost"].toInt();
    accumulatedProduction = obj["accumulatedproduction"].toInt();
    productionIndex = obj["productionindex"].toInt();
    productionYield = obj["productionyield"].toInt();
    loadedCityTileIndex = obj["citytileindex"].toInt();

    QJsonArray bArray = obj["completedbuildings"].toArray();
    for(int i = 0; i < bArray.size(); i++)
    {
        Building* b = new Building();
        b->ReadBuildingSaveData(bArray.at(i).toObject());
        producedBuildings.push_back(b);
    }

    QJsonArray controlledArray = obj["controlledtiles"].toArray();
    for(int i = 0; i < controlledArray.size(); i++)
    {
        controlledTilesIndex.push_back(controlledArray.at(i).toInt());
    }

    buildingStrength = obj["buildingStrength"].toInt();
    baseStrength = obj["basestrength"].toInt();
    cityStrength = obj["citystrength"].toInt();

    goldYield = obj["goldyield"].toInt();
    productionYield = obj["productionyield"].toInt();
    scienceYield = obj["scienceyield"].toInt();
    growthCost = obj["growthcost"].toInt();
    foodSurplus = obj["foodsurplus"].toInt();

    hasWorker = obj["hasWorker"].toBool();
    hasGarrison = obj["hasGarrison"].toBool();
}

void City::SetCitizenCount(int count)
{
    this->citizens = count;
}

void City::SetStagnant(bool isStagnant)
{
    this->stagnant = isStagnant;
}

void City::SetGrowthCost(int cost)
{
    this->growthCost = cost;
}

void City::SetFoodSurplus(int surplus)
{
    this->foodSurplus = surplus;
}

void City::SetBuildingStrength(int strength)
{
    this->buildingStrength = strength;
}

void City::SetFullyExpanded(bool fullExpansion)
{
    this->fullyExpanded = fullExpansion;
}

int City::GetCitizenCount()
{
    return this->citizens;
}

int City::GetFoodNeededToGrow()
{
    return this->growthCost;
}

int City::GetTurnsToNewCitizen()
{
    return this->turnsToNewCitizen;
}

int City::GetFoodSurplus()
{
    return this->foodSurplus;
}

int City::getCurrentProductionCost()
{
    return this->currentProductionCost;
}

void City::setCurrentProductionCost(int cost)
{
    this->currentProductionCost = cost;
}

int City::getAccumulatedProduction()
{
    return this->accumulatedProduction;
}

void City::setAccumulatedProduction(int total)
{
    this->accumulatedProduction += total;
}

void City::resetAccumulatedProduction()
{
    this->accumulatedProduction = 0;
}

void City::setProductionName(QString name)
{
    this->currentProductionName = name;
}

void City::setProductionIndex(int index)
{
    this->productionIndex = index;
}

void City::setProductionFinished(bool isFinished)
{
    this->productionFinished = isFinished;
}

bool City::getProductionFinished()
{
    return this->productionFinished;
}

void City::setIsUnit(bool isUnit)
{
    this->productionUnit = isUnit;
}

bool City::getIsUnit()
{
    return this->productionUnit;
}

int City::getProductionIndex()
{
    return this->productionIndex;
}

Unit *City::getProducedUnit()
{
    return this->producedUnit;
}

void City::setProducedUnit(Unit *unit)
{
    this->producedUnit = unit;
}

QString City::getProductionName()
{
    return this->currentProductionName;
}

bool City::getHasWorker()
{
    return this->hasWorker;
}

bool City::HasGarrisonUnit()
{
    return this->hasGarrison;
}

bool City::MSDIntersects(QPolygon targetMSD)
{
    QList<QPoint> t_MSD;
    QList<QPoint> c_MSD;

    foreach(QPoint pt, targetMSD)
    {
        t_MSD.push_back(pt);
    }

    foreach(QPoint pt, this->minimumSettleDistance)
    {
        c_MSD.push_back(pt);
    }

    int x, lastX, y, lastY, c_dstX, c_dstY, t_dstX, t_dstY, c_index = 0;
    float c_slope, t_slope;
    bool lrtbValid = false, rltbValid = false, rlbtValid = false, lrbtValid = false;

    foreach(QPoint pt, t_MSD)
    {
        lastX = c_MSD.at(c_MSD.size() - 1).x();
        lastY = c_MSD.at(c_MSD.size() - 1).y();
        x = c_MSD.at(c_index).x();
        y = c_MSD.at(c_index).y();

        //Compare the current point in t_MSD to every point in c_MSD
        for(int i = 0; i < c_MSD.size(); i++)
        {
            c_dstY = y - lastY;
            c_dstX = x - lastX;
            c_slope = static_cast<float>(c_dstY) / c_dstX;

            t_dstY = pt.y() - lastY;
            t_dstX = pt.x() - lastX;
            t_slope = static_cast<float>(t_dstY) / t_dstX;

            if(c_slope == t_slope)
                return false;

            if(x > lastX && y < lastY)
            {
                //Moving left to right bottom to top

                t_slope = (static_cast<float>(lastY) - pt.y()) / (lastX - pt.x());
                float currentSlope = (static_cast<float>(y - pt.y())) / (x - pt.x());

                t_slope = fabs(t_slope);
                currentSlope = fabs(currentSlope);

                if(t_slope > currentSlope)
                {
                    lrbtValid = true;
                }

            }
            else if(x > lastX && y > lastY)
            {
                //Moving left to right, top to bottom

                t_slope = (static_cast<float>(lastY) - pt.y()) / (lastX - pt.x());
                float currentSlope = (static_cast<float>(y - pt.y())) / (x - pt.x());

                t_slope = fabs(t_slope);
                currentSlope = fabs(currentSlope);

                if(t_slope < currentSlope)
                {
                    lrtbValid = true;
                }

            }
            else if(x < lastX && y > lastY)
            {
                //Moving right to left, top to bottom

                t_slope = (static_cast<float>(lastY) - pt.y()) / (lastX - pt.x());
                float currentSlope = (static_cast<float>(y - pt.y())) / (x - pt.x());

                t_slope = fabs(t_slope);
                currentSlope = fabs(currentSlope);

                if(t_slope > currentSlope)
                {
                    rltbValid = true;
                }

            }
            else if(x < lastX && y < lastY)
            {
                //Moving right to left, bottom to top

                t_slope = (static_cast<float>(lastY) - pt.y()) / (lastX - pt.x());
                float currentSlope = (static_cast<float>(y - pt.y())) / (x - pt.x());

                t_slope = fabs(t_slope);
                currentSlope = fabs(currentSlope);

                if(t_slope < currentSlope)
                {
                    rlbtValid = true;
                }

            }

            if((rltbValid || lrbtValid) || (lrtbValid || rlbtValid))
            {
                lrtbValid = false;
                lrbtValid = false;
                rltbValid = false;
                rlbtValid = false;

                //MSDs intersect
                return true;
            }

            lrtbValid = false;
            lrbtValid = false;
            rltbValid = false;
            rlbtValid = false;

            lastX = x;
            lastY = y;
            c_index++;

            if(c_index == c_MSD.size())
                c_index = 0;

            x = c_MSD.at(c_index).x();
            y = c_MSD.at(c_index).y();
        }

        c_index = 0;
    }

    //MSDs do not intersect
    return false;
}

void City::loadUnits(QString filename)
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
          tempUnit->setUnitType(type);
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

void City::loadBuildings(QString filename)
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
          initialBuildingList.push_back(building);
       }
       inputFile.close();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();

    }
}

void City::addBuilding(Building *building)
{
    this->producedBuildings.push_back(building);
    IncrementNumberOfBuildings();
    int bonusInt = building->getbonusType();
    if(bonusInt != 5) // 5 = Defense bonus type
    {
        Yield::YIELD bonusType = static_cast<Yield::YIELD>(bonusInt);
        cityTile->GetYield()->ChangeYield(bonusType, building->getBonusValue());
    }
    else
    {
        this->buildingStrength += building->getBonusValue();
    }
}

Yield* City::getCityYield()
{
    return this->cityTotalYield;
}

Nation City::GetControllingCiv()
{
    return this->controllingCiv;
}

QVector<Tile *> City::GetControlledTiles()
{
    return this->cityControlledTiles;
}

QList<Tile *> City::GetMaximumExpansionBorderTiles()
{
    return this->maximumExpansionBorderTiles;
}

QPolygon City::GetCityBorders()
{
    return this->cityBorder;
}

QPolygon City::GetMaximumExpansionBorder()
{
    return this->maximumExpansionBorder;
}

QPolygon City::GetMinimumSettleDistance()
{
    return this->minimumSettleDistance;
}

Unit *City::GetGarrisonedWorker()
{
    if(this->StationedWorkers != NULL)
    {
        return this->StationedWorkers;
    }

    return NULL;
}

Unit *City::GetGarrisonedMilitary()
{
    if(this->StationedMilitary != NULL)
    {
        return this->StationedMilitary;
    }

    return NULL;
}

int City::GetCityIndex()
{
    return this->cityIndex;
}

int City::GetCityRenderIndex()
{
    return this->cityRenderIndex;
}

int City::GetCityID()
{
    return this->cityID;
}

int City::GetCityHealth()
{
    return ceil(this->cityHealth);
}

int City::GetMaxHealth()
{
    return this->maxHealth;
}

int City::GetCityStrength()
{
    return this->cityStrength;
}

int City::GetCityBuildingStrength()
{
    return this->buildingStrength;
}

int City::GetBaseStrength()
{
    return this->baseStrength;
}
