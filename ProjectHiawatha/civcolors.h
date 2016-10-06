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

    CivColors()
    {
        this->NO_NATION_PRIMARY.setRgb(255, 255, 255, 0);
        this->AMERICA_PRIMARY.setRgb(0, 0, 255, 255);
        this->GERMANY_PRIMARY.setRgb(255, 0, 0, 255);
        this->INDIA_PRIMARY.setRgb(0, 255, 0, 255);
        this->CHINA_PRIMARY.setRgb(0, 200, 150, 255);
    }
};

#endif // CIVCOLORS_H
