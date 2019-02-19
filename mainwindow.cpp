#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    QByteArray bytes;
    string str;
    string data;
    size_t lenth = ui->lenth_spinBox->value();
    size_t errors = ui->errors_spinBox->value();
    ui->plainTextEdit->append("##############################");
    if(ui->checkBox->isChecked())
    {
        QString path = ui->lineEdit->text();
        ui->plainTextEdit->append("FILE: " + path);
        QImage img = QImage(path);
        QBuffer buffer(&bytes);
        qDebug() << bytes << endl;
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, path.split('.')[1].toStdString().c_str());
        str = bytes.toStdString();
        bytes.clear();
        //

        /*QFile histogramFile(path);
        if (histogramFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {

            QDataStream stream (&histogramFile);
            stream >>img;
            QByteArray buf = histogramFile.readAll();
            str = buf.toStdString();
            histogramFile.close();            
        }
        else
        {
            ui->plainTextEdit->append("file couldn't be open");
            ui->plainTextEdit->append("##############################");
            return;
        }*/
    }
    else
    {
        str = ui->lineEdit->text().toStdString();
        ui->plainTextEdit->append("Input: " + QString(str.c_str()));

    }
        for (int i =0; i < str.size(); i++)
        {
            bitset<CHAR_BIT> b(str[i]);
            data+=b.to_string();
        }
       // ui->plainTextEdit->append("DATA: " + QString(data.c_str()));
        bytes = QString(data.c_str()).toUtf8();


    QByteArrayList list =  encode(bytes,lenth);

    QVector<QPoint> errors_coord = make_error(list,lenth,errors);
    ui->plainTextEdit->append("Errors in :\n word id : byte id ");

    for (int i =0; i < errors_coord.size(); i++)
        ui->plainTextEdit->append(QString::number(errors_coord[i].x()) + " : " + QString::number(errors_coord[i].y()));

    QByteArrayList res_list = decode(list,lenth);
    QByteArray res;
    for (int i =0; i < res_list.size(); i++)
        res.push_back(res_list[i]);

    QString msg = to_str(res);
    qDebug() <<msg <<endl;
    if(ui->checkBox->isChecked())
    {
        QStringList list = ui->lineEdit->text().split('.');

        /*QImage img;
        img.fromData(QByteArray(msg.toStdString().c_str()));


        if(img.save("_res"))
             ui->plainTextEdit->append("Result saved in same place with mark '_res' ");
        else
            qDebug() <<"ERROR"<<endl;
        qDebug() << list[0]<<endl;*/

        QFile histogramFile(list[0] + "_res." + list[1]);
        if (histogramFile.open(QIODevice::ReadWrite))
        {
            histogramFile.write(msg.toStdString().c_str());
            histogramFile.close();
            ui->plainTextEdit->append("Result saved in same place with mark '_res' ");
        }
    }
    else
        ui->plainTextEdit->append("Output: " + msg);

    bool decode_res = check_errors(make_list(bytes,lenth),errors_coord,res_list);

    if (decode_res)
        ui->plainTextEdit->append("Successfully decoded!");
    else
        ui->plainTextEdit->append("Decode failed!");

     ui->plainTextEdit->append("##############################");
     ui->lineEdit->clear();
}
