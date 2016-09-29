#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <QWidget>

namespace Ui {
class GameOptions;
}

class GameOptions : public QWidget
{
    Q_OBJECT

public:
    explicit GameOptions(QWidget *parent = 0);
    ~GameOptions();
    void paintEvent(QPaintEvent*e);

private:
    Ui::GameOptions *ui;
};

#endif // GAMEOPTIONS_H
