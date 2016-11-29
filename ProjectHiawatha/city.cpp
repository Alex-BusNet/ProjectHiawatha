#include "city.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>

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
    this->cityFocus = GOLD_FOCUS;
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
}


City::~City()
{

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
            if(this->cityFocus == GOLD_FOCUS)
            {
                if(cityControlledTiles[j]->GetYield()->GetGoldYield() > cityControlledTiles[i]->GetYield()->GetGoldYield())
                {
                    Tile* temp = cityControlledTiles[i];
                    cityControlledTiles[i] = cityControlledTiles[j];
                    cityControlledTiles[j] = temp;
                }
            }
            else if(this->cityFocus == PRODUCTION_FOCUS)
            {
                if(cityControlledTiles[j]->GetYield()->GetProductionYield() > cityControlledTiles[i]->GetYield()->GetProductionYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
            else if(this->cityFocus == SCIENCE_FOCUS)
            {
                if(cityControlledTiles[j]->GetYield()->GetScienceYield() > cityControlledTiles[i]->GetYield()->GetScienceYield())
                {
                    Tile* temp = cityControlledTiles[i];
                    tileQueue[i] = cityControlledTiles[j];
                    cityControlledTiles[j] = temp;
                }
            }
            else if(this->cityFocus == FOOD_FOCUS)
            {
                if(cityControlledTiles[j]->GetYield()->GetFoodYield() > cityControlledTiles[i]->GetYield()->GetFoodYield())
                {
                    Tile* temp = cityControlledTiles[i];
                    cityControlledTiles[i] = cityControlledTiles[j];
                    cityControlledTiles[j] = temp;
                }
            }
            else if(this->cityFocus == CULTURE_FOCUS)
            {
                if(cityControlledTiles[j]->GetYield()->GetCultureYield() > cityControlledTiles[i]->GetYield()->GetCultureYield())
                {
                    Tile* temp = cityControlledTiles[i];
                    cityControlledTiles[i] = cityControlledTiles[j];
                    cityControlledTiles[j] = temp;
                }
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

    qDebug() << "   ---Sorting TileQueue; MEB Filter";

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
                    inMEB = true;
                }
            }
        }

        if(!inMEB)
        {
            qDebug() << "Tile" << tileQueue.at(k)->GetTileIDString() << "not in MEB; Removing";
            inMEB = true;
            tileQueue.remove(k);
        }
    }


    if(this->tileQueue.isEmpty())
    {
        qDebug() << "-City fully expanded";
        this->fullyExpanded = true;
        return;
    }

    for(int i = 0; i < tileQueue.size(); i++)
    {
        for(int j = i + 1; j < tileQueue.size(); j++)
        {
            if(this->cityFocus == GOLD_FOCUS)
            {
                if(tileQueue[j]->GetYield()->GetGoldYield() > tileQueue[i]->GetYield()->GetGoldYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
            else if(this->cityFocus == PRODUCTION_FOCUS)
            {
                if(tileQueue[j]->GetYield()->GetProductionYield() > tileQueue[i]->GetYield()->GetProductionYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
            else if(this->cityFocus == SCIENCE_FOCUS)
            {
                if(tileQueue[j]->GetYield()->GetScienceYield() > tileQueue[i]->GetYield()->GetScienceYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
            else if(this->cityFocus == FOOD_FOCUS)
            {
                if(tileQueue[j]->GetYield()->GetFoodYield() > tileQueue[i]->GetYield()->GetFoodYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
            else if(this->cityFocus == CULTURE_FOCUS)
            {
                if(tileQueue[j]->GetYield()->GetCultureYield() > tileQueue[i]->GetYield()->GetCultureYield())
                {
                    Tile* temp = tileQueue[i];
                    tileQueue[i] = tileQueue[j];
                    tileQueue[j] = temp;
                }
            }
        }
    }
}

void City::FilterMEBList()
{
    qDebug() << "-Filtering MEB list";
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

void City::IncrementNumberOfBuildings()
{
    this->numberofBuildings++;
}

Update_t City::UpdateProgress()
{
    Update_t update{false, false, false};

    if(turnsToBorderGrowth == 0 && !fullyExpanded)
    {
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

        //Redefine the city's borders
        this->DefineCityBorders(true);

        //Calculate the number of turns until a new tile is grabbed by the city.
        this->turnsToBorderGrowth = floor((20 + (pow(10*(this->cityControlledTiles.size() - 1), 1.1))) / this->cityTotalYield->GetCultureYield());
        update.updateBorders = true;
    }
    else
    {
        this->turnsToBorderGrowth--;
    }

    qDebug() << "       " << this->name << " turns to growth:" << turnsToBorderGrowth;

    this->turnsToNewCitizen--;

    if(turnsToNewCitizen == 0 && !this->stagnant)
    {
        //Increment the number of citizens in the city
        qDebug() << "--Increment Citizens";
        this->citizens++;

        //// THIS CALCULATION NEEDS TO CHANGE
        // Calculate the number of turns until a new citizen is born
        qDebug() << "--Calculate growth cost";
        this->growthCost = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));
        qDebug() << "--Calculate food surplus; food yield:" << this->cityTotalYield->GetFoodYield() << "citizens:" << this->citizens;
        this->foodSurplus = this->cityTotalYield->GetFoodYield() - (2 * this->citizens);
        qDebug() << "--Calculate turns to new citizen; Surplus:" << this->foodSurplus;
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
            qDebug() << "----City is stagnant";
            stagnant = true;
        }

        // If the city has more tiles than there are citizens then put a citizen to work.
        if(this->citizens < this->GetControlledTiles().size())
        {
            qDebug() << "---City has more tiles than citizens";
            foreach(Tile* tile, this->cityControlledTiles)
            {
                // If the tile is not a mountain and is not already worked, then work that tile.
                if(tile->GetTileType() != MOUNTAIN && !tile->IsWorked)
                {
                    qDebug() << "--Tile not mountain and not worked";
                    tile->IsWorked = true;
                    update.updateCitizens = true;
                    break;
                }
            }
            qDebug() << "updating city yield";
            this->UpdateCityYield();
        }
    }

    qDebug() << "       " << this->name << " turns to new citizen" << turnsToNewCitizen;
    if(this->currentProductionCost > 0)
    {
        this->accumulatedProduction += this->cityTotalYield->GetProductionYield();
    }

    if(this->accumulatedProduction >= this->currentProductionCost && (this->currentProductionName != "No Current Production"))
    {
        qDebug() << "----Production of" << this->getProductionName() << "finished";
        this->accumulatedProduction = 0;
        this->currentProductionName = "No Current Production";
        this->currentProductionCost = 0;
        update.productionFinished = true;
        this->productionFinished = true;

        if(!this->productionUnit)
            this->UpdateCityYield();
    }

    if(this->cityHealth < this->maxHealth)
    {
        qDebug() << "------Healing City";
        this->cityHealth += this->maxHealth * 0.15f;

        if(this->cityHealth > this->maxHealth)
            this->cityHealth = this->maxHealth;

        update.cityHealed = true;
    }

    this->cityStrength = this->baseStrength + this->citizens + this->buildingStrength;

    qDebug() << "       Total city Strength:" << this->cityStrength
             << "Base Strength:" << this->baseStrength
             << "citizens:" << this->citizens
             << "Building Strength" << this->buildingStrength;

    return update;
}

void City::SetCityFocus(City::Focus focus)
{
    qDebug() << "------City focus changed to" << focus;
    this->cityFocus = focus;
    this->SortTileQueue();
    this->SortControlledTiles();
    /// Need function to reassign worked tiles.
}

City::Focus City::GetCityFocus()
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

void City::FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, SortOrder sortOrder)
{
    int dstXUp, dstYUp, dstXLow, dstYLow, x, y, newX, newY;

//    qDebug() << "       upperX:" << upperX << "lowX:" << lowX << "upperY:" << upperY << "lowY:" << lowY;
//    qDebug() << "   --SortOrder:" << sortOrder;
    QVector<QPoint> tempPt;

    //Find any point that falls between the x and y bounds passed to the funtion.
    foreach(QPoint point, ptVect)
    {
        x = point.x();
        y = point.y();

        if((x >= lowX) && (x <= upperX))
        {
            if((y >= lowY) && (y <= upperY))
            {
                // if the point falls between the bounds,
                // place it in the temporary vector
                tempPt.push_back(point);
            }
        }
    }

    for(int j = 0; j < tempPt.size(); j++)
    {
        for(int k = j + 1; k < tempPt.size(); k++)
        {
            if(sortOrder == LRTB)
            {
                //Sort left to right, top to bottom
                if(tempPt[j].x() > tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
                    if(tempPt[j].y() > tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
                    }
                }

            }
            else if(sortOrder == RLTB)
            {
                //Sort right to left, top to bottom
                if(tempPt[j].x() < tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
                    if(tempPt[j].y() > tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
                    }
                }
            }
            else if(sortOrder == RLBT)
            {
                //Sort right to left, bottom to top
                if(tempPt[j].x() < tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
                    if(tempPt[j].y() < tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
                    }
                }
            }
            else if(sortOrder == LRBT)
            {
                //Sort left to right, bottom to top
                if(tempPt[j].x() > tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
                    if(tempPt[j].y() < tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
                    }
                }
            }
        }
    }

    // remove any duplicate points from the vector
    //      Note: these duplicates come from the fact that
    //      each polygon has points that overlap each other.
    for(int i = 0; i < tempPt.size() - 1; i++)
    {
        if(tempPt[i].x() == tempPt[i + 1].x())
        {
            if(tempPt[i].y() == tempPt[i + 1].y())
            {
//                qDebug() << "       --Removing point:" << tempPt[i +1];
                tempPt.removeAt(i + 1);
            }
        }

//        qDebug() << "       tempPt[" << i << "]" << tempPt[i];
    }
//    qDebug() << "       tempPt[" << tempPt.size() - 1 << "]" << tempPt.last();
    int lastX = 0, lastY = 0;

    // Load the points into the cityBorder vector
    foreach(QPoint point, tempPt)
    {
        x = point.x();
        y = point.y();

        dstXUp = upperX - x;
        dstYUp = upperY - y;
        dstXLow = x - lowX;
        dstYLow = y - lowY;

        // add some additional points in if the tiles are horizontally aligned. (dstY == 0)
        if(/*((abs(x - lastX) == 88) || (abs(x - lastX) == 176)) && */(abs(y - lastY) == 0))
        {
            if(abs(x - lastX) == 88)
            {
                newX = (x + lastX) / 2;

                if (lastX < x)
                {
                    newY = y + 24;
                }
                else if(lastX > x)
                {
                    newY = y - 24;
                }

                cityBorder.push_back(QPoint(newX, newY));
            }
            // This needs to be here because for some reason, if there are
            // more than three points in a horizontal line, the last two won't register
            // therefore a straight line is drawn. This block is supposed to fix that error.
            else if(abs(x - lastX) == 176)
            {
                newX = (x + lastX) / 2;
                newX -= 44;

                if (lastX < x)
                {
                    newY = y + 24;
                }
                else if(lastX > x)
                {
                    newY = y - 24;
                }

                cityBorder.push_back(QPoint(newX, newY));

                newX += 88;
                cityBorder.push_back(QPoint(newX, newY));
            }
        }

        //Store the x and y values for the next iteration.
        lastX = x;
        lastY = y;

        // Don't load the point if it is the same as one of the bounding points
        if(dstXLow == 0 && dstYUp == 0) { continue; }
        else if(dstXUp == 0 && dstYLow == 0) { continue; }
        else if(dstXLow == 0 && dstYLow == 0) { continue; }
        else if(dstXUp == 0 && dstYUp == 0) { continue; }

        // Load the point into the cityBorder vector.
        cityBorder.push_back(point);

    }


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
        qDebug() << "----City is no longer stagnant";
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

void City::SetCityBorders(QPolygon borders)
{
    foreach(QPoint pt, borders)
    {
        this->cityBorder.push_back(pt);
    }
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
    tile->SetControllingCiv(controllingCiv, this->cityTile->GetControllingCivListIndex());
    this->cityControlledTiles.push_back(tile);
}

void City::SetMaximumExpansionBorderTiles(QList<Tile *> tileVect)
{
    foreach(Tile* tile, tileVect)
    {
        this->maximumExpansionBorderTiles.push_back(tile);
    }

}

void City::DefineCityBorders(bool redefine)
{
    if(redefine)
    {
        cityBorder.clear();
    }

    //Get the center of each tile the city controls and
    // load the point into the QVector
    QVector<QPoint> points;
    int ptCount = 0;
    foreach(Tile* tile, cityControlledTiles)
    {
        for(int i = 0; i < 6; i++)
        {
            points.push_back(tile->GetHexPoint(i));
            ptCount++;
        }
    }

    //Initialize the number of points in the points QVector
    int numPts = points.size();

    //Initialize the output QVector
    QVector<QPoint> hull;

    //Find the leftmost point in the points QVector
    int l = 0;

    for(int i = 1; i < numPts; i++)
    {
        if(points[i].x() < points[l].x())
            l = i;
    }

    int p = l, q;

//    qDebug() << "------Leftmost point at:" << l << points[l];

    //This uses the Jarvis March / Gift-wrapping method for finding the convex hull
    // that encompasses all points in the set
//    qDebug() << "     Finding convex hull";

    do
    {
        hull.push_back(points[p]);

        q = (p + 1) % numPts;

        for(int i = 0; i < numPts; i++)
        {
            if(orientation(points[p], points[i], points[q]) == 2)
                q = i;
        }

        p = q;
    }
    while(p != l);

    if(hull.last().x() != hull.first().x())
        hull.push_back(points[l]);

    //Load the resulting convex hull into the cityBorder QPolygon
    int lastX, lastY, currentX, currentY, newX, newY, dstX, dstY;
    for(int i = 0; i < hull.size(); i++)
    {
        if(i > 1)
        {
            lastX = hull[i - 1].x();
            lastY = hull[i - 1].y();
            currentX = hull[i].x();
            currentY = hull[i].y();

            // Add a function or code block that checks the distance between two points
            // and if the distance is larger than normal (need to find out what this is)
            // then search points[] for any QPoint that is between the x pos and y pos
            // coordinates.

            dstX = currentX - lastX;
            dstY = currentY - lastY;

//            qDebug() << "   currentX:" << currentX << "lastX:" << lastX  << "dstX:" << dstX << "currentY:" << currentY << "lastY:" << lastY << "dstY:" << dstY;

            if(((abs(dstX) >= 88) || (abs(dstY) >= 50)))
            {
                if((currentX > lastX) && (currentY > lastY))
                {
                    FindPoints(lastX, lastY, currentX, currentY, points, LRTB);
                }
                else if((currentX < lastX) && (currentY > lastY))
                {
                    FindPoints(currentX, lastY, lastX, currentY, points, RLTB);
                }
                else if((currentX < lastX) && (currentY < lastY))
                {
                    FindPoints(currentX, currentY, lastX, lastY, points, RLBT);
                }
                else if((currentX > lastX) && (currentY < lastY))
                {
                    FindPoints(lastX, currentY, currentX, lastY, points, LRBT);
                }
                else if(dstY == 0)
                {
                    newX = (currentX + lastX) / 2;

                    if (lastX < currentX)
                    {
                        newY = currentY + 24;
                    }
                    else if(lastX > currentX)
                    {
                        newY = currentY - 24;
                    }

                    cityBorder.push_back(QPoint(newX, newY));
                }
                else if(dstX == 0)
                {
                    if(lastY > currentY)
                    {
//                        qDebug() << "     --Vertically aligned, left side";
                        FindPoints(lastX - 50, currentY, currentX, lastY, points, LRBT);
                    }
                    else if(lastY < currentY)
                    {
//                        qDebug() << "     --Vertically aligned, right side";
                        FindPoints(lastX - 50, lastY, currentX, currentY, points, RLTB);
                    }
                }
            }
        }

        this->cityBorder.push_back(hull[i]);
    }

//    qDebug() << "------------CityBorder points";
//    foreach(QPoint point, cityBorder)
//    {
//        qDebug() << "           " << point;
//    }

    if(!redefine)
    {
        this->turnsToBorderGrowth = floor((20 + (10*pow(this->cityControlledTiles.size() - 1, 1.1))) / this->cityTotalYield->GetCultureYield());
        this->foodSurplus = this->cityTotalYield->GetFoodYield() - (2 * this->citizens);
        this->growthCost = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));
        this->turnsToNewCitizen = this->growthCost / this->foodSurplus;
    }

//    qDebug() << "       Done";
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
        tile->SetGoverningCity(this);
    }

    this->initialized = true;
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
    qDebug() << "-----MSDINTERSECTS-----";
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

            if((rltbValid && lrbtValid) || (lrtbValid && rlbtValid))
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
//          qDebug()<<"Unit Name: "<<unitInfo[0];
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
//          qDebug()<<"TYPE: "<<type;
          tempUnit->SetUnitIcon(type);
          initialUnitList.push_back(tempUnit);


       }
       inputFile.close();

    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

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
//          qDebug()<<"Building Name: "<<buildingInfo[0];
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
//       qDebug()<<initialBuildingList.at(1)->getName();
//       qDebug()<<initialBuildingList.size();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}

void City::addBuilding(Building *building)
{
    this->producedBuildings.push_back(building);
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
}

Unit *City::GetGarrisonedMilitary()
{
    if(this->StationedMilitary != NULL)
    {
        return this->StationedMilitary;
    }
}

int City::GetCityIndex()
{
    return this->cityIndex;
}

int City::GetCityRenderIndex()
{
    return this->cityRenderIndex;
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
