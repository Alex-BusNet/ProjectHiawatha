#ifndef TECHTREE_H
#define TECHTREE_H

#include <QWidget>
#include <QString>
#include "technology.h"

namespace Ui {
class TechTree;
}

class TechTree : public QWidget
{
    Q_OBJECT

public:
    explicit TechTree(QWidget *parent = 0);
    ~TechTree();
    //Loads in the civ's cuurentTech, nextTech and currentProgress towards the currentTech
    void loadData(Technology* currentTech, Technology* nextTech, int currentProgress);
    //Adds the techs yet to be researched to the list on the future techs tab
    void updateWidget(int currentIndex);
    //loads a copy of the initial tech list to use for comparison
    void loadTechList(QString filename);

private slots:
    void on_listWidget_itemSelectionChanged();

private:
    Ui::TechTree *ui;
    //copy of the list of tech objects. used to compare to the civ's list in order to know what to display
    QVector <Technology*> techListCopy;
    //used to keep track of which item has been selected in the list
    int localIndex;
};

#endif // TECHTREE_H
