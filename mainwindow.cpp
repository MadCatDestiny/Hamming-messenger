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

    ui->lenth_spinBox->setMaximum(100000);
    ui->errors_spinBox->setMaximum(100000);

    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_5->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_5->setVisible(false);
    if (ui->lineEdit->text().isEmpty())
        return;

    QByteArray bytes;
    QPoint img_size;
    if (ui->checkBox->isChecked());
    size_t words_number = ui->lenth_spinBox->value();

    size_t errors = ui->errors_spinBox->value();
    ui->plainTextEdit->append("##############################");

    if(ui->checkBox->isChecked()) // Если строка является путем к файлу
    {
        QString path = ui->lineEdit->text();
        if (!check_path(path))
            return;

        bytes = img_to_bits(img_size,path);
        if (bytes == nullptr)
            return;
    }
    else // если просто текст
    {
        QString msg = ui->lineEdit->text();
        bytes = str_to_bits(msg);
    }
    size_t length = bytes.size()/words_number;
    if (bytes.size()%words_number != 0)
        length++;

    QByteArrayList list =  encode(bytes,length);

    QVector<QPoint> errors_coord = make_error(list,length,errors);
    QByteArrayList list_with_errors;
    for(int i =0; i < list.size(); i++)
        list_with_errors.push_back(delete_control_bits(list[i],count_of_controls_bits(length)));

    QByteArray res_with_errors = list_to_arr(list_with_errors);
    if(ui->checkBox->isChecked())
    {
        QImage img = bits_to_img(res_with_errors,img_size);
        QStringList path = ui->lineEdit->text().split('.');
        if(img.save(path[0] + "_err." + path[1],path[1].toStdString().c_str()))
            ui->plainTextEdit->append("Result with errors saved in same place with mark '_err' ");
        ui->label_4->setVisible(true);
        ui->label_4->setPixmap(QPixmap::fromImage(img));
    }
    else {
        QString msg = to_str(res_with_errors);
        ui->plainTextEdit->append("With errors : " + msg);
    }

    if(ui->show_errors->isChecked())
    {
        ui->plainTextEdit->append("Errors in :\n word id : byte id ");
        for (int i =0; i < errors_coord.size(); i++)
            ui->plainTextEdit->append(QString::number(errors_coord[i].x()) + " : " + QString::number(errors_coord[i].y()));
    }

    QByteArrayList res_list = decode(list,length);
    QByteArray res = list_to_arr(res_list);

    if(ui->checkBox->isChecked())
    {
        QImage img = bits_to_img(res,img_size);
        QStringList path = ui->lineEdit->text().split('.');
        if(img.save(path[0] + "_res." + path[1],path[1].toStdString().c_str()))
            ui->plainTextEdit->append("Result saved in same place with mark '_res' ");
        ui->label_5->setVisible(true);
        ui->label_5->setPixmap(QPixmap::fromImage(img));
    }
    else
    {
        QString msg = to_str(res);
        ui->plainTextEdit->append("Output: " + msg);
    }

    bool decode_res = check_errors(make_list(bytes,length),errors_coord,res_list);
    if (decode_res)
        ui->plainTextEdit->append("Successfully decoded!");
    else
        ui->plainTextEdit->append("Decode failed!");

     ui->plainTextEdit->append("##############################");
     ui->lineEdit->clear();
}

QByteArray MainWindow::img_to_bits(QPoint &img_size,QString path)
{
    ui->plainTextEdit->append("FILE: " + path);
    QImage img = QImage(path);
    img_size.rx() = img.width();
    img_size.ry() = img.height();
    QByteArray bytes;

    ui->label_3->setVisible(true);
    ui->label_3->setPixmap(QPixmap::fromImage(img));
    for(int i=0 ; i <img.width();i++)
    {
        for (int j= 0; j < img.height(); j++) // записываем каждый пиксель как 3 числа RGB  в двоичной с.с.
        {
            QColor color = img.pixelColor(i,j);
            bytes.push_back(bitset<8>(color.red()).to_string().c_str());
            bytes.push_back(bitset<8>(color.green()).to_string().c_str());
            bytes.push_back(bitset<8>(color.blue()).to_string().c_str());
        }
    }
    return bytes;
}

QByteArray MainWindow::str_to_bits(QString msg)
{
    QByteArray bytes;
    string str;
    string data;
    str = msg.toStdString();
    ui->plainTextEdit->append("Input: " + QString(str.c_str()));

    for (int i =0; i < str.size(); i++) // записываем каждый символ как двоичное число
    {
        bitset<CHAR_BIT> b(str[i]);
        data+=b.to_string();
    }
    bytes = QString(data.c_str()).toUtf8();
    return bytes;
}

QImage MainWindow::bits_to_img(QByteArray res, QPoint &img_size)
{
    QByteArrayList colors = make_list(res,8);
    QImage img(QSize(img_size.x(),img_size.y()),QImage::Format::Format_RGB32);
    for(int i=0 ; i < img_size.y();i++)
    {
        for (int j= 0; j < img_size.x(); j++)
        {
            bitset<8> r(colors[(i* img_size.x() + j)*3].toStdString().c_str());
            bitset<8> g(colors[(i* img_size.x() + j)*3+1].toStdString().c_str());
            bitset<8> b(colors[(i* img_size.x() + j)*3+2].toStdString().c_str());
            img.setPixelColor(j,i, QColor(r.to_ulong(),g.to_ulong(),b.to_ulong()));
        }
    }

    return img;
}

QByteArray MainWindow::list_to_arr(QByteArrayList &res_list)
{
    QByteArray res;
    for (int i =0; i < res_list.size(); i++)
        res.push_back(res_list[i]);
    return  res;
}

bool MainWindow::check_path(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        ui->plainTextEdit->append("File couldn't be open");
        ui->plainTextEdit->append("##############################");
        return false;
    }

    file.close();
    return true;
}
