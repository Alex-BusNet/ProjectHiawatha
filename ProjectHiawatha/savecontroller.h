#ifndef SAVECONTROLLER_H
#define SAVECONTROLLER_H
#include <QJsonDocument>
#include <QJsonObject>

class SaveController
{
public:
    SaveController();
    ~SaveController();

    void LoadFile(QString file);
private:
    QJsonObject *SaveObject;
    QJsonDocument *SaveDoc;

};

#endif // SAVECONTROLLER_H
