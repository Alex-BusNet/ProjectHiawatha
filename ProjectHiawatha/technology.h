#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include <QJsonObject>
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
    void setName(QString str);
    void setIndex(int techIndex);
    void setCost(int techCost);

    void WriteTechSaveData(QJsonObject &obj) const;
    void ReadTechSaveData(const QJsonObject &obj);

private:
    QString name;
    int cost;
    //Techs are stored in a list. Index helps to identify which tech a civ is on
    int index;

};

#endif // TECHNOLOGY_H
