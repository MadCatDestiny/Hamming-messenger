#include "hamming.h"


QByteArrayList encode(QByteArray data,const size_t lenth)
{ 
    QByteArrayList list = make_list(data,lenth);

    for (int i =0; i< list.size(); i++) //вставляем контрольные биты
        put_control_bits(list[i],lenth);

    return list;
}

void put_control_bits(QByteArray &word,const size_t lenth)
{
    size_t count_of_control_bits = count_of_controls_bits(lenth);

    for(int i =0; i < count_of_control_bits; i++)
        word.insert(pow(2,i)-1,'0');

    QByteArrayList matrix = create_transition_matrix(word.size(),count_of_control_bits);

    for(int i =0; i < count_of_control_bits; i++)
    {
        QByteArray row = get_row(matrix,i);
        size_t control_bit = scalar(row,word);
        control_bit = control_bit%2;
        if (control_bit)
            word[(unsigned int)pow(2,i)-1] = '1';
        else
            word[(unsigned int)pow(2,i)-1] = '0';
    }
}

QByteArrayList create_transition_matrix(const size_t word_size,const size_t count_of_control_bits)
{
    QByteArrayList matrix;
    matrix.reserve(word_size);
    for (int i =0; i < word_size; i++)
    {
        QByteArray column;
        bitset<1024> b(i+1);
        QString bits =b.to_string().c_str();

        for (int j =0; j < count_of_control_bits; j++)
        {
            QByteArray bit = QByteArray(QString(bits[1023-j]).toUtf8());
            column.push_back(bit);
        }
        matrix.push_back(column);
    }

    return matrix;
}

void show_matrix(const QByteArrayList & mtr)
{
    for (int i =0; i < mtr.size(); i++)
    {
        qDebug() << mtr[i] <<endl;
    }
}

QByteArray get_row(const QByteArrayList matrix,const size_t number)
{
    QByteArray row;
    for (int i = 0; i < matrix.size(); i++)
        row.push_back(matrix[i][number]);

    return row;
}

size_t scalar(QByteArray x,QByteArray y)
{
    QVector<char> xv;
    QVector<char> yv;
    for (int i =0; i < x.size(); i++)
    {
        xv.push_back(x[i]);
        yv.push_back(y[i]);
    }

    size_t res = 0;
    for (int i =0; i < x.size(); i++)
        res+=QString(xv[i]).toInt()* QString(yv[i]).toInt();
    return res;
}

QVector<QPoint> make_error(QByteArrayList &data, size_t lenth, size_t errors)
{
    srand(time_t(0));
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    QVector<QPoint> errors_coord;

    for (int i = 0; i < errors; i++)
    {

        size_t pos = qrand()%lenth*data.size();
        size_t word_id = pos/lenth;
        size_t byte_id =pos%lenth;
        errors_coord.push_back(QPoint(word_id,byte_id));

        if (data[word_id][byte_id] == '0')
            data[word_id][byte_id] = '1';
        else
            data[word_id][byte_id] = '0';
    }

    return errors_coord;
}

QByteArrayList decode(QByteArrayList data, size_t lenth)
{
    size_t count_of_control_bits =count_of_controls_bits(lenth);
    QByteArrayList matrix = create_transition_matrix(data[0].size(),count_of_control_bits);

    for (int i =0; i < data.size(); i++)
    {
        bool error_in_word = false;
        size_t id_wrong_bit = 0;

        for(int j=0; j < count_of_control_bits; j++)
        {
            QByteArray row = get_row(matrix,j);
            size_t control_bit = scalar(row,data[i]);
            control_bit = control_bit%2;
            if (control_bit > 0)
                error_in_word = true;
            id_wrong_bit += pow(2,j)*control_bit;
         }

        if(error_in_word)
        {
            if (data[i][id_wrong_bit-1] == '1')
                data[i][id_wrong_bit-1] = '0';
            else
                data[i][id_wrong_bit-1] = '1';
        }
        data[i] = delete_control_bits(data[i],count_of_control_bits);
    }

    return data;
}

QByteArray delete_control_bits(const QByteArray word ,size_t count_of_control_bits)
{
    QByteArray buf(word.data());
    for (int i =0 ; i < count_of_control_bits; i++)
        buf.remove((unsigned int)pow(2,i)-i-1,1);
    return buf;
}

QString to_str(const QByteArray & data)
{
    QString msg;
    QStringList list;
    list.reserve(data.size()/8);

    for (int i = 0; i < data.size()/8; i++)
    {
        QString buf;
        for (int j =0; j < 8; j++)
            buf.push_back(data[i*8+j]);

        list.push_back(buf);
        bitset<CHAR_BIT> b(list[i].toStdString().c_str());
        long int ascii_code= b.to_ulong(); 
        msg+= static_cast<char>(ascii_code);
    }
    return msg;
}

bool check_errors(QByteArrayList list, QVector<QPoint> errors, QByteArrayList res_list)
{
    for (int i =0; i < errors.size(); i++)
        if(list[errors[i].x()] != res_list[errors[i].x()])
            return false;
    return  true;
}

size_t count_of_controls_bits(size_t lenth)
{
    size_t count_of_control_bits = (unsigned int)log2(lenth) + 1;
    if (lenth + count_of_control_bits >= pow(2,count_of_control_bits))
        count_of_control_bits++;
    return count_of_control_bits;
}

QByteArrayList make_list(QByteArray data,size_t lenth)
{
    bool flag = false;
    size_t count =  data.size()/lenth;
    if (data.size()%lenth != 0) // если длина сообщения не кратна длине слова
    {
        count++;
        flag = true;
    }
    QByteArrayList list;
    for (int i = 0; i < count; i++) //разбиваем сообщение на слова нужной дины
    {
        QByteArray buf;
        for(int j = i*lenth; j < lenth*i+lenth; j++)
            buf.push_back(data[j]);

        list.push_back(buf);
    }
    if (flag)
    {
        list.back().truncate(lenth - (lenth - data.size()%lenth));
    }
    return list;
}
