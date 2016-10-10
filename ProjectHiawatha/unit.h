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
//    void SetPosition(int column, int row);
    void SetPosition(Tile* tile);
    void SetMovementPoints(int pts);
    void SetStrength(int strength);
    void SetRange(int range);
    void SetRangeStrength(int rangeStr);
    void SetHealth(int health);
    void SetMaxHealth(int maxHealth);
    void SetPath(QList<Tile*> path);
    void SetPixmapIndex(int index);

    QPixmap* GetUnitIcon();
    Nation GetOwner();
    Tile* GetPosition();
    int GetMovementPoints();
    int GetStrength();
    int GetRange();
    int GetRangeStrength();
    int GetHealth();
    int GetMaxHealth();
    int GetPixmapIndex();

    QList<Tile*> GetPath();

    bool isPathEmpty();
    bool isNonCombat();
    bool isSettler();
//    bool NeedsOrders();
    bool RequiresOrders;
    bool Updated;


private:
    QPixmap *unitIcon;

    Nation belongsTo;
    Tile *position;
    int colPosition;
    int rowPosition;

    int pixmapIndex;
    bool NonCombat;
    bool Settler;

    int movementPoints;
    int strength;
    int range;
    int rangeStrength;
    int health;
    int maxHealth;

    QList<Tile*> path;
};

#endif // UNIT_H
