#ifndef DATATYPES_H
#define DATATYPES_H

#include <QVector>


typedef struct {bool updateBorders, updateCitizens, productionFinished, cityHealed; QVector<int> cityIndecies;} Update_t;

typedef struct {int column; int row;} TileID;
typedef struct {int column, row; bool newData, relocateOrderGiven;} TileData;
typedef struct {int index; bool player, target; } SelectData;

typedef struct {int gold, production, research, food, culture;} YieldStruct;

typedef struct{int IconIndex; QString ToolTipMessage;} Notification;

#endif // DATATYPES_H
