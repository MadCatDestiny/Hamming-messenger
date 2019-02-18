#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <bitset>
#include <string>
#include <climits>
#include <QString>
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
    ui->plainTextEdit->append("##############################");
    ui->plainTextEdit->append("Input: " + QString(str.c_str()));
    size_t lenth = ui->lenth_spinBox->value();
    size_t errors = ui->errors_spinBox->value();
   // qDebug() << "STR :" << QString(str.c_str())  << endl;
    string data;
    for (int i =0; i < str.size(); i++)
    {
        bitset<CHAR_BIT> b(str[i]);
        //qDebug () << "BITS IN STR :" << QString(b.to_string().c_str()) <<endl;
        data+=b.to_string();
    }
   // qDebug() << "DATA: " << QString(data.c_str()) <<endl;
    QByteArrayList list =  encode(QByteArray(QString(data.c_str()).toUtf8()),lenth);
    make_error(list,lenth,errors);

    QByteArrayList res_list = decode(list,lenth);
    QByteArray res;
    for (int i =0; i < res_list.size(); i++)
        res.push_back(res_list[i]);
   // qDebug ()<<"RES: " << res <<endl;
    QString msg = to_str(res);
    //qDebug()<<"MSG: " << msg <<endl;
    ui->plainTextEdit->append("Output: " + msg);
    ui->plainTextEdit->append("##############################");
}
