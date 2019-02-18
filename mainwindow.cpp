#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <bitset>
#include <string>
#include <climits>
#include <QString>
#include <fstream>
#include <QFile>
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lenth_spinBox->setMinimum(0);
    ui->errors_spinBox->setMinimum(0);
    ui->lenth_spinBox->setValue(8);
    ui->errors_spinBox->setValue(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->lineEdit->text().isEmpty())
        return;

    string str = ui->lineEdit->text().toStdString();
    ui->lineEdit->clear();

    QByteArray bytes;
    size_t lenth = ui->lenth_spinBox->value();
    size_t errors = ui->errors_spinBox->value();
    ui->plainTextEdit->append("##############################");
    if(ui->checkBox->isChecked())
    {
        QFile histogramFile(QString(str.c_str()));
        if (histogramFile.open(QIODevice::ReadOnly))
        {
            bytes = histogramFile.readAll();
            histogramFile.close();
        }

    }
    else
    {

        ui->plainTextEdit->append("Input: " + QString(str.c_str()));
        string data;

        for (int i =0; i < str.size(); i++)
        {
            bitset<CHAR_BIT> b(str[i]);
            data+=b.to_string();
        }

        ui->plainTextEdit->append("DATA: " + QString(data.c_str()));
        bytes = QString(data.c_str()).toUtf8();
    }

    QByteArrayList list =  encode(bytes,lenth);

    QVector<QPoint> errors_coord = make_error(list,lenth,errors);
    ui->plainTextEdit->append("Errors in :\n word id : byte id ");
    for (int i =0; i < errors_coord.size(); i++)
    {
        ui->plainTextEdit->append(QString::number(errors_coord[i].x()) + " : " + QString::number(errors_coord[i].y()));
    }

    QByteArrayList res_list = decode(list,lenth);
    QByteArray res;
    for (int i =0; i < res_list.size(); i++)
        res.push_back(res_list[i]);
    //qDebug ()<<"RES: " << res <<endl;
    if(ui->checkBox->isChecked())
    {
        QStringList list = QString(str.c_str()).split('.');
        QFile histogramFile(QString(list[0]+"_res."+list[1]));
        if (histogramFile.open(QIODevice::WriteOnly))
        {
            histogramFile.write(res);
            histogramFile.close();
            ui->plainTextEdit->append("Result saved in same place with mark '_res' ");
            if (res == bytes)
                ui->plainTextEdit->append("Successfully decoded!");
            else
                ui->plainTextEdit->append("Decode failed!");
        }

    }
    else
    {
        QString msg = to_str(res);
        ui->plainTextEdit->append("Output: " + msg);
        if (msg == QString(str.c_str()))
            ui->plainTextEdit->append("Successfully decoded!");
        else
            ui->plainTextEdit->append("Decode failed!");
    }
     ui->plainTextEdit->append("##############################");
}
