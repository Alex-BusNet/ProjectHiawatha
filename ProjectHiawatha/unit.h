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
    Unit(Nation owner = India, UnitType type = WORKER);
    Unit(int index);
    ~Unit();
    void setUnitType(UnitType type);
    void SetUnitIcon(UnitType type);
    void SetOwner(Nation owner);
    void SetName(QString str);
    void setUnlocked(int unlock);
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
    void SetTechIndex(int index);
    void SetUnitImage(QImage *image);
    void Use();
    void DealDamage(int damage);

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
    int GetTechIndex();
    int GetPixmapIndex();
    int GetUnitListIndex();

    QString GetName();
    QList<Tile*> GetPath();

    UnitType GetUnitType();
    int  isUnlocked();
    bool isPathEmpty();
    bool isNonCombat();
    bool isSettler();
    bool isNaval();
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
    bool naval;
    int movementPoints;
    int strength;
    int range;
    int rangeStrength;
    int health;
    int maxHealth;
    int uses; // uses = -1 for combat units.
    int cost;
    int unlocked;
    int techIndex;
    UnitType type;

    QList<Tile*> path;
};

#endif // UNIT_H
