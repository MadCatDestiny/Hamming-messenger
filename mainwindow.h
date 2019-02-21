#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <bitset>
#include <string>
#include <climits>
#include <QString>
#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QRgb>
#include <QPicture>
#include "hamming.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    QByteArray img_to_bits(QPoint &img_size,QString path);
    QByteArray str_to_bits(QString msg);
    QImage bits_to_img(QByteArray res, QPoint& img_size);
    QByteArray list_to_arr(QByteArrayList &res_list);
    bool check_path(QString path);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
