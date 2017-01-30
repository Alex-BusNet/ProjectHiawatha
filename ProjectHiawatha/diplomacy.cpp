#include "diplomacy.h"
#include "unitcontroller.h"

#include <QMessageBox>
#include <QDebug>

Diplomacy::Diplomacy(QWidget *parent) : QWidget(parent)
{
    QString DiploStyle = "QWidget { background-color: #145e88; } QFrame { background-color: black; }";
    DiploStyle += "QPushButton { background-color: #4899C8; border: 1px solid #f6b300; border-radius: 6px; font: 10px; min-width: 100px;}";
    DiploStyle += "QPushButton:pressed { background-color: #77adcb; }";
    DiploStyle += "QScrollBar:vertical { border: 2px sold black; background: #77adcb; width: 15px; margin: 12px 0 12px 0;} QScrollBar::handle:vertical { background: #4899C8; min-height: 10px; }";
    DiploStyle += "QScrollBar::add-line:vertical { border: 1px solid black; background: #dedede; height: 10px; subcontrol-position: bottom; subcontrol-origin: margin; }  QScrollBar::sub-line:vertical { border: 1px solid black; height: 10px; background: #dedede; subcontrol-position: top; subcontrol-origin: margin; }";
    DiploStyle += "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 1px solid black; width: 3px; height: 3px; background: purple; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }";
    DiploStyle += "QListView { background-color: grey; font-size: 24; color: black; border: 5px inset #f6b300; show-decoration-selected: 1; } QLabel{ border: 2px inset #f6b300; background: #dedede; color: black; font: bold; max-height: 15px; min-width: 150px; text-align: center; }";

    this->setStyleSheet(DiploStyle);
    leaderListArea = new QListWidget(this);
    leaderImage = new QLabel("?");

    this->setFixedSize(1000, 500);
    leaderListArea->setViewMode(QListWidget::ListMode);
    leaderListArea->setAcceptDrops(false);
    leaderListArea->setMaximumSize(650, 500);

    leaderName = new QLabel(" ");
    warStatus = new QLabel(" ");
    nationName = new QLabel(" ");
    atWarWith = new QLabel(" ");
    atWarWith->setStyleSheet("QLabel{ border: 2px inset #f6b300; background: #dedede; color: red; font: bold; min-height: 50px; max-height: 100px; min-width: 150px; }");
    turn = 0;

    makePeace = new QPushButton("Make Peace");
    makePeace->setEnabled(false);

    declareWar = new QPushButton("Declare War");
    declareWar->setEnabled(true);

    closeDiplo = new QPushButton("Return to game");
    connect(closeDiplo, SIGNAL(clicked(bool)), this, SLOT(hide()));

    connect(leaderListArea, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectLeader(QListWidgetItem*)));

    QHBoxLayout *civInfo = new QHBoxLayout();
    civInfo->addWidget(leaderName);
    civInfo->addWidget(nationName);

    QVBoxLayout *overview = new QVBoxLayout();
    overview->addWidget(leaderImage);
    overview->addLayout(civInfo);
    overview->addWidget(atWarWith);

    QHBoxLayout *warButtons = new QHBoxLayout();
    warButtons->addWidget(declareWar);
    warButtons->addWidget(makePeace);
    overview->addLayout(warButtons);

    QVBoxLayout *list = new QVBoxLayout();
    list->addWidget(leaderListArea);
    list->addWidget(closeDiplo);

    QHBoxLayout *window = new QHBoxLayout();
    window->addLayout(list);
    window->addLayout(overview);

    this->setLayout(window);
}

Diplomacy::~Diplomacy()
{
    qDebug() << "   Diplomacy Dec'tor called";
    foreach(DiplomacyItem* di, diploItemList)
    {
        if(di != NULL)
            delete di;
    }

    delete closeDiplo;
    delete leaderImage;
    delete leaderName;
    delete warStatus;
    delete nationName;
    delete atWarWith;
    delete leaderListArea;

    delete declareWar;
    delete makePeace;

    qDebug() << "   --Diplomacy Deconstructed";
}

void Diplomacy::UpdateTurn()
{
    turn++;
}

void Diplomacy::UpdateLeader()
{
    selectLeader(leaderListArea->item(0));
}

int Diplomacy::GetLengthOfWar(Nation ai)
{
    for(int i = 0; i < diploItemList.at(0)->warChart.size(); i++)
    {
        if(diploItemList.at(0)->warChart.at(i).nation == ai)
        {
            return (turn - diploItemList.at(0)->warChart.at(i).warStartedOn);
        }
    }
}

int Diplomacy::GetNumberOfWars(Nation ai)
{
    for(int i = 0; i < diploItemList.at(0)->warChart.size(); i++)
    {
        if(diploItemList.at(0)->warChart.at(i).nation == ai)
        {
            return diploItemList.at(0)->warChart.at(i).timesAtWarWith;
        }
    }
}

