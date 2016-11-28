#include "notificationsystem.h"
#include <QDebug>

NotificationSystem::NotificationSystem(QWidget *parent) : QListWidget(parent)
{
    QIcon capitolLost("../ProjectHiawatha/Assets/Icons/capitolLost.png");
    QIcon unitKilled("../ProjectHiawatha/Assets/Icons/unitKilled.png");
    QIcon populationIncreased("../ProjectHiawatha/Assets/Icons/populationIncreased.png");
    QIcon cityStagnant("../ProjectHiawatha/Assets/Icons/cityStagnant.png");
    QIcon cityProductionFinished("../ProjectHiawatha/Assets/Icons/productionFinished.png");
    QIcon warDeclared("../ProjectHiawatha/Assets/Icons/warDeclared.png");

    IconArray.push_back(capitolLost);
    IconArray.push_back(unitKilled);
    IconArray.push_back(populationIncreased);
    IconArray.push_back(cityStagnant);
    IconArray.push_back(cityProductionFinished);
    IconArray.push_back(warDeclared);

    this->notificationWaiting = false;

    this->setAutoFillBackground(false);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setIconSize(QSize(40, 40));
    this->setViewMode(QListWidget::IconMode);
    this->setMinimumSize(50, 648);
    this->setMaximumWidth(50);
    this->setStyleSheet("QListWidget { background: transparent; border-color: transparent; }");
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
        QListWidgetItem *ni = new QListWidgetItem(IconArray.at(n.IconIndex), QString(" "), this, 0);
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
    this->takeItem(this->currentRow());
}

