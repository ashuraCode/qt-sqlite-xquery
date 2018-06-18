#ifndef ASHURAWIND_H
#define ASHURAWIND_H

#include <QtCore>
#include <QtWidgets>
#include <QMainWindow>
#include <QtSql>

namespace Ui {
class AshuraWind;
}

class AshuraWind : public QMainWindow
{
    Q_OBJECT

public:
    explicit AshuraWind(QWidget *parent = 0);
    ~AshuraWind();

protected:
    virtual void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void changeEvent(QEvent *event);

private slots:
    void on_actionKonwertery_sql_triggered();
    void on_actionBazy_danych_triggered();
    void on_actionOtw_rz_triggered();
    void on_actionStatystyk_triggered();
    void on_actionOkna_triggered();
    void on_actionInformacje_triggered();
    void on_actionO_mnie_triggered();
    void setTransparent(bool b);
    void resetActive();

    void on_actionOdwr_kolorystyke_triggered();

    void on_actionJu_wiem_triggered();

private:
    Ui::AshuraWind *ui;
    QSqlDatabase m_sqlDataBase;
    quint32 m_numberOfRows;
    QPoint m_dragPosition;
    bool m_blackWhite;
    bool m_activeWindow;

};

#endif // ASHURAWIND_H
