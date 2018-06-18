#include "conversions.h"
#include "ui_conversions.h"
#include <QtXmlPatterns>
#include <QtConcurrent>


Conversions::Conversions(QSqlDatabase &db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Conversions),
    m_sqlDataBase(db)
{
    ui->setupUi(this);
    m_coding = Unicode;

    m_dialog = new QMessageBox(QMessageBox::Question, QStringLiteral("Wybór kodowania"), QString(""), QMessageBox::Ok | QMessageBox::Close);
    m_dialog->setIcon(QMessageBox::Question);
    m_dialog->setFixedSize(200, 400);

    m_kodowanie = new QListWidget(m_dialog);
    m_kodowanie->addItems(QStringList {"Unicode", "Windows_1250"});
    connect(m_kodowanie, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(chooseCoding(QListWidgetItem*)));
    m_kodowanie->setFixedSize(200, 50);

    QBoxLayout *lay = qobject_cast<QBoxLayout*>(m_dialog->layout());
    if (lay)
        lay->insertWidget(0, m_kodowanie);
}

Conversions::~Conversions()
{
    delete ui;
}

void Conversions::on_actionOtw_rz_triggered()
{
    m_dialog->show();
    m_dialog->exec();
    m_dialog->hide();

    if (m_dialog->result()!=QMessageBox::Ok) {
        m_coding = (Coding)0;
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Otwórz plik"), QDir::currentPath(), tr("Pliki xml, txt (*.xml *.txt)"));;

    if (!fileName.isEmpty()) {
        QFile file (fileName);
        if (file.open(QFile::ReadOnly)) {
            m_loaded = file.readAll();
            file.close();
            QTextDecoder* decoder;
            switch (m_coding) {
            case Unicode:
                ui->inFileText->setPlainText(m_loaded);
                break;
            case Windows_1250:
                decoder = QTextCodec::codecForName("Windows-1250")->makeDecoder();
                ui->inFileText->setPlainText(decoder->toUnicode(m_loaded));
                break;
            default:
                ui->inFileText->setPlainText(m_loaded);
                break;
            }

        } else {
            QMessageBox::warning(NULL, "Warning", QStringLiteral("Nie udało się wczytać pliku!"));
        }
    }
}

void translate(QByteArray &string)
{
      int itx = 0;
      int num;
      int numlen;

      while (-1!=(itx=string.indexOf('\\', itx)))
      {
            numlen=1;
            switch (string[itx+1])
            {
            case '\\':
                  break;

            case 'r':
                  string[itx] = '\r';
                  break;

            case 'n':
                  string[itx] = '\n';
                  break;

            case 't':
                  string[itx] = '\t';
                  break;

            case 'v':
                  string[itx] = '\v';
                  break;

            case 'a':
                  string[itx] = '\a';
                  break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                  numlen = sscanf(string.constData()+itx+1,"%o",&num);
                  string[itx] = (char)num;
                  break;

            case 'x':
                  numlen = sscanf(string.constData()+itx+1,"%x",&num);
                  string[itx] = (char) num;
                  break;
            }
            itx++;
            string.remove(itx, numlen);
      }
}

void Conversions::on_pushButton_clicked()
{
    QString output;
    if (ui->xquery->checkState() == Qt::Checked) {
        QBuffer sourceDocument(&m_loaded);
        sourceDocument.open(QIODevice::ReadOnly);

        QByteArray outArray;
        QBuffer buffer(&outArray);
        buffer.open(QIODevice::ReadWrite);

        QXmlQuery query;
        QAbstractMessageHandler * msg = query.messageHandler();
        query.bindVariable("inputDocument", &sourceDocument);
        query.setQuery(ui->xqueryRecipe->toPlainText());
        if (!query.isValid()) {
            ui->output->setPlainText("Query not valid");
            return;
        }

        QXmlFormatter formatter(query, &buffer);
        if (!query.evaluateTo(&formatter)) {
            ui->output->setPlainText("Evaluation error");
            return;
        }

        output = QString::fromUtf8(outArray.constData());
        ui->output->setPlainText(output);
    }
    if (ui->regex->checkState() == Qt::Checked) {
        // (?<key>[\\w]+)(?<value>[\\w ]*)[\\r\\n]+
        if (output.isEmpty()) {

            QTextDecoder* decoder;
            switch (m_coding) {
            case Unicode:
                output = m_loaded;
                break;
            case Windows_1250:
                decoder = QTextCodec::codecForName("Windows-1250")->makeDecoder();
                output = decoder->toUnicode(m_loaded);
                break;
            default:
                output = m_loaded;
                break;
            }
        }
        QByteArray array = ui->regexRecipe->text().toLocal8Bit();
        translate(array);
        QRegularExpression re(array);
        re.setPatternOptions(QRegularExpression::DotMatchesEverythingOption | QRegularExpression::UseUnicodePropertiesOption | re.patternOptions());
        QRegularExpressionMatchIterator i = re.globalMatch(output);

        ui->afterFiltering->setRowCount(0);
        int row = 0;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString key = match.captured(1);
            QString value = match.captured(2);
            ui->afterFiltering->insertRow(ui->afterFiltering->rowCount());
            ui->afterFiltering->setItem(row, 0, new QTableWidgetItem(key));
            ui->afterFiltering->setItem(row, 1, new QTableWidgetItem(value));
            row++;
        }
    }
}

void Conversions::chooseCoding(QListWidgetItem *coding)
{
    m_coding = (Coding)(m_kodowanie->row(coding)+1);
}

void updateBase(QSqlDatabase *m_sqlDataBase, QTableWidget *afterFiltering, QPlainTextEdit *output)
{
    QSqlQuery query;
    for (int row=0; row<afterFiltering->rowCount(); row++) {
        query.exec(QString("INSERT INTO WORDSLANG VALUES (NULL, '%1', '%2')").arg(afterFiltering->item(row,0)->text()).arg(afterFiltering->item(row,1)->text()));
        output->setPlainText(QString::number(((double)row)/afterFiltering->rowCount()*100)+"%");
    }
    m_sqlDataBase->commit();
}

void Conversions::on_actionZapisz_jako_sql_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("Zapisz plik"), QDir::currentPath(), tr("Pliki sqlite (*.sqlite)"));;

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
            bool ret = query.exec("create table WORDSLANG ("
                             "id integer primary key,"
                             "KLUCZ          varchar(80),"
                             "WARTOSC        varchar(80))");
            qDebug() << ret;
            m_sqlDataBase.commit();

            QString bigQuery = "INSERT INTO WORDSLANG VALUES ";
            for (int row=0; row<ui->afterFiltering->rowCount(); row++) {
                qDebug() << row;
                bigQuery += QString("(NULL, '%1', '%2')").arg(ui->afterFiltering->item(row,0)->text()).arg(ui->afterFiltering->item(row,1)->text());
                if ((row+1)!=ui->afterFiltering->rowCount())
                    bigQuery += ",";
//                query.exec(QString("INSERT INTO WORDSLANG VALUES (NULL, '%1', '%2')").arg(ui->afterFiltering->item(row,0)->text()).arg(ui->afterFiltering->item(row,1)->text()));
//                ui->output->setPlainText(QString::number(((double)row)/ui->afterFiltering->rowCount()*100)+"%");
            }
            query.exec(bigQuery);
            m_sqlDataBase.commit();
        }
    }
}

