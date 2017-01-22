#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include "nation.h"

class Diplomacy : public QWidget
{
    Q_OBJECT
public:
    explicit Diplomacy(QWidget *parent = 0);
//    ~Diplomacy();

    void AddLeader(QString _name, QPixmap _image, Nation _nation, bool isPlayer);
    void RemoveLeader(Nation nation);
    void DeclareWarOn(Nation target, Nation aggressor);
    void UpdateTurn();
    void UpdateLeader();

    int GetLengthOfWar(Nation ai);
    int GetNumberOfWars(Nation ai);
    int GetIndex();

    bool MakePeaceWith(Nation player, Nation ai);
    bool AtPermanentWar(Nation ai);

    QPushButton *declareWar;
    QPushButton *makePeace;

private:
    enum WarStatus {AT_WAR, AT_PEACE, PERMANENT_WAR, SELF};
    typedef struct {Nation nation; int warStartedOn, timesAtWarWith; WarStatus warStat;} WarHistory;
    typedef struct {QPixmap image; QString leaderName, displayString; Nation nation; QVector<WarHistory> warChart;} DiplomacyItem;

    QPushButton *closeDiplo;
    QLabel *leaderImage, *leaderName, *warStatus, *nationName, *atWarWith;

    QListWidget *leaderListArea;

    QVector<DiplomacyItem*> diploItemList;
    Nation player;

    int turn;

signals:

public slots:
    void selectLeader(QListWidgetItem *item);
    void slot_delcareWar();
    void slot_makePeace();
};

#endif // DIPLOMACY_H