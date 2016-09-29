#include "gameoptions.h"
#include "ui_gameoptions.h"
#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QPixmap>

GameOptions::GameOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameOptions)
{
    ui->setupUi(this);

    //new QListWidgetItem(tr("China"), ui->listWidget);
    //new QListWidgetItem(tr("Germany"), ui->listWidget);
    //new QListWidgetItem(tr("India"), ui->listWidget);
}

GameOptions::~GameOptions()
{
    delete ui;
}

void GameOptions::paintEvent(QPaintEvent *e)
{
//     QPixmap pic("../MessingAroundWithAnimation/George_head.jpg");
//     QPixmap pic2("../MessingAroundWithAnimation/Mao.jpg");
//     QPixmap pic3("../MessingAroundWithAnimation/hitler.jpg");

//    if(ui->listWidget->currentRow()==0){
//        ui->label_3->setText("Mao Zedong");
//        ui->label_2->setPixmap(pic2);
//    }else if(ui->listWidget->currentRow()==3){
//        ui->label_3->setText("George Washington");
//        ui->label_2->setPixmap(pic);
//    }else if(ui->listWidget->currentRow()==1){
//        ui->label_3->setText("Hitler");
//        ui->label_2->setPixmap(pic3);
//    }
//    else{
//        ui->label_3->setText("Gandhi");
//    }
}
