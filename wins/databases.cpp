#include "databases.h"
#include "ui_databases.h"

DataBases::DataBases(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataBases)
{
    ui->setupUi(this);
}

DataBases::~DataBases()
{
    delete ui;
}
