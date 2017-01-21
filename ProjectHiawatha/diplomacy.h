#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include "gamemanager.h"
#include "nation.h"

class GameManager;

class Diplomacy : public QWidget
{
    Q_OBJECT
public:
    explicit Diplomacy(GameManager *parent = 0);
//    ~Diplomacy();

    void AddLeader(QString _name, QPixmap _image, Nation _nation, bool isPlayer);
    void RemoveLeader(Nation nation);
    void DeclareWarOn(Nation target, Nation aggressor);
    bool MakePeaceWith(Nation _nat1, Nation _nat2);
    void UpdateTurn();

private:
    enum WarStatus {AT_WAR, AT_PEACE, PERMANENT_WAR};
    typedef struct {Nation nation; int warStartedOn, timesAtWarWith; WarStatus warStat;} WarHistory;
    typedef struct {QPixmap image; QString leaderName, displayString; Nation nation; QVector<WarHistory*> warChart;} DiplomacyItem;

    GameManager *manager;
    QPushButton *declareWar;
    QPushButton *makePeace;
    QPushButton *closeDiplo;
    QLabel *leaderImage, *leaderName, *warStatus, *nationName, *atWarWith;

    QListWidget *leaderListArea;

    QVector<DiplomacyItem> diploItemList;
    Nation player;

    int turn;

signals:

public slots:
    void selectLeader(QListWidgetItem *item);
    void slot_delcareWar();
    void slot_makePeace();
};

#endif // DIPLOMACY_H