bool Diplomacy::AtPermanentWar(Nation ai)
{
    for(int i = 0; i < diploItemList.at(0)->warChart.size(); i++)
    {
        if(diploItemList.at(0)->warChart.at(i).nation == ai)
        {
            if(diploItemList.at(0)->warChart.at(i).warStat == PERMANENT_WAR)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

int Diplomacy::GetIndex()
{
    return leaderListArea->currentRow();
}

void Diplomacy::AddLeader(QString _name, QPixmap _image, Nation _nation, bool isPlayer)
{
    DiplomacyItem *di = new DiplomacyItem{_image, _name, QString(" "), _nation};
    if(!isPlayer)
    {
        WarHistory wh{_nation, 0, 0, AT_PEACE};

        //Add new leader WarHistory to existing leader's warChart
        foreach(DiplomacyItem *d, diploItemList)
        {
            d->warChart.push_back(wh);
        }

        //Add existing leaders to new leader's warChart
        foreach(DiplomacyItem *d, diploItemList)
        {
            wh.nation = d->nation;
            wh.timesAtWarWith = 0;
            wh.warStartedOn = 0;
            wh.warStat = AT_PEACE;

            di->warChart.push_back(wh);
        }

        di->displayString = QString("%1 \t(%2)\n").arg(_name).arg(UnitController::NationName(_nation));

        di->warChart.push_back(WarHistory{_nation, 0, 0, SELF});
    }
    else
    {
        di->displayString = QString("%1 (You)\t(%2)\n").arg(_name).arg(UnitController::NationName(_nation));
        di->warChart.push_back(WarHistory{_nation, 0, 0, SELF});
    }

    leaderListArea->addItem(di->displayString);
    diploItemList.push_back(di);
}

void Diplomacy::RemoveLeader(Nation nation)
{
    for(int i = 0; i < diploItemList.size(); i++)
    {
        if(diploItemList[i]->nation == nation)
        {
            leaderListArea->takeItem(i);
            diploItemList.remove(i);
            break;
        }
    }
}

void Diplomacy::DeclareWarOn(Nation target, int targetIndex, Nation aggressor)
{
    for(int i = 0; i < diploItemList.at(targetIndex)->warChart.size(); i++)
    {
        if(diploItemList.at(targetIndex)->warChart.at(i).nation == aggressor)
        {
            WarHistory wh{NO_NATION, 0,0, AT_PEACE};
            wh.nation = aggressor;
            wh.timesAtWarWith = diploItemList.at(targetIndex)->warChart.at(i).timesAtWarWith + 1;
            wh.warStartedOn = turn;

            if(diploItemList.at(targetIndex)->warChart.at(i).timesAtWarWith >= 4)
            {
                wh.warStat = PERMANENT_WAR;
            }
            else
            {
                wh.warStat = AT_WAR;
            }

            diploItemList.at(targetIndex)->warChart.replace(i, wh);
            break;
        }
    }

    WarHistory wh{NO_NATION, 0, 0, AT_PEACE};
    wh.nation = target;
    wh.timesAtWarWith = diploItemList.at(0)->warChart.at(targetIndex).timesAtWarWith + 1;
    wh.warStartedOn = turn;

    if(diploItemList.at(0)->warChart.at(targetIndex).timesAtWarWith >= 4)
    {
        wh.warStat = PERMANENT_WAR;
    }
    else
    {
        wh.warStat = AT_WAR;
    }

    diploItemList.at(0)->warChart.replace(targetIndex, wh);
}

bool Diplomacy::MakePeaceWith(Nation player, int targetIndex, Nation ai)
{
    for(int i = 0; i < diploItemList.at(targetIndex)->warChart.size(); i++)
    {
        if(diploItemList.at(targetIndex)->warChart.at(i).nation == player)
        {
            WarHistory wh{NO_NATION, 0, 0, AT_PEACE};
            wh.nation = player;
            wh.timesAtWarWith = diploItemList.at(targetIndex)->warChart.at(i).timesAtWarWith;
            wh.warStartedOn = diploItemList.at(targetIndex)->warChart.at(i).warStartedOn;
            diploItemList.at(targetIndex)->warChart.replace(i, wh);
            break;
        }
    }

    for(int i = 0; i < diploItemList.at(0)->warChart.size(); i++)
    {
        if(diploItemList.at(0)->warChart.at(i).nation == ai)
        {
            diploItemList.at(0)->warChart.replace(i, WarHistory{diploItemList.at(0)->nation, diploItemList.at(0)->warChart.at(i).warStartedOn, diploItemList.at(0)->warChart.at(i).timesAtWarWith, AT_PEACE});
            return true;
        }
    }

    return true;
}

void Diplomacy::selectLeader(QListWidgetItem *item)
{
    DiplomacyItem *d = diploItemList.at(leaderListArea->currentRow());
    leaderName->setText(d->leaderName);
    leaderImage->setPixmap(d->image.scaled(350,350));
    leaderImage->setFixedSize(350, 350);
    nationName->setText(UnitController::NationName(d->nation));

    QString war(" ");
    bool first = true;

    foreach(WarHistory wh, d->warChart)
    {
        if(wh.warStat == AT_WAR)
        {
            if(first)
            {
                war.append("At war with:\n\t");
                first = false;
            }
            war.append(UnitController::NationName(wh.nation));
            war.append("\n\t");
        }
    }
    atWarWith->setText(war);

    if((leaderListArea->currentRow() != 0))
    {
        if(d->warChart.at(0).warStat == AT_WAR || d->warChart.at(0).warStat == PERMANENT_WAR)
        {
            declareWar->setEnabled(false);
            makePeace->setEnabled(true);
        }
        else
        {
            declareWar->setEnabled(true);
            makePeace->setEnabled(false);
        }
    }
    else
    {
        declareWar->setEnabled(false);
        makePeace->setEnabled(false);
    }
}

void Diplomacy::slot_delcareWar()
{
    Nation t = diploItemList.at(leaderListArea->currentRow())->nation;
    this->DeclareWarOn(t, leaderListArea->currentRow(), player);
}

void Diplomacy::slot_makePeace()
{
    Nation t = diploItemList.at(leaderListArea->currentRow())->nation;
    this->MakePeaceWith(t, leaderListArea->currentRow(), player);
}
