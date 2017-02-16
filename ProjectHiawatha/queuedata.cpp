#include "queuedata.h"

QueueData::QueueData()
{
}

void QueueData::enqueue(CityProdData cpd)
{
    cityUpdate.enqueue(cpd);
}

CityProdData QueueData::dequeue()
{
    return cityUpdate.dequeue();
}

bool QueueData::isEmpty()
{
    return cityUpdate.isEmpty();
}
