#include "techtree.h"
#include "ui_techtree.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

TechTree::TechTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TechTree)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0,"Current Tech");
    ui->tabWidget->setTabText(1,"Next Tech");
    ui->tabWidget->setTabText(2,"Future Techs");
    ui->progressBar->setMinimum(0);
    ui->Cost->setText("No Tech Selected");
    ui->pic->setText(" ");

}

TechTree::~TechTree()
{
    qDebug() << "   TechTree Dec'tor called";
    foreach(Technology* t, techListCopy)
    {
        if(t != NULL)
            delete t;
    }

    delete ui;
    qDebug() << "   --TechTree Deconstructed";
}

void TechTree::loadData(Technology *currentTech, Technology *nextTech, int currentProgress)
{
#ifdef __APPLE__
    QDir bin(QCoreApplication::applicationDirPath());
    qDebug()<<"TechTree";
    bin.cdUp();
    bin.cdUp();
    bin.cdUp();
    QDir::setCurrent(bin.absolutePath());
#endif
    QString str = "Assets/Techs/";
    QString str2 = currentTech->getName();
    QString str3 = nextTech->getName();
    QString str4 = str + str3;
    str4 = str4 + ".png";
    str = str + str2;
    str = str + ".png";
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

void TechTree::updateWidget(int currentIndex)
{
    this->localIndex = currentIndex;
    for(int i = currentIndex;i<techListCopy.size();i++)
    {
        ui->listWidget->addItem(techListCopy.at(i)->getName());

    }
    this->update();
}

void TechTree::loadTechList(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList techInfo = line.split(",");
          int x = techInfo[1].toInt();
          int y = techInfo[2].toInt();
          Technology* tech = new Technology(techInfo[0],x,y);
          techListCopy.push_back(tech);
       }
       inputFile.close();
    }else
    {
        QMessageBox* mBox = new QMessageBox();
        mBox->setText("File Not Found");
        mBox->exec();
        qDebug()<<"File Not Found";

    }
}



void TechTree::on_listWidget_itemSelectionChanged()
{
#ifdef __APPLE__
    QDir bin(QCoreApplication::applicationDirPath());
    qDebug()<<"TechTree";
    bin.cdUp();
    bin.cdUp();
    bin.cdUp();
    QDir::setCurrent(bin.absolutePath());
#endif
    int vectorIndex = ui->listWidget->currentRow()+ localIndex;
    QString str = "Assets/Techs/";
    QString str2 = techListCopy.at(vectorIndex)->getName();
    str = str + str2;
    str = str + ".png";
    QString tempStr = "Cost: ";
    QString cost;
    cost = cost.number(techListCopy.at(vectorIndex)->getCost());
    tempStr = tempStr + cost;
    QPixmap picture(str);
    ui->Cost->setText(tempStr);
    ui->pic->setPixmap(picture.scaled(ui->pic->size()));
}
