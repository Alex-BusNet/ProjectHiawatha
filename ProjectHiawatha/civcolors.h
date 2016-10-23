#ifndef CIVCOLORS_H
#define CIVCOLORS_H

#include <QColor>
#include "nation.h"

class CivColors
{
public:
    QColor NO_NATION_PRIMARY;
    QColor AMERICA_PRIMARY;
    QColor GERMANY_PRIMARY;
    QColor INDIA_PRIMARY;
    QColor CHINA_PRIMARY;
    QColor MONGOLIA_PRIMARY;
    QColor AZTEC_PRIMARY;
    QColor FRANCE_PRIMARY;

    CivColors()
    {
        this->NO_NATION_PRIMARY.setRgb(255, 255, 255, 0);
        this->AMERICA_PRIMARY.setRgb(0, 0, 255, 255);
        this->GERMANY_PRIMARY.setRgb(255, 0, 0, 255);
        this->INDIA_PRIMARY.setRgb(0, 255, 0, 255);
        this->CHINA_PRIMARY.setRgb(0, 200, 150, 255);
        this->MONGOLIA_PRIMARY.setRgb(175, 38, 43, 255);
        this->AZTEC_PRIMARY.setRgb(175, 90, 0, 255);
        this->FRANCE_PRIMARY.setRgb(0, 124, 175, 255);
    }

    QColor GetCivColor(Nation nation)
    {
        switch(nation)
        {
        case NO_NATION:
            return NO_NATION_PRIMARY;
            break;
        case America:
            return AMERICA_PRIMARY;
            break;
        case Germany:
            return GERMANY_PRIMARY;
            break;
        case India:
            return INDIA_PRIMARY;
            break;
        case China:
            return CHINA_PRIMARY;
            break;
        case Mongolia:
            return MONGOLIA_PRIMARY;
            break;
        case Aztec:
            return AZTEC_PRIMARY;
            break;
        case France:
            return FRANCE_PRIMARY;
            break;
        default:
            return NO_NATION_PRIMARY;
            break;
        }
    }
};

#endif // CIVCOLORS_H
