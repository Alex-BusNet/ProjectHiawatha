#ifndef CIVCOLORS_H
#define CIVCOLORS_H

#include <QColor>


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
};

#endif // CIVCOLORS_H
