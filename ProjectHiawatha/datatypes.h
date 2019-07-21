#ifndef DATATYPES_H
#define DATATYPES_H

#include <QVector>
#include "nation.h"

class Tile;

enum ActionState { MOVE_UNIT, ATTACK_MELEE, ATTACK_RANGE, ATTACK_CITY, FOUND_CITY, AI_FOUND_CITY, FIND_UNIT, FIND_CITY, INVADE, IDLE, CONQUER, AI_DECLARE_WAR, CLEAR };
enum ViewState { DISCOVERED, HIDDEN, VISIBLE };
typedef struct {bool updateBorders, updateCitizens, productionFinished, cityHealed; } Update_t;

typedef struct {int column; int row;} TileID;
typedef struct {int column, row; bool newData, relocateOrderGiven;} TileData;
typedef struct {int index; bool player, target; } SelectData;
typedef struct {int index; ViewState state;} ViewData;

typedef struct {int gold, production, research, food, culture;} YieldStruct;

typedef struct{int IconIndex; QString ToolTipMessage;} Notification;
typedef struct{bool atWar; QVector<int> warringCivListIndex;} WarData;

typedef struct{int cityIndex; int producedUnitIndex; bool isUnit; int civIndex;} CityProdData;

typedef struct{Nation OccupantNation; int civIndex; int unitIndex; bool nonCombat;} OccupantData;
typedef struct{Tile* tile; OccupantData od;} MapData;

const OccupantData DEFAULT_OCCUPANT{NO_NATION, -1, -1, true};

#endif // DATATYPES_H
