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
    //Unit(Nation owner = India, bool isNonCombat = true, bool isSettler = false, int movementPoints = 2, int strength = 1, int range = 3, int rangeStrength = 2, int health = 100, UnitType type = WORKER);
    Unit(Nation owner = India, UnitType type = WORKER);
    ~Unit();

    void SetUnitIcon(UnitType type);
    void SetOwner(Nation owner);
    void SetName(QString str);
    void SetPosition(int column, int row);
    void SetPositionIndex(int tileMapIndex);
    void SetPixmapIndex(int index);
    void SetUnitListIndex(int index);
    void SetUnitTargetTileIndex(int tileIndex);
    void SetUnitTargetTile(int column, int row);
    void SetHealthBarIndex(int index);
    void SetCost(int price);
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
    int GetHealthBarIndex();
    int GetCost();
    int GetMovementPoints();
    int GetStrength();
    int GetRange();
    int GetRangeStrength();
    int GetHealth();
    int GetMaxHealth();
    int GetRemainingUses();

    int GetPixmapIndex();
    int GetUnitListIndex();

    QString GetName();
    QList<Tile*> GetPath();

    UnitType GetUnitType();

    bool isPathEmpty();
    bool isNonCombat();
    bool isSettler();

    bool RequiresOrders;
    bool Updated;
    bool HasNoMovementLeft;
    bool isFortified;
    bool isMelee;
    bool isRoadWorker;
    bool needsPath;


private:
    QImage *unitIcon;
    QString name;
    Nation belongsTo;
//    Tile *position;
    int colPosition, targetColPos;
    int rowPosition, targetRowPos;

    int pixmapIndex, dataMapIndex, unitListIndex, targetTileIndex, healthBarIndex;
    bool NonCombat;
    bool Settler;

    int movementPoints;
    int strength;
    int range;
    int rangeStrength;
    int health;
    int maxHealth;
    int uses; // uses = -1 for combat units.
    int cost;

    UnitType type;

    QList<Tile*> path;
};

#endif // UNIT_H
