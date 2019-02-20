#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lenth_spinBox->setMinimum(1);
    ui->errors_spinBox->setMinimum(1);
    ui->lenth_spinBox->setValue(1);
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
    QPoint img_size;
    if (ui->checkBox->isChecked());
    size_t words_number = ui->lenth_spinBox->value();
    size_t length;
    size_t errors = ui->errors_spinBox->value();
    ui->plainTextEdit->append("##############################");

    if(ui->checkBox->isChecked()) // Если строка является путем к файлу
    {

        QString path = ui->lineEdit->text();
        ui->plainTextEdit->append("FILE: " + path);
        QImage img = QImage(path);
        img_size.rx() = img.width();
        img_size.ry() = img.height();
        for(int i=0 ; i <img.height();i++)
        {
            for (int j= 0; j < img.width(); j++) // записываем каждый пиксель как 3 числа RGB  в двоичной с.с.
            {
                QColor color = img.pixelColor(i,j);
                bytes.push_back(bitset<8>(color.red()).to_string().c_str());
                bytes.push_back(bitset<8>(color.green()).to_string().c_str());
                bytes.push_back(bitset<8>(color.blue()).to_string().c_str());
            }
        }        
    }
    else // если просто текст
    {
        str = ui->lineEdit->text().toStdString();
        ui->plainTextEdit->append("Input: " + QString(str.c_str()));

        for (int i =0; i < str.size(); i++) // записываем каждый символ как двоичное число
        {
            bitset<CHAR_BIT> b(str[i]);
            data+=b.to_string();
        }
        bytes = QString(data.c_str()).toUtf8();
    }
    length = bytes.size()/words_number;
    QByteArrayList list =  encode(bytes,length);
    QVector<QPoint> errors_coord = make_error(list,length,errors);
    ui->plainTextEdit->append("Errors in :\n word id : byte id ");

    for (int i =0; i < errors_coord.size(); i++)
        ui->plainTextEdit->append(QString::number(errors_coord[i].x()) + " : " + QString::number(errors_coord[i].y()));

    QByteArrayList res_list = decode(list,length);
    QByteArray res;
    for (int i =0; i < res_list.size(); i++)
        res.push_back(res_list[i]);

    QString msg = to_str(res);
    if(ui->checkBox->isChecked())
    {
        QStringList list = ui->lineEdit->text().split('.');
        QByteArrayList colors = make_list(res,8);
        QImage img(QSize(img_size.x(),img_size.y()),QImage::Format::Format_RGB32);
        for(int i=0 ; i < img_size.y();i++)
        {
            for (int j= 0; j < img_size.x(); j++)
            {
                bitset<8> r(colors[(i* img_size.x() + j)*3].toStdString().c_str());
                bitset<8> g(colors[(i* img_size.x() + j)*3+1].toStdString().c_str());
                bitset<8> b(colors[(i* img_size.x() + j)*3+2].toStdString().c_str());
                img.setPixelColor(i,j, QColor(r.to_ulong(),g.to_ulong(),b.to_ulong()));
            }
        }

        if(img.save(list[0] + "_res." + list[1],list[1].toStdString().c_str()))
             ui->plainTextEdit->append("Result saved in same place with mark '_res' ");
    }
    else
        ui->plainTextEdit->append("Output: " + msg);

    bool decode_res = check_errors(make_list(bytes,length),errors_coord,res_list);
    if (decode_res)
        ui->plainTextEdit->append("Successfully decoded!");
    else
        ui->plainTextEdit->append("Decode failed!");

     ui->plainTextEdit->append("##############################");
     ui->lineEdit->clear();
}
