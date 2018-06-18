#include "ashurawind.h"
#include "ui_ashurawind.h"
#include "wins/conversions.h"
#include "wins/databases.h"
#include "wins/statistics.h"
#include "wins/winproperties.h"


AshuraWind::AshuraWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AshuraWind),
    m_numberOfRows(0)
      ,   m_blackWhite(0)
{
    ui->setupUi(this);
    m_activeWindow = true;

    QDir::setCurrent(qApp->applicationDirPath());
    qsrand(QTime::currentTime().msec());

    ui->menuBar->installEventFilter(this);

    QRect displayGeometry = QApplication::desktop()->screenGeometry();
    QPoint topRightPos = displayGeometry.topRight();
    move(topRightPos.x() - width() - 3, topRightPos.y() + 475);

    if (QSqlDatabase::isDriverAvailable("QSQLITE")) {
        m_sqlDataBase = QSqlDatabase::addDatabase("QSQLITE");
    } else {
        QMessageBox::critical(NULL, "Error", QStringLiteral("Sterownik bazy danych QSqlLite jest nie dostępny!"));
        exit(1);
    }

    startTimer(10 * 1000);
}

AshuraWind::~AshuraWind()
{
    delete ui;
}

void AshuraWind::timerEvent(QTimerEvent *event)
{
    if (m_numberOfRows) {
        if (m_sqlDataBase.isOpen()) {
            QSqlQuery query;
            quint32 randomized = (qrand()<<15)+qrand();
            query.exec(QString("SELECT KLUCZ,WARTOSC FROM WORDSLANG where ID = %1").arg(randomized%m_numberOfRows));
            if (query.next()) {
                ui->qpl->setText(query.value(0).toString());
                ui->qen->setText(query.value(1).toString());
            }
        }
    }
}

bool AshuraWind::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->menuBar) {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                m_dragPosition = mouse_event->globalPos() - frameGeometry().topLeft();
                return false;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                move(mouse_event->globalPos() - m_dragPosition);
                return false;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void AshuraWind::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if(this->isActiveWindow())
        {
            if (m_activeWindow) {
                setWindowFlags( Qt::FramelessWindowHint );
                show();
                activateWindow();
                m_activeWindow = 0;
                QTimer::singleShot(100, this, SLOT(resetActive()));
                setTransparent(false);
            }
        } else {
            if (m_activeWindow) {
                setWindowFlags( Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint );
                show();
                m_activeWindow = 0;
                QTimer::singleShot(100, this, SLOT(resetActive()));
                setTransparent(true);
            }
        }
    }

    QMainWindow::changeEvent(event);
}

void AshuraWind::on_actionKonwertery_sql_triggered()
{
    Conversions *conversions = new Conversions(m_sqlDataBase);
    conversions->setAttribute(Qt::WA_DeleteOnClose);
    conversions->show();
}

void AshuraWind::on_actionOtw_rz_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Otwórz bazę SqlLite"), QDir::currentPath(), tr("Pliki sqlite (*.sqlite)"));;

    if (!fileName.isEmpty()) {
        m_sqlDataBase.setDatabaseName( fileName );
        if (!m_sqlDataBase.open()) {
            QMessageBox::critical(NULL, "Critical", m_sqlDataBase.lastError().text());
        }
        if (m_sqlDataBase.isOpenError()) {
            QMessageBox::warning(NULL, "Warning", QStringLiteral("Wystąpił błąd podczas otwarcia!"));
            m_sqlDataBase.close();
        }
        if (!m_sqlDataBase.isValid()) {
            QMessageBox::warning(NULL, "Warning", QStringLiteral("Plik z bazą danych jest nie poprawny!"));
            m_sqlDataBase.close();
        }

        if (m_sqlDataBase.isOpen()) {
            QSqlQuery query;
            query.exec("SELECT COUNT(*) FROM WORDSLANG");
            if (query.next()) {
                m_numberOfRows = query.value(0).toInt();
            }
            timerEvent(NULL);
        }
    }
}

void AshuraWind::on_actionBazy_danych_triggered()
{
    DataBases *databases = new DataBases;
    databases->setAttribute(Qt::WA_DeleteOnClose);
    databases->show();
}

void AshuraWind::on_actionStatystyk_triggered()
{
    Statistics *statistics = new Statistics;
    statistics->setAttribute(Qt::WA_DeleteOnClose);
    statistics->show();
}

void AshuraWind::on_actionOkna_triggered()
{
    WinProperties *winproperties = new WinProperties;
    winproperties->setAttribute(Qt::WA_DeleteOnClose);
    winproperties->show();
}

void AshuraWind::on_actionInformacje_triggered()
{

}

void AshuraWind::on_actionO_mnie_triggered()
{
    QMessageBox::information(NULL, "O mnie", QStringLiteral("Autor: AshuraCode"));
}

#define BLACK_WHITE ((m_blackWhite) ? (0x00ffffff) : (0))
#define REVERSED_BLACK_WHITE ((m_blackWhite) ? (0) : (0x00ffffff))
#define REVERSED_BLACK_WHITE_THRESHOLD ((m_blackWhite) ? (0x00121212) : (0x00ededed))
void AshuraWind::setTransparent(bool b)
{
    if(!b) {
        ui->qpl->setStyleSheet(QString("color: #%1; background-color:#%2").arg(BLACK_WHITE, 6, 16, QChar('0')).arg(REVERSED_BLACK_WHITE, 6, 16, QChar('0')));
        ui->qen->setStyleSheet(QString("color: #%1; background-color:#%2").arg(BLACK_WHITE, 6, 16, QChar('0')).arg(REVERSED_BLACK_WHITE, 6, 16, QChar('0')));
    } else {
        ui->qpl->setStyleSheet(QString("color: #%1; background-color: transparent;").arg(BLACK_WHITE, 6, 16, QChar('0')));
        ui->qen->setStyleSheet(QString("color: #%1; background-color: transparent;").arg(BLACK_WHITE, 6, 16, QChar('0')));
    }
}
void AshuraWind::resetActive()
{
    m_activeWindow = 1;
}

void AshuraWind::on_actionOdwr_kolorystyke_triggered()
{
    m_blackWhite = !m_blackWhite;
    setTransparent(false);
}

void AshuraWind::on_actionJu_wiem_triggered()
{

}
