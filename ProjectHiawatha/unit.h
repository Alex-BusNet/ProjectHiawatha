#ifndef UNIT_H
#define UNIT_H

#include <QPixmap>
#include <QList>
#include "nation.h"
#include "tile.h"
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
    void SetPosition(int tileMapIndex);
    void SetMovementPoints(int pts);
    void SetStrength(int strength);
    void SetRange(int range);
    void SetRangeStrength(int rangeStr);
    void SetHealth(int health);
    void SetMaxHealth(int maxHealth);
    void SetPath(QList<Tile*> newPath);
    void SetPixmapIndex(int index);

    QPixmap* GetUnitIcon();
    Nation GetOwner();
    int GetTileIndex();
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
    bool HasNoMovementLeft;


private:
    QPixmap *unitIcon;

    Nation belongsTo;
//    Tile *position;
//    int colPosition;
//    int rowPosition;

    int pixmapIndex, dataMapIndex;
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
