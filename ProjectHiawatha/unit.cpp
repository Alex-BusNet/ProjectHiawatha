#include "unit.h"

Unit::Unit()
{
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
//    this->unitIcon = new QPixmap("../ProjectHiawatha/Assets/Icons/TestUnit.png");
}


Unit::Unit(Nation owner, bool isNonCombat, bool isSettler, int movementPoints, int strength, int range, int health, UnitType type)
{
    SetUnitIcon(type);

    this->belongsTo = owner;
    this->NonCombat = isNonCombat;
    this->Settler = isSettler;
    this->movementPoints = movementPoints;
    this->strength = strength;
    this->range = range;
    this->health = health;
    this->RequiresOrders = true;
    this->Updated = false;
    this->HasNoMovementLeft = false;
    this->isFortified = false;
    this->isMelee = true;
    this->needsPath = false;
    this->type = type;
    this->isRoadWorker=false;
    this->maxHealth = this->health;

    if(isNonCombat && !isSettler)
    {
        this->uses = 3;
    }
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
    this->maxHealth = this->health;
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
        this->Settler = false;
        this->movementPoints = 2;
        this->strength = 0;
        this->range = 3;
        this->health = 100;
        this->isRoadWorker = false;
        this->uses = 3;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/worker.png");
        break;
    case SETTLER:
        this->NonCombat = true;
        this->Settler = true;
        this->movementPoints = 2;
        this->strength = 0;
        this->range = 3;
        this->health = 100;
        this->isRoadWorker = false;
        this->uses = 1;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/settler.png");
        break;
    case WARRIOR:
        this->NonCombat = false;
        this->Settler = false;
        this->movementPoints = 2;
        this->strength = 5;
        this->range = 3;
        this->health = 100;
        this->isRoadWorker = false;
        this->uses = 3;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/warrior.png");
        break;
    case ARCHER:
        this->NonCombat = false;
        this->Settler = false;
        this->movementPoints = 3;
        this->strength = 5;
        this->range = 3;
        this->rangeStrength = 10;
        this->health = 100;
        this->isRoadWorker = false;
        this->uses = -1;
        this->isMelee = false;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/archer.png");
        break;
    case SPEARMAN:
        this->NonCombat = false;
        this->Settler = false;
        this->movementPoints = 4;
        this->strength = 10;
        this->range = 3;
        this->health = 100;
        this->isRoadWorker = false;
        this->uses = -1;
        this->isMelee = true;
        this->unitIcon = new QImage("../ProjectHiawatha/Assets/Units/spearman.png");
        break;
    case PIKEMAN:
        break;
    case HORSEMAN:
        break;
    case SCOUT:
        break;
    case HORSE_ARCHER:
        break;
    case CATAPULT:
        break;
    case TREBUCHET:
        break;
    case TRIREME:
        break;
    case GALLEY:
        break;
    case KNIGHT:
        break;
    case CROSSBOWMAN:
        break;
    case LANDSNACK:
        break;
    case LONGSWORDSMAN:
        break;
    case MUSKETMAN:
        break;
    case RIFLEMAN:
        break;
    case CARAVEL:
        break;
    case CANNON:
        break;
    case LANCER:
        break;
    case CAVALRY:
        break;
    case INFANTRY:
        break;
    case FRIGATE:
        break;
    case IRONCLAD:
        break;
    case ARTILLERY:
        break;
    case TANK:
        break;
    case PARATROOPER:
        break;
    case ANTI_TANK_GUN:
        break;
    case AA:
        break;
    case BATTLESHIP:
        break;
    case SUBMARINE:
        break;
    case NUKE:
        break;
    case MOBILE_SAM:
        break;
    case MODERN_INFANTRY:
        break;
    case MODERN_ARMOR:
        break;
    case FIGHTER:
        break;
    case BOMBER:
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

int Unit::GetUnitListIndex()
{
    return this->unitListIndex;
}

QList<Tile *> Unit::GetPath()
{
    return this->path;
}

UnitType Unit::GetUnitType()
{
    return this->type;
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


void Unit::SetPath(QList<Tile *> newPath)
{
    foreach(Tile* tile, newPath)
    {
        this->path.push_back(tile);
    }
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

void Unit::UpdatePath()
{
    this->path.removeFirst();
}
