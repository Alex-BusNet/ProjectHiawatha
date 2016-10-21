#include "gameoptions.h"
#include "ui_gameoptions.h"
#include "gamemanager.h"
#include "nation.h"
#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QPixmap>
#include <QDebug>
#include <QMediaPlayer>

typedef struct{QString CivName; QString LeaderName; QString PrimaryColor; QString SecondColor; int mapDimension1;
               int mapDimension2;}CivInfo;

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
    QListWidgetItem* item4 = new QListWidgetItem(tr("United States of America"),ui->listWidget);
    QListWidgetItem* item5 =new QListWidgetItem(tr("Mongolia"), ui->listWidget);
    QListWidgetItem* item6 =new QListWidgetItem(tr("Aztec Empire"), ui->listWidget);
    QListWidgetItem* item7 =new QListWidgetItem(tr("France"), ui->listWidget);
    QIcon* icon1 = new QIcon();
    QIcon* icon2 = new QIcon();
    QIcon* icon3 = new QIcon();
    QIcon* icon4 = new QIcon();
    QIcon* icon5 = new QIcon();
    QIcon* icon6 = new QIcon();
    QIcon* icon7 = new QIcon();
    icon1->addFile("../ProjectHiawatha/Assets/Leaders/Icons/CHINA.png");
    icon2->addFile("../ProjectHiawatha/Assets/Leaders/Icons/GERMANY2.png");
    icon3->addFile("../ProjectHiawatha/Assets/Leaders/Icons/INDIA.png");
    icon4->addFile("../ProjectHiawatha/Assets/Leaders/Icons/USA.png");
    icon5->addFile("../ProjectHiawatha/Assets/Leaders/Icons/MONGOLIA.jpg");
    icon6->addFile("../ProjectHiawatha/Assets/Leaders/Icons/AZTEC.png");
    icon7->addFile("../ProjectHiawatha/Assets/Leaders/Icons/FRANCE.png");
    item1->setIcon(*icon1);
    item2->setIcon(*icon2);
    item3->setIcon(*icon3);
    item4->setIcon(*icon4);
    item5->setIcon(*icon5);
    item6->setIcon(*icon6);
    item7->setIcon(*icon7);
    ui->listWidget->addItem(item1);
    ui->listWidget->addItem(item2);
    ui->listWidget->addItem(item3);
    ui->listWidget->addItem(item4);
    ui->listWidget->addItem(item5);
    ui->listWidget->addItem(item6);
    ui->listWidget->addItem(item7);
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
    QPixmap pic5("../ProjectHiawatha/Assets/Leaders/khan.jpg");
    QPixmap pic6("../ProjectHiawatha/Assets/Leaders/montezuma.jpg");
    QPixmap pic7("../ProjectHiawatha/Assets/Leaders/napoleon.jpg");
    switch(ui->listWidget->currentRow())
    {
        case 0:ui->label_3->setText("Mao Zedong");
               ui->label->setPixmap(pic);
               break;
        case 1:ui->label_3->setText("Otto Von Bismarck");
               ui->label->setPixmap(pic3);
               break;
        case 2:ui->label_3->setText("Gandhi");
               ui->label->setPixmap(pic4);
               break;
        case 3:ui->label_3->setText("George Washington");
               ui->label->setPixmap(pic2);
               break;
        case 4:ui->label_3->setText("Genghis Khan");
               ui->label->setPixmap(pic5);
               break;
        case 5:ui->label_3->setText("Montezuma");
               ui->label->setPixmap(pic6);
               break;
        case 6:ui->label_3->setText("Napoleon Bonaparte");
               ui->label->setPixmap(pic7);
               break;
        default:ui->label_3->setText("Gandhi");
                ui->label->setPixmap(pic4);

    }

}

void GameOptions::on_pushButton_clicked()
{
    QListWidgetItem* selectedItem;
    if(ui->listWidget->currentItem() == NULL)
    {
        selectedItem = ui->listWidget->item(0);
    }else
    {
        selectedItem = ui->listWidget->currentItem();
    }
    QString str = ui->comboBox->currentText();
    QString tempStr = str.remove(0,(str.indexOf(':',0)+1));
    QString tempStr2 = tempStr.remove((tempStr.indexOf('x',0)),20);
    int mapSize1 = tempStr2.toInt();
    mapSize1 = mapSize1 / 2;
    tempStr = str.remove(0,(str.indexOf('x',0)+1));
    tempStr2 = tempStr.remove((tempStr.indexOf('x',0)+3),20);
    int mapSize2 = tempStr2.toInt();
    CivInfo info = {selectedItem->text(),ui->label_3->text(),"red","blue",mapSize1,mapSize2};//This is data that needs passed
    if(game != NULL)
    {
        delete game;
    }
    qDebug()<<"Map Size: "<<info.mapDimension1;
    switch(ui->listWidget->currentRow())
    {
        case 0:player = China;
               break;
        case 1:player = Germany;
               break;
        case 2:player = India;
               break;
        case 3:player = America;
               break;
        case 4:player = Mongolia;
               break;
        case 5:player = Aztec;
               break;
        case 6:player = France;
               break;
        default:player = India;

    }
    QMediaPlayer *musicPlayer = new QMediaPlayer();
    musicPlayer->setMedia(QUrl::fromLocalFile("../ProjectHiawatha/Assets/Sound/notificationunitkilled.wav"));
    musicPlayer->setVolume(50);
    musicPlayer->play();
    game = new GameManager(0, FullScreen, info.mapDimension1, info.mapDimension2, player);
    this->close();

}


void GameOptions::on_listWidget_itemSelectionChanged()
{
    ui->pushButton->setFlat(false);
    this->update();
}
