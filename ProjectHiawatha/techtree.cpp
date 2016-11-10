#include "techtree.h"
#include "ui_techtree.h"
#include <QDebug>

TechTree::TechTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TechTree)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0,"Current Tech");
    ui->tabWidget->setTabText(1,"Next Tech");
    ui->tabWidget->setTabText(2,"Future Techs");
    ui->progressBar->setMinimum(0);

}

TechTree::~TechTree()
{
    delete ui;
}

void TechTree::loadData(Technology *currentTech, Technology *nextTech, int currentProgress)
{
    QString str = "../ProjectHiawatha/Assets/Techs/";
    QString str2 = currentTech->getName();
    QString str3 = nextTech->getName();
    QString str4 = str + str3;
    str4 = str4 + ".png";
    str = str + str2;
    str = str + ".png";
    qDebug()<<"STRING EQUALS "<<str;
    QString tempStr = "Cost: ";
    QString cost;
    QString cost2;
    cost= cost.number(currentTech->getCost());
    cost2 = cost2.number(nextTech->getCost());
    QString tempStr2 = tempStr + cost2;
    tempStr = tempStr + cost;
    QPixmap pic(str);
    QPixmap pic2(str4);
    ui->label->setText(currentTech->getName());
    ui->label_2->setPixmap(pic.scaled(ui->label_2->size()));
    ui->label_6->setText(tempStr);
    ui->label_3->setText(nextTech->getName());
    ui->label_4->setPixmap(pic2.scaled(ui->label_4->size()));
    ui->label_5->setText(tempStr2);
    ui->progressBar->setMaximum(cost.toInt());
    ui->progressBar->setValue(currentProgress);
    this->update();
}
