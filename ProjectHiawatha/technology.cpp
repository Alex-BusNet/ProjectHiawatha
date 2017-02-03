#include "technology.h"

Technology::Technology()
{

}

Technology::Technology(QString str, int techCost, int techIndex)
{
    name = str;
    cost = techCost;
    index = techIndex;

}

Technology::~Technology()
{

}

QString Technology::getName()
{
    return name;
}

int Technology::getCost()
{
    return cost;
}

int Technology::getIndex()
{
    return index;
}

void Technology::setName(QString str)
{
    name = str;
}

void Technology::setIndex(int techIndex)
{
    index = techIndex;
}

void Technology::setCost(int techCost)
{
    cost = techCost;
}

void Technology::WriteTechSaveData(QJsonObject &obj) const
{
    obj["name"] = name;
    obj["cost"] = cost;
    obj["index"] = index;
}





