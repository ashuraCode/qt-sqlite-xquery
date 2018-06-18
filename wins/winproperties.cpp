#include "winproperties.h"
#include "ui_winproperties.h"

WinProperties::WinProperties(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinProperties)
{
    ui->setupUi(this);
}

WinProperties::~WinProperties()
{
    delete ui;
}

void WinProperties::on_actionOtw_rz_konfiguracj_triggered()
{

}

void WinProperties::on_actionZapisz_triggered()
{

}
