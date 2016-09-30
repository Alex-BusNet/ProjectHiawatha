#include "gameoptions.h"
#include "ui_gameoptions.h"
#include "gamemanager.h"
#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QPixmap>
#include <QDebug>

typedef struct{QString CivName; QString LeaderName; QString PrimaryColor; QString SecondColor;}CivInfo;

GameOptions::GameOptions(QWidget *parent, bool fullscreen) :
    QWidget(parent),
    ui(new Ui::GameOptions)
{
    ui->setupUi(this);
    FullScreen = fullscreen;
    game = NULL;
    QListWidgetItem* item1 =new QListWidgetItem(tr("China"), ui->listWidget);
    QListWidgetItem* item2 =new QListWidgetItem(tr("Germany"), ui->listWidget);
    QListWidgetItem* item3 =new QListWidgetItem(tr("India"), ui->listWidget);
    QListWidgetItem* item4 = new QListWidgetItem();
    QIcon* icon1 = new QIcon();
    QIcon* icon2 = new QIcon();
    QIcon* icon3 = new QIcon();
    QIcon* icon4 = new QIcon();
    item4->setText("United States of America");
    icon1->addFile("../ProjectHiawatha/Assets/Leaders/Icons/CHINA.png");
    icon2->addFile("../ProjectHiawatha/Assets/Leaders/Icons/GERMANY.png");
    icon3->addFile("../ProjectHiawatha/Assets/Leaders/Icons/INDIA.png");
    icon4->addFile("../ProjectHiawatha/Assets/Leaders/Icons/USA.png");
    item1->setIcon(*icon1);
    item2->setIcon(*icon2);
    item3->setIcon(*icon3);
    item4->setIcon(*icon4);
    ui->listWidget->addItem(item1);
    ui->listWidget->addItem(item2);
    ui->listWidget->addItem(item3);
    ui->listWidget->addItem(item4);
    ui->comboBox->addItem("Duel: 40x24 - 2 Players",0);
    ui->comboBox->addItem("Tiny: 56x36 - 4 Players",0);
    ui->comboBox->addItem("Small: 66x42 - 6 Players",0);
    ui->comboBox->addItem("Standard: 80x52 - 8 Players",0);
    ui->comboBox->addItem("Large: 104x64 - 10 Players",0);
    ui->comboBox->addItem("Huge: 128x80 - 12 Players",0);
    ui->label->setScaledContents(true);
    ui->pushButton->setFlat(true);
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
    QPixmap pic3("../ProjectHiawatha/Assets/Leaders/bismark.jpg");
    QPixmap pic4("../ProjectHiawatha/Assets/Leaders/gandhi.jpg");

   if(ui->listWidget->currentRow()==0){
       ui->label_3->setText("Mao Zedong");
       ui->label->setPixmap(pic);
   }else if(ui->listWidget->currentRow()==3){
       ui->label_3->setText("George Washington");
       ui->label->setPixmap(pic2);
   }else if(ui->listWidget->currentRow()==1){
       ui->label_3->setText("Otto Von Bismarck");
       ui->label->setPixmap(pic3);
   }
   else{
       ui->label_3->setText("Gandhi");
       ui->label->setPixmap(pic4);
   }
}

void GameOptions::on_pushButton_clicked()
{
    QListWidgetItem* selectedItem = ui->listWidget->currentItem();
    CivInfo info = {selectedItem->text(),ui->label_3->text(),"red","blue"};
    if(game != NULL)
    {
        delete game;
    }
    qDebug()<<"Civ: "<<info.LeaderName;
    game = new GameManager(0, FullScreen);
    this->close();

}


void GameOptions::on_listWidget_itemSelectionChanged()
{
    ui->pushButton->setFlat(false);
    this->update();
}
