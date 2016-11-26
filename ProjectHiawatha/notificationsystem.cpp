#include "notificationsystem.h"

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

    this->setAutoFillBackground(false);
    this->setIconSize(QSize(25, 25));
    this->setViewMode(QListWidget::IconMode);
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(removeNotification()));
}

void NotificationSystem::PostNotification(Notification n)
{
    this->notificationsToBePosted.enqueue(n);
}

void NotificationSystem::ShowNotifications()
{
    int rows = 0;
    while(!notificationsToBePosted.isEmpty())
    {
        Notification n = notificationsToBePosted.dequeue();
        this->insertItem(rows, new QListWidgetItem(IconArray.at(n.IconIndex), QString(" "), this, 0));
        this->item(rows)->setToolTip(n.ToolTipMessage);
        rows++;
    }
}

bool NotificationSystem::HasNotificationsWaiting()
{
    return !this->notificationsToBePosted.isEmpty();
}

void NotificationSystem::removeNotification()
{
    this->removeItemWidget(this->item(this->currentRow()));
}

