#ifndef YIELD_H
#define YIELD_H

//
// This class is used to provide the total yield output for tiles.
// This class does not handle the total yield for the entire civ.
//

class Yield
{
public:
    Yield();
    Yield(int gold, int prod, int research, int food, int culture);
    ~Yield();

    enum YIELD { GOLD, PRODUCTION, RESEARCH, FOOD, CULTURE };

    void ChangeYield(YIELD yieldType, int newValue);

    int GetYield(YIELD yieldType);

private:
    int totalYield[5];

    int gold;
    int production;
    int research;
    int food;
    int culture;
};

#endif // YIELD_H
