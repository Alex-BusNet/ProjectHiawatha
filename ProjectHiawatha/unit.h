#ifndef UNIT_H
#define UNIT_H

#include <QImage>
#include <QList>
#include "nation.h"
#include "unittype.h"
#include "tile.h"

class Tile;

class Unit
{
public:
    Unit();
    Unit(Nation owner = India, bool isNonCombat = true, bool isSettler = false, int movementPoints = 2, int strength = 1, int range = 3, int health = 100, UnitType type = WORKER);
    ~Unit();

    void SetUnitIcon(UnitType type);
    void SetOwner(Nation owner);

    void SetPosition(int column, int row);
    void SetPositionIndex(int tileMapIndex);
    void SetPixmapIndex(int index);
    void SetUnitListIndex(int index);
    void SetUnitTargetTileIndex(int tileIndex);
    void SetUnitTargetTile(int column, int row);

    void SetMovementPoints(int pts);
    void SetStrength(int strength);
    void SetRange(int range);
    void SetRangeStrength(int rangeStr);
    void SetHealth(int health);
    void SetMaxHealth(int maxHealth);
    void SetPath(QList<Tile*> newPath);

    void SetUnitImage(QImage *image);
    void Use();

    void UpdatePath();

    QImage* GetUnitIcon();
    Nation GetOwner();

    int GetTileIndex();
    int GetTargetTileIndex();
    int GetTileColumn();
    int GetTileRow();
    int GetTargetTileColumn();
    int GetTargetTileRow();

    int GetMovementPoints();
    int GetStrength();
    int GetRange();
    int GetRangeStrength();
    int GetHealth();
    int GetMaxHealth();
    int GetRemainingUses();

    int GetPixmapIndex();
    int GetUnitListIndex();

    QList<Tile*> GetPath();

    bool isPathEmpty();
    bool isNonCombat();
    bool isSettler();

    bool RequiresOrders;
    bool Updated;
    bool HasNoMovementLeft;
    bool isFortified;
    bool isMelee;
    bool needsPath;


private:
    QImage *unitIcon;

    Nation belongsTo;
//    Tile *position;
    int colPosition, targetColPos;
    int rowPosition, targetRowPos;

    int pixmapIndex, dataMapIndex, unitListIndex, targetTileIndex;
    bool NonCombat;
    bool Settler;

    int movementPoints;
    int strength;
    int range;
    int rangeStrength;
    int health;
    int maxHealth;
    int uses;

    QList<Tile*> path;
};

#endif // UNIT_H
