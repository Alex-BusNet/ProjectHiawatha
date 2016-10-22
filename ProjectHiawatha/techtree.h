#ifndef TECHTREE_H
#define TECHTREE_H

#include <QWidget>
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
    void loadData(Technology* currentTech, Technology* nextTech);

private:
    Ui::TechTree *ui;
};

#endif // TECHTREE_H
