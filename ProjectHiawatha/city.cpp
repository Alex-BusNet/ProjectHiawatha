#include "city.h"
#include <QDebug>

City::City()
{
    this->cityTotalYield = new Yield(1,1,1,1,1);
}


City::~City()
{

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
    int newGold = 0, newProd = 0, newSci = 0, newFood = 0, newCul = 0;

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
