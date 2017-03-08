#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include <QJsonObject>
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
    ~Diplomacy();

    void AddLeader(QString _name, QPixmap _image, Nation _nation, bool isPlayer);
    void RemoveLeader(Nation nation);
    void DeclareWarOn(Nation target, int targetIndex, Nation aggressor, int aggressorIndex);

    void UpdateTurn();
    void UpdateLeader(int row);
    void MeetPlayer(int row);
    void SetLeaderImage(int index, QPixmap &image);
    void WriteDiploSaveData(QJsonObject &obj) const;
    void ReadDiploSaveData(const QJsonObject &obj);

    int GetLengthOfWar(Nation ai);
    int GetNumberOfWars(Nation ai);
    int GetIndex();

    bool MakePeaceWith(Nation player, int targetIndex, Nation ai);
    bool AtPermanentWar(Nation ai);

    QPushButton *declareWar;
    QPushButton *makePeace;

private:
    enum WarStatus {SELF, AT_WAR, AT_PEACE, PERMANENT_WAR};
    typedef struct {Nation nation; int warStartedOn, timesAtWarWith; WarStatus warStat; } WarHistory;
    typedef struct {QPixmap image; QString leaderName, displayString; Nation nation; QVector<WarHistory> warChart; bool hasMetPlayer;} DiplomacyItem;

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
