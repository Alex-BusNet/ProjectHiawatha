#include "city.h"
#include <QDebug>

City::City()
{
    this->cityTotalYield = new Yield(1,1,1,1,1);
    this->turnsToBorderGrowth = 0;
    this->accumulatedProduction = 0;
    this->productionFinished = false;
    this->productionUnit = false;
    this->currentProductionName = "No Current Production";
}


City::~City()
{

}

void City::SortTileQueue()
{
    // Select the next tile to be claimed by a city based on:
    //  -Is the tile already claimed by another civ?
    //  -The total yield of the tile
    //  -Resources on tile
    //  -Tile type

    qDebug() << "   ---Sorting TileQueue";

    for(int i = 0; i < tileQueue.size(); i++)
    {
        for(int j = i + 1; j < tileQueue.size(); j++)
        {
            if(tileQueue[j]->GetYield()->GetGoldYield() > tileQueue[i]->GetYield()->GetGoldYield())
            {
                Tile* temp = tileQueue[i];
                tileQueue[i] = tileQueue[j];
                tileQueue[j] = temp;
            }
        }
    }

//    foreach(Tile* tile, tileQueue)
//    {
//        qDebug() << "           tileQueue" << tile->GetTileIDString() << tile->GetYield()->GetGoldYield();
//    }


    //Store all eligible tiles in a heap.
}

Update_t City::UpdateProgress()
{
    qDebug() << "       " << this->name << " turns to growth:" << turnsToBorderGrowth;
    Update_t update{false, false};
    if(turnsToBorderGrowth == 0)
    {
        this->AddControlledTile(this->tileQueue.first());
        this->tileQueue.removeFirst();
        this->DefineCityBorders(true);
        this->turnsToBorderGrowth = floor((20 + (pow(10*(this->cityControlledTiles.size() - 1), 1.1))) / this->cityTotalYield->GetCultureYield());
        update.updateBorders = true;
    }
    else
    {
        this->turnsToBorderGrowth--;
    }

    qDebug() << "       " << this->name << " turns to new citizen" << turnsToNewCitizen;
    if(turnsToNewCitizen == 0)
    {
        this->citizens++;
        this->turnsToNewCitizen = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));

        if(this->citizens < this->GetControlledTiles().size())
            this->GetControlledTiles().at(citizens - 1)->IsWorked = true;

        this->UpdateCityYield();
        update.updateCitizens = true;
    }
    else
    {
        this->turnsToNewCitizen--;
    }

    return update;
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

