#ifndef U_FILE_H
#define U_FILE_H

#include <QString>
#include <QHash>

#include "jasu.h"

class design_data
{
public:
    __u8    can_value1;
    __u8    can_value2;
    __u8    can_value3;
    __u8    can_value4;
//    __u8    can_value5;
//    __u8    can_value6;
//    __u8    can_value7;
//    __u8    can_value8;
};

class U_file
{
public:
    U_file();

    int load(const QString &fname, QHash<int, QString> &hlist);
    int save(const QString &fname, QHash<int, QString> &hlist);
    int load(const QString &fname, QHash<QString, QString> &hlist);
    int save(const QString &fname, QHash<QString, QString> &hlist);

    int load(const QString &fname, QList<design_data> &hlist);
    int save(const QString &fname, QList<design_data> &hlist);


};

#endif // U_FILE_H
