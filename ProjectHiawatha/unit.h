#ifndef UNIT_H
#define UNIT_H

#include <QPixmap>
#include "tile.h"
#include "nation.h"

class Tile;

class Unit
{
public:
    Unit();
    Unit(Nation owner = India, bool isNonCombat = false, bool isSettler = false, int movementPoints = 2, int strength = 1, int range = 3, int health = 5);
    ~Unit();

    void SetUnitIcon(QPixmap *icon);
    void SetOwner(Nation owner);
    void SetPosition(int column, int row);
//    void SetPosition(TileID pos);
    void SetMovementPoints(int pts);
    void SetStrength(int strength);
    void SetRange(int range);
    void SetRangeStrength(int rangeStr);
    void SetHealth(int health);
    void SetMaxHealth(int maxHealth);

    QPixmap* GetUnitIcon();
    Nation GetOwner();
//    TileID GetPosition();
    int GetMovementPoints();
    int GetStrength();
    int GetRange();
    int GetRangeStrength();
    int GetHealth();
    int GetMaxHealth();

    bool isNonCombat();
    bool isSettler();
    bool NeedsOrders();
private:
    QPixmap *unitIcon;

    Nation belongsTo;
    int colPosition;
    int rowPosition;

    bool NonCombat;
    bool Settler;
    bool requiresOrders;

    int movementPoints;
    int strength;
    int range;
    int rangeStrength;
    int health;
    int maxHealth;
};

#endif // UNIT_H
