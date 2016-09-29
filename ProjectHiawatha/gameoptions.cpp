#include "gameoptions.h"
#include "ui_gameoptions.h"
#include "gamemanager.h"
#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QPixmap>


GameOptions::GameOptions(QWidget *parent, bool fullscreen) :
    QWidget(parent),
    ui(new Ui::GameOptions)
{
    ui->setupUi(this);
    FullScreen = fullscreen;
    game = NULL;
    new QListWidgetItem(tr("China"), ui->listWidget);
    new QListWidgetItem(tr("Germany"), ui->listWidget);
    new QListWidgetItem(tr("India"), ui->listWidget);
    QListWidgetItem* item1 = new QListWidgetItem();
    QIcon* icon1 = new QIcon();
    item1->setText("United States of America");
    icon1->addFile("../ProjectHiawatha/Assets/Leaders/george_washington.gif");
    item1->setIcon(*icon1);
    ui->listWidget->addItem(item1);
    ui->label->setScaledContents(true);
    //ui->label_2->setText("China");
}

GameOptions::~GameOptions()
{
    delete ui;
    delete game;
}

void GameOptions::paintEvent(QPaintEvent *e)
{
    QPixmap pic("../ProjectHiawatha/Assets/Leaders/Mao.jpg");
    QPixmap pic2("../ProjectHiawatha/Assets/Leaders/George_head.jpg");
    QPixmap pic3("../ProjectHiawatha/Assets/Leaders/hitler.jpg");
    QPixmap pic4("../ProjectHiawatha/Assets/Leaders/gandhi.jpg");

   if(ui->listWidget->currentRow()==0){
       ui->label_3->setText("Mao Zedong");
       ui->label->setPixmap(pic);
   }else if(ui->listWidget->currentRow()==3){
       ui->label_3->setText("George Washington");
       ui->label->setPixmap(pic2);
   }else if(ui->listWidget->currentRow()==1){
       ui->label_3->setText("Adolf Hitler");
       ui->label->setPixmap(pic3);
   }
   else{
       ui->label_3->setText("Gandhi");
       ui->label->setPixmap(pic4);
   }
}

void GameOptions::on_pushButton_clicked()
{
    if(game != NULL)
    {
        delete game;
    }
    game = new GameManager(0, FullScreen);
    this->close();

}
