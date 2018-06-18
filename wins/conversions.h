#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <QtCore>
#include <QtWidgets>
#include <QMainWindow>
#include <QtSql>

namespace Ui {
class Conversions;
}

class Conversions : public QMainWindow
{
    Q_OBJECT

public:
    explicit Conversions(QSqlDatabase &db, QWidget *parent = 0);
    ~Conversions();

private slots:
    void on_actionOtw_rz_triggered();
    void on_pushButton_clicked();
    void chooseCoding(QListWidgetItem *);

    void on_actionZapisz_jako_sql_triggered();
//    void updateBase();

private:
    enum Coding {
        Unicode = 1,
        Windows_1250
    };

    Ui::Conversions *ui;
    QByteArray m_loaded;
    Coding m_coding;
    QListWidget *m_kodowanie;
    QMessageBox *m_dialog;
    QSqlDatabase &m_sqlDataBase;

};

#endif // CONVERSIONS_H
