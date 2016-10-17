#include "city.h"
#include <QDebug>

City::City()
{
    this->cityTotalYield = new Yield(1,1,1,1,1);
}


City::~City()
{

}

void City::TileToGetNext()
{
    // Select the next tile to be claimed by a city based on:
    //  -Is the tile already claimed by another civ?
    //  -The total yield of the tile
    //  -Resources on tile
    //  -Tile type

    //Store all eligible tiles in a heap.
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
        newGold += tile->GetYield()->GetGoldYield();
        newProd += tile->GetYield()->GetProductionYield();
        newSci += tile->GetYield()->GetScienceYield();
        newFood += tile->GetYield()->GetFoodYield();
        newCul += tile->GetYield()->GetCultureYield();

    }

    this->cityTotalYield->ChangeYield(newGold, newProd, newSci, newFood, newCul);
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

void City::DefineCityBorders()
{

    //Get the center of each tile the city controls and
    // load the point into the QVector
    QVector<QPoint> points;
    qDebug() << "     Getting centers";
    foreach(Tile* tile, cityControlledTiles)
    {
        for(int i = 0; i < 6; i++)
            points.push_back(tile->GetHexPoint(i));
    }

    //Initialize the number of points in the points QVector
    int numPts = points.size();
    //Initialize the output QVector
    QVector<QPoint> hull;

    //Find the leftmost point in the points QVector
    int l = 0;
    qDebug() << "     Finding leftmost point";
    for(int i = 1; i < numPts; i++)
    {
        if(points[i].x() < points[l].x())
            l = i;
    }

    int p = l, q;

    //This uses the Jarvis March / Gift-wrapping method for finding the convex hull
    // that encompasses all points in the set
    qDebug() << "     Finding convex hull";

    do
    {
        qDebug() << "p" << p << ", points[p]" << points[p];
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

    qDebug() << "     Loading borders";
    //Load the resulting convex hull into the cityBorder QPolygon
    for(int i = 0; i < hull.size(); i++)
    {
        this->cityBorder.push_back(hull[i]);
        qDebug() << "Border point:" << hull[i];
    }

    qDebug() << "     Done";
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
