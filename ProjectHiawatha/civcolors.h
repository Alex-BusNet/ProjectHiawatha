#ifndef CIVCOLORS_H
#define CIVCOLORS_H

#include <QColor>
#include "nation.h"

class CivColors
{
public:
    QColor NO_NATION_PRIMARY;
    QColor NO_NATION_SECONDARY;
    QColor AMERICA_PRIMARY;
    QColor GERMANY_PRIMARY;
    QColor INDIA_PRIMARY;
    QColor CHINA_PRIMARY;
    QColor MONGOLIA_PRIMARY;
    QColor AZTEC_PRIMARY;
    QColor FRANCE_PRIMARY;
    QColor IROQUOIS_PRIMARY;
    QColor GREECE_PRIMARY;
    QColor ROME_PRIMARY;
    QColor ENGLAND_PRIMARY;
    QColor ARABIA_PRIMARY;
    QColor PERSIA_PRIMARY;
    QColor RUSSIA_PRIMARY;
    QColor JAPAN_PRIMARY;
    QColor EGYPT_PRIMARY;

    CivColors()
    {
        this->NO_NATION_PRIMARY.setRgb(255, 255, 255, 0);
        this->NO_NATION_SECONDARY.setRgb(200, 200, 200, 0);
        this->AMERICA_PRIMARY.setRgb(0, 0, 255, 255);
        this->GERMANY_PRIMARY.setRgb(120, 120, 120, 255);
        this->INDIA_PRIMARY.setRgb(0, 213, 0, 255);
        this->CHINA_PRIMARY.setRgb(0, 200, 150, 255);
        this->MONGOLIA_PRIMARY.setRgb(201, 38, 43, 255);
        this->AZTEC_PRIMARY.setRgb(175, 90, 0, 255);
        this->FRANCE_PRIMARY.setRgb(58, 132, 170, 255);
        this->IROQUOIS_PRIMARY.setRgb(0, 66, 128, 255);
        this->GREECE_PRIMARY.setRgb(0, 155, 255, 255);
        this->ROME_PRIMARY.setRgb(74, 0, 143, 255);
        this->ENGLAND_PRIMARY.setRgb(132, 0, 8, 255);
        this->ARABIA_PRIMARY.setRgb(0, 109, 0, 255);
        this->PERSIA_PRIMARY.setRgb(221, 39, 0, 255);
        this->RUSSIA_PRIMARY.setRgb(255, 167, 0, 255);
        this->JAPAN_PRIMARY.setRgb(245, 245, 245, 255);
        this->EGYPT_PRIMARY.setRgb(248, 232, 128, 255);
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
        case Iroquois:
            return IROQUOIS_PRIMARY;
            break;
        case Greece:
            return GREECE_PRIMARY;
            break;
        case Rome:
            return ROME_PRIMARY;
            break;
        case England:
            return ENGLAND_PRIMARY;
            break;
        case Arabia:
            return ARABIA_PRIMARY;
            break;
        case Persia:
            return PERSIA_PRIMARY;
            break;
        case Russia:
            return RUSSIA_PRIMARY;
            break;
        case Japan:
            return JAPAN_PRIMARY;
            break;
        case Egypt:
            return EGYPT_PRIMARY;
            break;
        default:
            return NO_NATION_PRIMARY;
            break;
        }
    }
};

#endif // CIVCOLORS_H
