#ifndef DATABASES_H
#define DATABASES_H

#include <QMainWindow>

namespace Ui {
class DataBases;
}

class DataBases : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataBases(QWidget *parent = 0);
    ~DataBases();

private:
    Ui::DataBases *ui;
};

#endif // DATABASES_H
