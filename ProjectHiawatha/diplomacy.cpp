#include "diplomacy.h"
#include "unitcontroller.h"

#include <QMessageBox>
#include <QDebug>

Diplomacy::Diplomacy(GameManager *parent) : QWidget(parent)
{
    QString OptionsStyle = "QWidget { background-color: #145e88; } QFrame { background-color: black; }";
    OptionsStyle += "QPushButton { background-color: #4899C8; border: 1px solid #f6b300; border-radius: 6px; font: 10px; min-width: 100px;}";
    OptionsStyle += "QPushButton:pressed { background-color: #77adcb; }";
    OptionsStyle += "QScrollBar:vertical { border: 2px sold black; background: #77adcb; width: 15px; margin: 12px 0 12px 0;} QScrollBar::handle:vertical { background: #4899C8; min-height: 10px; }";
    OptionsStyle += "QScrollBar::add-line:vertical { border: 1px solid black; background: #dedede; height: 10px; subcontrol-position: bottom; subcontrol-origin: margin; }  QScrollBar::sub-line:vertical { border: 1px solid black; height: 10px; background: #dedede; subcontrol-position: top; subcontrol-origin: margin; }";
    OptionsStyle += "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 1px solid black; width: 3px; height: 3px; background: purple; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }";
    OptionsStyle += "QListView { background-color: white; }";
    OptionsStyle += "QListView#listWidget { border: 5px inset #f6b300; show-decoration-selected: 1; } QLabel#label_3, #label_2 { border: 2px inset #f6b300; background: #dedede; font: bold; max-height: 15px; min-width: 150px; }";
    OptionsStyle += "QComboBox { background-color: white; }";

    this->setStyleSheet(OptionsStyle);
    manager = parent;
    leaderListArea = new QListWidget(this);
    leaderImage = new QLabel("?");
    leaderImage->setGeometry(855, 20, 350, 350);
    leaderImage->setMaximumSize(350, 350);
    this->setFixedSize(1000, 500);
    leaderListArea->setViewMode(QListWidget::ListMode);
    leaderListArea->setAcceptDrops(false);
    leaderListArea->setMaximumSize(650, 500);

    leaderName = new QLabel(" ");
    warStatus = new QLabel(" ");
    nationName = new QLabel(" ");
    atWarWith = new QLabel(" ");
    turn = 0;

    makePeace = new QPushButton("Make Peace");
    connect(makePeace, SIGNAL(clicked(bool)), this, SLOT(slot_makePeace()));
    makePeace->setEnabled(false);

    declareWar = new QPushButton("Declare War");
    connect(declareWar, SIGNAL(clicked(bool)), this, SLOT(slot_delcareWar()));
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

void Diplomacy::UpdateTurn()
{
    turn++;
}

void Diplomacy::AddLeader(QString _name, QPixmap _image, Nation _nation, bool isPlayer)
{

    DiplomacyItem di {_image, _name, QString(" "), _nation};
    if(!isPlayer)
    {
        WarHistory *wh = new WarHistory{_nation, 0, 0, AT_PEACE};

        //Add new leader WarHistory to existing leader's warChart
        foreach(DiplomacyItem d, diploItemList)
        {
            d.warChart.push_back(wh);
        }

        //Add existing leaders to new leader's warChart
        foreach(DiplomacyItem d, diploItemList)
        {
            wh = new WarHistory{d.nation, 0, 0, AT_PEACE};
            di.warChart.push_back(wh);
        }

        di.displayString = QString("%1 \t(%2)\n").arg(_name).arg(UnitController::NationName(_nation));
    }
    else
    {
        di.displayString = QString("%1 (You)\t(%2)\n").arg(_name).arg(UnitController::NationName(_nation));
    }

    leaderListArea->addItem(di.displayString);
    diploItemList.push_back(di);
}

void Diplomacy::RemoveLeader(Nation nation)
{
    for(int i = 0; i < diploItemList.size(); i++)
    {
        if(diploItemList[i].nation == nation)
        {
            leaderListArea->takeItem(i);
            diploItemList.remove(i);
            break;
        }
    }
}

void Diplomacy::DeclareWarOn(Nation target, Nation aggressor)
{
    int row = leaderListArea->currentRow();

    for(int i = 0; i < diploItemList.at(row).warChart.size(); i++)
    {
        if(diploItemList.at(row).warChart.at(i)->nation == aggressor)
        {
            diploItemList.at(row).warChart.at(i)->timesAtWarWith++;
            diploItemList.at(row).warChart.at(i)->warStartedOn = turn;
            diploItemList.at(row).warChart.at(i)->warStat = AT_WAR;
            break;
        }
    }

    for(int i = 0; i < diploItemList.at(0).warChart.size(); i++)
    {
        if(diploItemList.at(0).warChart.at(i)->nation == target)
        {
            diploItemList.at(0).warChart.at(i)->timesAtWarWith++;
            diploItemList.at(0).warChart.at(i)->warStartedOn = turn;
            diploItemList.at(0).warChart.at(i)->warStat = AT_WAR;
            manager->WarByDiplomacy(diploItemList.at(0).warChart.at(i)->nation);
            break;
        }
    }

}

bool Diplomacy::MakePeaceWith(Nation _nat1, Nation _nat2)
{
    int row = leaderListArea->currentRow();

    for(int i = 0; i < diploItemList.at(row).warChart.size(); i++)
    {
        if(diploItemList.at(row).warChart.at(i)->nation == _nat2)
        {
            if((turn - diploItemList.at(row).warChart.at(i)->warStartedOn) <= 10)
            {
                QMessageBox *mbox = new QMessageBox();
                mbox->setText(QString("You cannot make peace for the first ten turns of war."));
                mbox->exec();
                delete mbox;
                return false;
            }

            diploItemList.at(row).warChart.at(i)->warStat = AT_PEACE;
            break;
        }
    }

    for(int i = 0; i < diploItemList.at(0).warChart.size(); i++)
    {
        if(diploItemList.at(0).warChart.at(i)->nation == _nat1)
        {
            diploItemList.at(0).warChart.at(i)->warStat = AT_PEACE;
            return true;
        }
    }


}

void Diplomacy::selectLeader(QListWidgetItem *item)
{
    DiplomacyItem d = diploItemList.at(leaderListArea->currentRow());
    leaderName->setText(d.leaderName);
    leaderImage->setPixmap(d.image);
    nationName->setText(UnitController::NationName(d.nation));

    QString war(" ");
    bool first = true;

    foreach(WarHistory *wh, d.warChart)
    {
        if(wh->warStat == AT_WAR)
        {
            if(first)
            {
                war.append("At war with:\n ");
                first = false;
            }
            war.append(UnitController::NationName(wh->nation));
            war.append("\n ");
        }
    }
    atWarWith->setText(war);

    if((leaderListArea->currentRow() != 0))
    {
        if(d.warChart.at(0)->warStat == AT_WAR)
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
    Nation t = diploItemList.at(leaderListArea->currentRow()).nation;
    this->DeclareWarOn(t, player);
}

void Diplomacy::slot_makePeace()
{
    Nation t = diploItemList.at(leaderListArea->currentRow()).nation;
    this->MakePeaceWith(t, player);
}
