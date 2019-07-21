#include "unit.h"
#include <QDebug>
#include <math.h>

Unit::Unit()
{
    this->belongsTo = NO_NATION;
    this->NonCombat = false;
    this->Settler = false;
    this->naval = false;
    this->movementPoints = 2;
    this->strength = 1;
    this->range = 3;
    this->health = 100;
    this->maxHealth = 100;
    this->RequiresOrders = true;
    this->Updated = false;
    this->isFortified = false;
    this->isMelee = false;
    this->uses = 1;
    this->needsPath = false;
    this->unlocked = 0;
    this->isGarrisoned=false;
    this->pixmapIndex = -1;
    //this->dataMapIndex = -1;
    this->unitTile = NULL;
    this->healthBarIndex = -1;
    this->unitListIndex = -1;

}



Unit::Unit(Nation owner, UnitType type, int civIdx)
{
    SetUnitIcon(type);

    this->civIndex = civIdx;
    this->belongsTo = owner;
    this->RequiresOrders = true;
    this->Updated = false;
    this->HasNoMovementLeft = false;
    this->isFortified = false;
    this->needsPath = false;
    this->type = type;
    this->health = 100;
    this->maxHealth = this->health;
    this->unlocked = 0;
    this->range = 1;
    this->uses = 1;
    this->isGarrisoned=false;
    this->pixmapIndex = -1;
    this->unitTile = NULL;
    this->healthBarIndex = -1;
    this->unitListIndex = -1;
}

Unit::Unit(int index)
{
    index = 0;
    this->belongsTo = India;
    this->NonCombat = false;
    this->Settler = false;
    this->movementPoints = 2;
    this->strength = 1;
    this->range = 3;
    this->health = 100;
    this->maxHealth = 100;
    this->RequiresOrders = true;
    this->Updated = false;
    this->isFortified = false;
    this->isMelee = false;
    this->uses = 1;
    this->needsPath = false;
    this->unlocked = 0;
    this->isGarrisoned=false;
    this->pixmapIndex = -1;
    this->unitTile = NULL;
    this->healthBarIndex = -1;
    this->unitListIndex = -1;
}

Unit::~Unit()
{

}

void Unit::setUnitType(UnitType type)
{
   this->type = type;
}

void Unit::SetUnitIcon(UnitType type)
{
    switch(type)
    {
    case WORKER:
        this->NonCombat = true;
        this->naval = false;
        this->strength = 1;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/worker.png");
        this->name = "Worker";
        break;
    case SETTLER:
        this->NonCombat = true;
        this->naval = false;
        this->strength = 1;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/settler.png");
        this->name = "Settler";
        break;
    case WARRIOR:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/warrior.png");
        this->name = "Warrior";
        break;
    case ARCHER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/archer.png");
        this->name = "Archer";
        break;
    case SPEARMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/spearman.png");
        this->name = "Spearman";
        break;
    case PIKEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/pikeman.png");
        this->name = "Pikeman";
        break;
    case HORSEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/horseman.png");
        this->name = "Horseman";
        break;
    case SCOUT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/scout.png");
        this->name = "Scout";
        break;
    case HORSE_ARCHER:
        break;
    case CATAPULT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = true;
        this->unitIcon = new QImage("Assets/Units/catapult.png");
        this->name = "Catapult";
        break;
    case TREBUCHET:
        break;
    case TRIREME:
        break;
    case GALLEY:
        this->NonCombat = false;
        this->naval = true;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/galley.png");
        this->name = "Galley";
        break;
    case KNIGHT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/knight.png");
        this->name = "Knight";
        break;
    case CROSSBOWMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/crossbowman.png");
        this->name = "Crossbowman";
        break;
    case LANDSNACK:
        break;
    case LONGSWORDSMAN:
        break;
    case SWORDSMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/swordsman.png");
        this->name = "Swordsman";
        break;
    case MUSKETMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/musketman.png");
        this->name = "Musketman";
        break;
    case RIFLEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/rifleman.png");
        this->name = "Rifleman";
        break;
    case CARAVEL:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/caravel.png");
        this->name = "Caravel";
        break;
    case CANNON:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = true;
        this->unitIcon = new QImage("Assets/Units/cannon.png");
        this->name = "Cannon";
        break;
    case LANCER:
        break;
    case CAVALRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/cavalry.png");
        this->name = "Cavalary";
        break;
    case INFANTRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/infantry.png");
        this->name = "Infantry";
        break;
    case FRIGATE:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/frigate.png");
        this->name = "Frigate";
        break;
    case IRONCLAD:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/ironclad.png");
        this->name = "Ironclad";
        break;
    case ARTILLERY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = true;
        this->unitIcon = new QImage("Assets/Units/artillery.png");
        this->name = "Artillery";
        break;
    case TANK:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/tank.png");
        this->name = "Tank";
        break;
    case PARATROOPER:
        break;
    case ANTI_TANK_GUN:
        break;
    case AA:
        break;
    case BATTLESHIP:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/battleship.png");
        this->name = "Battleship";
        break;
    case CARRIER:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/carrier.png");
        this->name = "Aircraft Carrier";
        break;
    case SUBMARINE:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/submarine.png");
        this->name = "Submarine";
        break;
    case NUKE:
        break;
    case MOBILE_SAM:
        break;
    case HELICOPTER_GUNSHIP:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/Helicopter Gunship.png");
        this->name = "Helicopter Gunship";
        break;
    case MECHANIZED_INFANTRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/Mechanized Infantry.png");
        this->name = "Mechanized Infantry";
        break;
    case MODERN_ARMOR:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/Modern Armor.png");
        this->name = "Modern Armor";
        break;
    case FIGHTER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/fighter.png");
        this->name = "Fighter Plane";
        break;
    case BOMBER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = true;
        this->unitIcon = new QImage("Assets/Units/bomber.png");
        this->name = "Bomber Plane";
        break;
    case JET_FIGHTER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = false;
        this->unitIcon = new QImage("Assets/Units/Jet Fighter.png");
        this->name = "Fighter Jet";
        break;
    case STEALTH_BOMBER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->isSiege = true;
        this->unitIcon = new QImage("Assets/Units/Stealth Bomber.png");
        this->name = "Stealth Bomber";
        break;
    case GDR:
        break;
    case GANDHI_DR:
        break;
    default:
        break;
    }

}

