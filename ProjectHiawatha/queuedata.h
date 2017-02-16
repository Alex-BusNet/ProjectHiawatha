#ifndef QUEUEDATA_H
#define QUEUEDATA_H
#include "datatypes.h"
#include <QQueue>

class QueueData
{
public:
    QueueData();

    static void enqueue(CityProdData cpd);
    static CityProdData dequeue();
    static bool isEmpty();

private:
};

static QQueue<CityProdData> cityUpdate;
#endif // QUEUEDATA_H
