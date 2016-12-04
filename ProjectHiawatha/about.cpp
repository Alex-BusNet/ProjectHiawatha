#include "about.h"
#include "ui_about.h"
#include "QDesktopWidget"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    QString AboutStyle = "QWidget { background-color: #145e88; } QFrame { background-color: black; }";
    AboutStyle += "QPushButton { background-color: #4899C8; border: 1px solid black; border-radius: 6px; font: 10px; min-width: 100px;}";
    AboutStyle += "QPushButton:pressed { background-color: #77adcb; }";
    AboutStyle += "QLabel { background-color: transparent; color: white; }";

    this->setStyleSheet(AboutStyle);
    QDesktopWidget widget;
    QRect rect = widget.screenGeometry(widget.primaryScreen());
    this->setGeometry((rect.width() / 2) - 200, (rect.height() / 2) - 290, this->width(), this->height());
}

About::~About()
{
    delete ui;
}
