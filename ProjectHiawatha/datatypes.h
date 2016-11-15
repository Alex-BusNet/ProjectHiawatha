#ifndef DATATYPES_H
#define DATATYPES_H

typedef struct {bool updateBorders, updateCitizens, productionFinished, cityHealed;} Update_t;

typedef struct {int column; int row;} TileID;
typedef struct {int column, row; bool newData, relocateOrderGiven;} TileData;
typedef struct {int index; bool player, target; } SelectData;

typedef struct {int gold, production, research, food, culture;} YieldStruct;

#endif // DATATYPES_H
