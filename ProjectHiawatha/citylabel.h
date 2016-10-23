#ifndef CITYLABEL_H
#define CITYLABEL_H

#include <QLabel>
#include <QString>
#include <QPixmap>

class CityLabel : public QLabel
{
public:
    CityLabel();
    CityLabel(QString string);
    CityLabel(QPixmap pixmap);

private:
    QLabel *label;
    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // CITYLABEL_H
