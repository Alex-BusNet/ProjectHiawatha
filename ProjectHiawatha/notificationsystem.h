#ifndef NOTIFICATIONSYSTEM_H
#define NOTIFICATIONSYSTEM_H

#include <QQueue>
#include <QVector>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include "datatypes.h"

class NotificationSystem : public QListWidget
{
    Q_OBJECT
public:
    explicit NotificationSystem(QWidget *parent = 0);
    ~NotificationSystem();

    void PostNotification(Notification n);
    void ShowNotifications();
    bool HasNotificationsWaiting();

private:
    QQueue<Notification> notificationsToBePosted;

    QVector<QIcon> IconArray;
    bool notificationWaiting;
signals:

public slots:
    void removeNotification(QListWidgetItem *item);
};

#endif // NOTIFICATIONSYSTEM_H
