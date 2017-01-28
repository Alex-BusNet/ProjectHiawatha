#include "notificationsystem.h"
#include <QDebug>

NotificationSystem::NotificationSystem(QWidget *parent) : QListWidget(parent)
{
    QIcon capitalLost("Assets/Icons/capitolLost.png");
    QIcon unitKilled("Assets/Icons/unitKilled.png");
    QIcon populationIncreased("Assets/Icons/populationIncreased.png");
    QIcon cityStagnant("Assets/Icons/cityStagnant.png");
    QIcon cityProductionFinished("Assets/Icons/productionFinished.png");
    QIcon warDeclared("Assets/Icons/warDeclared.png");
    QIcon madePeace("Assets/Icons/madePeace.png");

    IconArray.push_back(capitalLost);
    IconArray.push_back(unitKilled);
    IconArray.push_back(populationIncreased);
    IconArray.push_back(cityStagnant);
    IconArray.push_back(cityProductionFinished);
    IconArray.push_back(warDeclared);
    IconArray.push_back(madePeace);

    this->notificationWaiting = false;

    this->setAutoFillBackground(false);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setIconSize(QSize(40, 40));
    this->setViewMode(QListWidget::IconMode);
    this->setAcceptDrops(false);
    this->setMinimumSize(50, 648);
    this->setMaximumWidth(50);

    QString nsStyle = "QListWidget { background: transparent; border-color: transparent; }";
    nsStyle += "QListView::item:hover { background: transparent; } QListView::item:selected: { background: transparent; border: 1px solid transparent}";

    this->setStyleSheet(nsStyle);
}

NotificationSystem::~NotificationSystem()
{
    qDebug() << " Notification System Dec'tor called";


    qDebug() << "   --Notification System Deconstructed";
}

void NotificationSystem::PostNotification(Notification n)
{
    this->notificationsToBePosted.enqueue(n);
    this->notificationWaiting = true;
}

void NotificationSystem::ShowNotifications()
{
    while(!notificationsToBePosted.isEmpty())
    {
        Notification n = notificationsToBePosted.dequeue();
        QListWidgetItem *ni = new QListWidgetItem(IconArray.at(n.IconIndex), QString(""), this, 0);
        ni->setToolTip(n.ToolTipMessage);
        this->addItem(ni);
    }

    this->notificationWaiting = false;
}

bool NotificationSystem::HasNotificationsWaiting()
{
    return this->notificationWaiting;
}

void NotificationSystem::removeNotification(QListWidgetItem* item)
{
    item = this->takeItem(this->currentRow());
    delete item;
}

