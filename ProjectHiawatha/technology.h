#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include <QString>

class Technology
{
public:
    Technology();
    Technology(QString str, int techCost, int techIndex );
    ~Technology();
    QString getName();
    int getCost();
    int getIndex();
    setName(QString str);
    setIndex(int techIndex);
    setCost(int techCost);

private:
    QString name;
    int cost;
    int index;

};

#endif // TECHNOLOGY_H
