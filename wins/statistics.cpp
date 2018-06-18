#include "statistics.h"
#include "ui_statistics.h"

Statistics::Statistics(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Statistics)
{
    ui->setupUi(this);
}

Statistics::~Statistics()
{
    delete ui;
}
