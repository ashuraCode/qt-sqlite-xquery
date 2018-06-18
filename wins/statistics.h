#ifndef STATISTICS_H
#define STATISTICS_H

#include <QMainWindow>

namespace Ui {
class Statistics;
}

class Statistics : public QMainWindow
{
    Q_OBJECT

public:
    explicit Statistics(QWidget *parent = 0);
    ~Statistics();

private:
    Ui::Statistics *ui;
};

#endif // STATISTICS_H
