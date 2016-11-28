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
    void loadData(Technology* currentTech, Technology* nextTech, int currentProgress);
    void updateWidget(int currentIndex);
    void loadTechList(QString filename);

private slots:
    void on_listWidget_itemSelectionChanged();

private:
    Ui::TechTree *ui;
    QVector <Technology*> techListCopy;
    int localIndex;
};

#endif // TECHTREE_H
