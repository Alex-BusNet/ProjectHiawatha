#include "citylabel.h"

CityLabel::CityLabel()
{

}

CityLabel::CityLabel(QString string)
{
    label = new QLabel(string);
}

CityLabel::CityLabel(QPixmap pixmap)
{

}

void CityLabel::mouseReleaseEvent(QMouseEvent *e)
{

}
