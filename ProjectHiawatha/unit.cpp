#include "unit.h"

Unit::Unit()
{
    this->belongsTo = India;
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
//    this->unitIcon = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
}



Unit::Unit(Nation owner, UnitType type)
{
    SetUnitIcon(type);

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
}

Unit::~Unit()
{

}

void Unit::SetUnitIcon(UnitType type)
{
    //// Most, if not all, of the unit specific data will need to be changed
    ///   (Paul's job)
    ///
    switch(type)
    {
    case WORKER:
        this->NonCombat = true;
        this->naval = false;
        this->strength = 1;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/worker.png");
        break;
    case SETTLER:
        this->NonCombat = true;
        this->naval = false;
        this->strength = 1;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/settler.png");
        break;
    case WARRIOR:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/warrior.png");
        break;
    case ARCHER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/archer.png");
        break;
    case SPEARMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/spearman.png");
        break;
    case PIKEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/pikeman.png");
        break;
    case HORSEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/horseman.png");
        break;
    case SCOUT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/scout.png");
        break;
    case HORSE_ARCHER:
        break;
    case CATAPULT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/catapult.png");
        break;
    case TREBUCHET:
        break;
    case TRIREME:
        break;
    case GALLEY:
        this->NonCombat = false;
        this->naval = true;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/galley.png");
        break;
    case KNIGHT:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/knight.png");
        break;
    case CROSSBOWMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/crossbowman.png");
        break;
    case LANDSNACK:
        break;
    case LONGSWORDSMAN:
        break;
    case SWORDSMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/swordsman.png");
        break;
    case MUSKETMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/musketman.png");
        break;
    case RIFLEMAN:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/rifleman.png");
        break;
    case CARAVEL:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/caravel.png");
        break;
    case CANNON:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/cannon.png");
        break;
    case LANCER:
        break;
    case CAVALRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/cavalry.png");
        break;
    case INFANTRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/infantry.png");
        break;
    case FRIGATE:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/frigate.png");
        break;
    case IRONCLAD:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/ironclad.png");
        break;
    case ARTILLERY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/artillery.png");
        break;
    case TANK:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/tank.png");
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
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/battleship.png");
        break;
    case CARRIER:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/carrier.png");
        break;
    case SUBMARINE:
        this->naval = true;
        this->NonCombat = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/submarine.png");
        break;
    case NUKE:
        break;
    case MOBILE_SAM:
        break;
    case HELICOPTER_GUNSHIP:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/helicopter_gunship.png");
        break;
    case MECHANIZED_INFANTRY:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/mechanized_infantry.png");
        break;
    case MODERN_ARMOR:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/modern_armor.png");
        break;
    case FIGHTER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/fighter.png");
        break;
    case BOMBER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/bomber.png");
        break;
    case JET_FIGHTER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/jet_fighter.png");
        break;
    case STEALTH_BOMBER:
        this->NonCombat = false;
        this->naval = false;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/stealth_bomber.png");
        break;
    case GDR:
        break;
    case GANDHI_DR:
        break;
    default:
        break;
    }

}

void Unit::SetOwner(Nation owner)
{
    this->belongsTo = owner;
}

void Unit::SetName(QString str)
{
    this->name = str;
}

void Unit::setUnlocked(int unlock)
{
    this->unlocked = unlock;
}

void Unit::SetPositionIndex(int tileMapIndex)
{
    this->dataMapIndex = tileMapIndex;
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

QImage *Unit::GetUnitIcon()
{
    return this->unitIcon;
}

Nation Unit::GetOwner()
{
    return this->belongsTo;
}

int Unit::GetTileIndex()
{
    return this->dataMapIndex;
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
    this->path.removeFirst();
}
