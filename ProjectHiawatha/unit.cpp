#include "unit.h"

Unit::Unit()
{
    this->belongsTo = India;
    this->NonCombat = false;
    this->Settler = false;
    this->movementPoints = 2;
    this->strength = 1;
    this->range = 3;
    this->health = 5;
    this->requiresOrders = true;

    this->unitIcon = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
}


Unit::Unit(Nation owner, bool isNonCombat, bool isSettler, int movementPoints, int strength, int range, int health)
{
    this->belongsTo = owner;
    this->NonCombat = isNonCombat;
    this->Settler = isSettler;
    this->movementPoints = movementPoints;
    this->strength = strength;
    this->range = range;
    this->health = health;
    this->requiresOrders = true;

    this->unitIcon = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
}

Unit::~Unit()
{

}

void Unit::SetUnitIcon(QPixmap *icon)
{
    this->unitIcon = icon;
}

void Unit::SetOwner(Nation owner)
{
    this->belongsTo = owner;
}

void Unit::SetPosition(int column, int row)
{
    this->colPosition = column;
    this->rowPosition = row;
}

void Unit::SetMovementPoints(int pts)
{
    this->movementPoints = pts;
}

void Unit::SetStrength(int strength)
{
    this->strength = strength;
}

void Unit::SetRange(int range)
{
    this->range = range;
}

void Unit::SetRangeStrength(int rangeStr)
{
    this->rangeStrength = rangeStr;
}

void Unit::SetHealth(int health)
{
    this->health = health;
}

void Unit::SetMaxHealth(int maxHealth)
{
    this->maxHealth = maxHealth;
}

QPixmap *Unit::GetUnitIcon()
{
    return this->unitIcon;
}

Nation Unit::GetOwner()
{
    return this->belongsTo;
}

//TileID Unit::GetPosition()
//{
//    return this->position;
//}

int Unit::GetMovementPoints()
{
    return this->movementPoints;
}

int Unit::GetStrength()
{
    return this->strength;
}

int Unit::GetRange()
{
    return this->range;
}

int Unit::GetRangeStrength()
{
    return this->rangeStrength;
}

int Unit::GetHealth()
{
    return this->health;
}

int Unit::GetMaxHealth()
{
    return this->maxHealth;
}

bool Unit::isNonCombat()
{
    return this->NonCombat;
}

bool Unit::isSettler()
{
    return this->isSettler();
}

bool Unit::NeedsOrders()
{
    return this->requiresOrders;
}
