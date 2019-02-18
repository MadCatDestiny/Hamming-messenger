#include "hamming.h"


QByteArrayList encode(QByteArray data,const size_t lenth)
{
     qDebug() <<"____________" <<endl << "IN ENCODE:" <<endl<<"____________" << endl;
    bool flag = false;
   qDebug() << data<<endl;
   size_t count =  data.size()/lenth;
   if (data.size()%lenth != 0) // если длина сообщения не кратна длине слова
   {
       count++;
       flag = true;
      // qDebug() << "true" <<endl;
   }
   // qDebug() << "Count : " << count <<endl  << "Size : " << data.size() <<endl;
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
    //qDebug() << "In list: ";
    for (int i =0; i< list.size(); i++) //вставляем контрольные биты
    {
      //  qDebug() << list[i];
        put_control_bits(list[i],lenth);

    }

    return list;
}

void put_control_bits(QByteArray &word,const size_t lenth)
{
    size_t count_of_control_bits = (unsigned int)log2(lenth) + 1;
    if (lenth + count_of_control_bits >= pow(2,count_of_control_bits))
        count_of_control_bits++;

    for(int i =0; i < count_of_control_bits; i++)
        word.insert(pow(2,i)-1,'0');

    QByteArrayList matrix = create_transition_matrix(word.size(),count_of_control_bits);
   // qDebug () << "WORD IN PUT:" << word <<endl;
    for(int i =0; i < count_of_control_bits; i++)
    {
        QByteArray row = get_row(matrix,i);
        size_t control_bit = scalar(row,word);
        control_bit = control_bit%2;
        if (control_bit)
            word[(unsigned int)pow(2,i)-1] = '1';
        else {
            word[(unsigned int)pow(2,i)-1] = '0';
        }
    }
}

QByteArrayList create_transition_matrix(const size_t word_size,const size_t count_of_control_bits)
{
    //qDebug() <<"____________" <<endl << "IN CREATE MATRIX:" <<endl<<"____________" << endl;
    QByteArrayList matrix;
    matrix.reserve(word_size);
    for (int i =0; i < word_size; i++)
    {
        QByteArray column;
        bitset<8> b(i+1);
        QString bits =b.to_string().c_str();

        //qDebug()<<"bits in create :" << bits <<endl;
        for (int j =0; j < count_of_control_bits; j++)
        {
            QByteArray bit = QByteArray(QString(bits[7-j]).toUtf8());
            //qDebug() << "Bit: " << bit << endl;
            column.push_back(bit);
        }
        //qDebug() << "Column:" << column <<endl;
        matrix.push_back(column);
    }
    //qDebug () << "Matrix:"<<endl;
    //show_matrix(matrix);
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
    //qDebug() << "get_row " << number << " : " << row << endl;
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
    //qDebug() << "X:" << x <<endl << "Y:" << y <<endl;
    size_t res = 0;
    for (int i =0; i < x.size(); i++)
        res+=QString(xv[i]).toInt()* QString(yv[i]).toInt();
    return res;
}

QVector<QPoint> make_error(QByteArrayList &data, size_t lenth, size_t errors)
{
    qDebug() <<"____________" <<endl << "IN MAKE ERRORS:" <<endl<<"____________" << endl;
    qDebug () << "WITHOUT ERRORS:" << data <<endl;
    srand(time_t(0));
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    qDebug() << "Errors:" << endl;
    QVector<QPoint> errors_coord;
    for (int i = 0; i < errors; i++)
    {

        size_t pos = qrand()%lenth*data.size();
        size_t word_id = pos/lenth;
        size_t byte_id =pos%lenth;
        errors_coord.push_back(QPoint(word_id,byte_id));
        qDebug () << "word id : byte id : byte" << endl;
        qDebug () << word_id << " : " << byte_id << " : "<< data[word_id][byte_id]<< endl;
        if (data[word_id][byte_id] == '0')
            data[word_id][byte_id] = '1';
        else {
            data[word_id][byte_id] = '0';
        }
    }
    qDebug () << "WITH ERRORS:" << data <<endl;
    return errors_coord;
}

QByteArrayList decode(QByteArrayList data, size_t lenth)
{
    qDebug() <<"____________" <<endl << "IN DECODE:" <<endl<<"____________" << endl;

    size_t count_of_control_bits = (unsigned int)log2(lenth) + 1;
    if (lenth + count_of_control_bits >= pow(2,count_of_control_bits))
        count_of_control_bits++;
    QByteArrayList matrix = create_transition_matrix(data[0].size(),count_of_control_bits);
   //qDebug() << "data.size = " << data.size() << endl;
    for (int i =0; i < data.size(); i++)
    {
        bool error_in_word = false;
       // qDebug() << "Data[" << i << "] = " << data[i] <<endl;
        size_t id_wrong_bit = 0;
        for(int j=0; j < count_of_control_bits; j++)
        {
            QByteArray row = get_row(matrix,j);
            size_t control_bit = scalar(row,data[i]);
           // qDebug() << "scalar = " << control_bit <<endl;

            control_bit = control_bit%2;
            if (control_bit > 0)
                error_in_word = true;
            id_wrong_bit += pow(2,j)*control_bit;
            //qDebug() << "ID wrong bit: " << id_wrong_bit <<endl;

         }
        if(error_in_word)
        {
            if (data[i][id_wrong_bit-1] == '1')
                data[i][id_wrong_bit-1] = '0';
            else
                data[i][id_wrong_bit-1] = '1';

        }
         delete_control_bits(data[i],count_of_control_bits);
    }
    return data;
}

void delete_control_bits(QByteArray & word ,size_t count_of_control_bits)
{
    //qDebug() << "BEFOR REMOVE:" << word<<endl;
    for (int i =0 ; i < count_of_control_bits; i++)
        word.remove((unsigned int)pow(2,i)-i-1,1);
    //qDebug() << "AFTER REMOVE:" << word<<endl;
}
QString to_str(const QByteArray & data)
{
    QString msg;
    QStringList list;
    //qDebug() << data.size()/8 <<endl;
    list.reserve(data.size()/8);
    for (int i = 0; i < data.size()/8; i++)
    {
        QString buf;
        for (int j =0; j < 8; j++)
            buf.push_back(data[i*8+j]);
        list.push_back(buf);
        //qDebug() << "LIST " << i << " : " << list[i] <<endl;
        bitset<CHAR_BIT> b(list[i].toStdString().c_str()); // одна из проблем тут
        long int ascii_code= b.to_ulong();
        qDebug() << ascii_code << " : " << char(ascii_code) <<endl;
        msg+= static_cast<char>(ascii_code);
    }
    return msg;
}
