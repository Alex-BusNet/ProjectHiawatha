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

Technology::setName(QString str)
{
    name = str;
}

Technology::setIndex(int techIndex)
{
    index = techIndex;
}

Technology::setCost(int techCost)
{
    cost = techCost;
}