void Unit::SetOwner(Nation owner, int idx)
{
    this->belongsTo = owner;
    this->civIndex = idx;
}

void Unit::SetName(QString str)
{
    this->name = str;
}

void Unit::setUnlocked(int unlock)
{
    this->unlocked = unlock;
}

/*void Unit::SetPositionIndex(int tileMapIndex)
{
    this->dataMapIndex = tileMapIndex;
}
*/

void Unit::SetPosition(Tile* tile)
{
    this->unitTile = tile;
    this->colPosition = tile->GetTileID().column;
    this->rowPosition = tile->GetTileID().row;
}

/*
void Unit::SetPosition(int column, int row)
{
    this->colPosition = column;
    this->rowPosition = row;
}
*/

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

QImage *Unit::GetUnitIcon()
{
    return this->unitIcon;
}

Nation Unit::GetOwner()
{
    return this->belongsTo;
}

void Unit::WriteUnitSaveData(QJsonObject &obj) const
{
    obj["name"] = name;
    obj["type"] = type;
    obj["str"] = strength;
    obj["hp"] = health;
    obj["maxhp"] = maxHealth;
    obj["range"] = range;
    obj["rangestr"] = rangeStrength;
    obj["belongsto"] = belongsTo;
    obj["movepoints"] = movementPoints;
    obj["datamaploc"] = unitTile->GetTileIndex(); //dataMapIndex;
    obj["fortified"] = isFortified;
    obj["healthbarindex"] = healthBarIndex;
    obj["pixmapindex"] = pixmapIndex;
    obj["unitlistindex"] = unitListIndex;
    obj["noncombat"] = NonCombat;
    obj["settler"] = Settler;
    obj["naval"] = naval;
    obj["uses"] = uses;
    obj["cost"] = cost;
    obj["unlocked"] = unlocked;
    obj["techindex"] = techIndex;
    obj["fortified"] = false;
    obj["melee"] = isMelee;
    obj["siege"] = isSiege;
    obj["cannuke"] = false; //For GDR, Gandhi-DR, and Nukes. When added;
}

void Unit::ReadUnitSaveData(const QJsonObject &obj)
{
    name = obj["name"].toString();
    type = static_cast<UnitType>(obj["type"].toInt());
    strength = obj["str"].toInt();
    health = obj["hp"].toInt();
    maxHealth = obj["maxhp"].toInt();
    range = obj["range"].toInt();
    rangeStrength = obj["rangestr"].toInt();
    belongsTo = static_cast<Nation>(obj["belongsto"].toInt());
    movementPoints = obj["movepoints"].toInt();
    /// TODO: Figure out how to save the tile the unit is on
    //dataMapIndex = obj["datamaploc"].toInt();
    isFortified = obj["fortified"].toBool();
    healthBarIndex = obj["healthBarIndex"].toInt();
    pixmapIndex = obj["pixmapindex"].toInt();
    unitListIndex = obj["unitlistindex"].toInt();
    NonCombat = obj["noncombat"].toBool();
    Settler = obj["settler"].toBool();
    naval = obj["naval"].toBool();
    uses = obj["uses"].toInt();
    cost = obj["cost"].toInt();
    unlocked = obj["unlocked"].toInt();
    techIndex = obj["techindex"].toInt();
    isFortified = obj["fortified"].toBool();
    isMelee = obj["melee"].toBool();
    isSiege = obj["siege"].toBool();

    SetUnitIcon(type);
}

