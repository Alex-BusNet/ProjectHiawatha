#ifndef YIELD_H
#define YIELD_H
#include "datatypes.h"

#include <QJsonObject>
/*
 *
 * This class is used to provide the total yield output for tiles.
 * This class does not handle the total yield for an entire civ.
 * It is used to store, update, and retrieve the yield data for a single object
 *
 */


class Yield
{
public:
    Yield();
    Yield(int gold, int prod, int research, int food, int culture);
    ~Yield();

    enum YIELD { GOLD, PRODUCTION, RESEARCH, FOOD, CULTURE };

    void ChangeYield(YIELD yieldType, int increaseBy);
    void ChangeYield(int gold, int prod, int sci, int food, int cul);

    int GetYield(YIELD yieldType);

    int GetGoldYield();
    int GetProductionYield();
    int GetScienceYield();
    int GetFoodYield();
    int GetCultureYield();

    void WriteYieldSaveData(QJsonObject &obj) const;
    void ReadYieldSaveData(const QJsonObject &obj);

private:

    YieldStruct totalYield;
};

#endif // YIELD_H