void City::FindPoints(int lowX, int lowY, int upperX, int upperY, QVector<QPoint> ptVect, bool reverseSort)
{
    int dstXUp, dstYUp, dstXLow, dstYLow, x, y, newX, newY;

//    qDebug() << "   upperX:" << upperX << "lowX:" << lowX << "upperY:" << upperY << "lowY:" << lowY;
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

    // sort the vector from smallest x-value to largest x-value if reverseSort is false,
    // else sort the vector from largest x-value to smallest x-value
//    qDebug() << "           reverseSort:" << reverseSort;
    for(int j = 0; j < tempPt.size(); j++)
    {
        for(int k = j + 1; k < tempPt.size(); k++)
        {
//            qDebug() << "       --tempPt[" << j << "]" << tempPt[j] << "tempPt[" << k << "]" << tempPt[k];
            if(!reverseSort)
            {
                if(tempPt[j].x() > tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
//                    qDebug() << "           x greater";
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
//                    qDebug() << "           x equal";
                    if(tempPt[j].y() > tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
//                        qDebug() << "           y greater";
                    }
                    else if(tempPt[j].y() < tempPt[k].y())
                    {
//                        qDebug() << "       y less than";
                    }
                }

            }
            else
            {
                if(tempPt[j].x() < tempPt[k].x())
                {
                    QPoint temp = tempPt[j];
                    tempPt[j] = tempPt[k];
                    tempPt[k] = temp;
//                    qDebug() << "           x less than";
                }
                else if(tempPt[j].x() == tempPt[k].x())
                {
//                    qDebug() << "           x equal";
                    if(tempPt[j].y() < tempPt[k].y())
                    {
                        QPoint temp = tempPt[j];
                        tempPt[j] = tempPt[k];
                        tempPt[k] = temp;
//                        qDebug() << "           y less";
                    }
                    else if(tempPt[j].y() > tempPt[k].y())
                    {
//                        qDebug() << "       y greater than";
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
        if(((abs(x - lastX) == 88) || (abs(x - lastX) == 176)) && (abs(y - lastY) == 0))
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
            else if(abs(x- lastX) == 176)
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
void City::SetCityAsCaptial()
{
    this->isCaptial = true;
}

void City::SetName(QString name)
{
    this->name = name;
}

void City::SetCityTile(Tile *tile)
{
    this->cityTile = tile;
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
    qDebug() << "   City controls" << cityControlledTiles.size() << "tile(s)";
    int newGold = cityTile->GetYield()->GetGoldYield(),
            newProd = cityTile->GetYield()->GetProductionYield(),
            newSci = cityTile->GetYield()->GetScienceYield(),
            newFood = cityTile->GetYield()->GetFoodYield(),
            newCul = cityTile->GetYield()->GetCultureYield();

    foreach(Tile *tile, cityControlledTiles)
    {
        qDebug() << "   Tile at:" << tile->GetTileIDString() << "isWorked:" << tile->IsWorked;

        if(tile->IsWorked)
        {
            newGold += tile->GetYield()->GetGoldYield();
            newProd += tile->GetYield()->GetProductionYield();
            newSci += tile->GetYield()->GetScienceYield();
            newFood += tile->GetYield()->GetFoodYield();
            newCul += tile->GetYield()->GetCultureYield();
        }
    }

    this->cityTotalYield->ChangeYield(newGold, newProd, newSci, newFood, newCul);
}

void City::SetCityHealthBarIndex(int index)
{
    this->cityHealthBarIndex = index;
}

void City::GarrisonWorker(Unit *worker)
{
    if(worker->isNonCombat())
    {
        this->StationedWorkers.push_back(worker);
        this->hasWorker = true;
    }
}

void City::GarrisonMilitary(Unit *military)
{
    if(!military->isNonCombat())
    {
        this->StationedMilitary.push_back(military);
        this->hasGarrison = true;
    }
}

void City::AddControlledTile(Tile *tile)
{
    this->cityControlledTiles.push_back(tile);
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
    qDebug() << "     Finding convex hull";

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

//            qDebug() << "   dstX:" << dstX << "dstY:" << dstY;

            if(((abs(dstX) >= 88) || (abs(dstY) >= 50)))
            {
                if((currentX > lastX) && (currentY > lastY))
                {
                    FindPoints(lastX, lastY, currentX, currentY, points, false);
                }
                else if((currentX < lastX) && (currentY > lastY))
                {
                    FindPoints(currentX, lastY, lastX, currentY, points, true);
                }
                else if((currentX < lastX) && (currentY < lastY))
                {
                    FindPoints(currentX, currentY, lastX, lastY, points, true);
                }
                else if((currentX > lastX) && (currentY < lastY))
                {
                    FindPoints(lastX, currentY, currentX, lastY, points, false);
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
//                        qDebug() << "     Vertically aligned, left side";
                        FindPoints(lastX - 50, currentY, currentX, lastY, points, true);
                    }
                    else if(lastY < currentY)
                    {
//                        qDebug() << "     Vertically aligned, right side";
                        FindPoints(lastX - 50, lastY, currentX, currentY, points, false);
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
        this->turnsToNewCitizen = floor(15 + 6*(this->citizens - 1) + pow(this->citizens - 1, 1.8));
    }

    qDebug() << "       Done";
}

void City::SetCityBordersIndex(int index)
{
    this->cityBordersIndex = index;
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

void City::setCapitolConnection(bool flag)
{
    this->hasCapitolConnection=flag;
}

bool City::getCapitolConnection()
{
    return this->hasCapitolConnection;
}

void City::SetCitizenCount(int count)
{
    this->citizens = count;
}

int City::GetCitizenCount()
{
    return this->citizens;
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

void City::setProductionFisished(bool isFinished)
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

QString City::getProductionName()
{
    return this->currentProductionName;
}

bool City::getHasWorker()
{
    return this->hasWorker;
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

QPolygon City::GetCityBorders()
{
    return this->cityBorder;
}

int City::GetCityIndex()
{
    return this->cityIndex;
}

int City::GetCityHealthBarIndex()
{
    return this->cityHealthBarIndex;
}

int City::GetCityBordersIndex()
{
    return this->cityBordersIndex;
}