int Unit::GetTileIndex()
{
//    return this->dataMapIndex;
    return this->unitTile->GetTileIndex();
}

int Unit::GetTargetTileIndex()
{
    return this->targetTileIndex;
}

int Unit::GetTileColumn()
{
    return this->colPosition;
}

int Unit::GetTileRow()
{
    return this->rowPosition;
}

int Unit::GetTargetTileColumn()
{
    return this->targetColPos;
}

int Unit::GetTargetTileRow()
{
    return this->targetRowPos;
}

int Unit::GetHealthBarIndex()
{
    return this->healthBarIndex;
}

int Unit::GetCost()
{
    return this->cost;
}

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

int Unit::GetPixmapIndex()
{
    return this->pixmapIndex;
}

int Unit::GetRemainingUses()
{
    return this->uses;
}

int Unit::GetTechIndex()
{
    return this->techIndex;
}

int Unit::GetUnitListIndex()
{
    return this->unitListIndex;
}

int Unit::GetUnitPower()
{
    if(this->isMelee)
    {
        return ceil(pow(strength, 1.5) * pow(movementPoints, 0.3));
    }
    else
    {
        return ceil(pow(rangeStrength, 1.45) * pow(movementPoints, 0.3));
    }
}

QString Unit::GetName()
{
    return this->name;
}

QList<Tile *> Unit::GetPath()
{
    return this->path;
}

Tile *Unit::GetNextTileInPath()
{
    return this->path.first();
}

Tile *Unit::GetUnitTile()
{
    return this->unitTile;
}

UnitType Unit::GetUnitType()
{
    return this->type;
}

int Unit::isUnlocked()
{
    return this->unlocked;
}

bool Unit::isPathEmpty()
{
    return path.isEmpty();
}

bool Unit::isNonCombat()
{
    return this->NonCombat;
}

bool Unit::isSettler()
{
    return this->isSettler();
}

bool Unit::isNaval()
{
    return this->naval;
}


void Unit::SetPath(QList<Tile *> newPath)
{
    foreach(Tile* tile, newPath)
    {
        this->path.push_back(tile);
    }
}

void Unit::SetTechIndex(int index)
{
    this->techIndex = index;
}

void Unit::SetPixmapIndex(int index)
{
    this->pixmapIndex = index;
}

void Unit::SetUnitListIndex(int index)
{
    this->unitListIndex = index;
}

void Unit::SetUnitTargetTile(int column, int row)
{
    this->targetColPos = column;
    this->targetRowPos = row;
}

void Unit::SetHealthBarIndex(int index)
{
    this->healthBarIndex = index;
}

void Unit::SetCost(int price)
{
    this->cost = price;
}

void Unit::SetUnitImage(QImage *image)
{
    this->unitIcon = image;
}

void Unit::SetUnitTargetTileIndex(int tileIndex)
{
    this->targetTileIndex = tileIndex;
}

void Unit::Use()
{
    this->uses--;
}

void Unit::DealDamage(int damage)
{
    this->health -= damage;
}

void Unit::UpdatePath()
{
    if(this->GetNextTileInPath()->ContainsUnit())
    {
        // If the unit in the next tile is not moving or the unit in
        // our way is trying to move onto the tile we occupy, then
        // clear the path otherwise wait for the unit to move.
        if(this->GetNextTileInPath()->GetOccupyingUnit()->RequiresOrders
                || this->GetNextTileInPath()->GetOccupyingUnit()->GetNextTileInPath()->GetTileIndex() == this->unitTile->GetTileIndex())
        {
            this->ClearPath();
            this->RequiresOrders = true;
            return;
        }
    }

//    this->unitTile->ContainsUnit = false;
    this->unitTile->SetOccupyingUnit(NULL);

    if(this->unitTile->Selected)
        this->unitTile->Selected = false;

    this->unitTile = this->GetNextTileInPath();
//    this->unitTile->ContainsUnit = true;
    this->unitTile->SetOccupyingUnit(this);

    this->path.removeFirst();
}

void Unit::ClearPath()
{
    while(!this->path.isEmpty())
    {
        this->path.removeFirst();
    }
}

int Unit::GetOwningCivIndex()
{
        return this->civIndex;
}
