#pragma once
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <cmath>
#include <bitset>
#include <ctime>
#include <cstdlib>
#include <QTime>
#include "hamming.h"
using namespace std;

QByteArrayList encode(QByteArray data,size_t lenth);

void put_control_bits(QByteArray &word,const size_t lenth);

QByteArrayList create_transition_matrix(const size_t word_size,const size_t count_of_control_bits);

void show_matrix(const QByteArrayList & mtr);

QByteArray get_row(const QByteArrayList matrix,const size_t number);

size_t scalar(QByteArray x,QByteArray y);

void make_error(QByteArrayList & data,size_t lenth,size_t errors);

QByteArrayList decode(QByteArrayList data,size_t lenth);

void delete_control_bits(QByteArray & word, size_t count_of_control_bits);

QString to_str(const QByteArray & data);
