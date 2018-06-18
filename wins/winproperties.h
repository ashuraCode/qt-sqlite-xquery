#ifndef WINPROPERTIES_H
#define WINPROPERTIES_H

#include <QMainWindow>

namespace Ui {
class WinProperties;
}

class WinProperties : public QMainWindow
{
    Q_OBJECT

public:
    explicit WinProperties(QWidget *parent = 0);
    ~WinProperties();

private slots:
    void on_actionOtw_rz_konfiguracj_triggered();

    void on_actionZapisz_triggered();

private:
    Ui::WinProperties *ui;
};

#endif // WINPROPERTIES_H
