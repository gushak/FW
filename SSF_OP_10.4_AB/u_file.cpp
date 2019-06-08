#include "u_file.h"
#include <QDebug>
#include <QFile>
#include <QRegExp>


#define CR  0x0D
#define LF  0x0A


U_file::U_file()
{
}

int U_file::load(const QString &fname, QHash<int, QString> &hlist)
{
QString str;
int     no ;
char    line[1024] ;
char    *p;

	QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "(U_file::load) File Open Error: " << fname;
		return 1;
    }
    else {
		while (!file.atEnd()) {
			file.readLine(line,1024) ;

			if((p = strchr(line,':'))){
				*p  = 0;
				no  = atoi(line);
				strtok(p+1, "\r\n");
				str.sprintf("%s", p+1);
				str.replace("\3", "\n");
				hlist.insert(no, str);
			}
		}
		file.close();
	}
	return 0;
}

int U_file::load(const QString &fname, QHash<QString, QString> &hlist)
{
	QString var;
	QString val;
	char    line[1024] ;
	char    *p;

	QFile file(fname);
	if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "(U_file::load)1 File Open Error: " << fname;
		return 1;
	} else {
		while (!file.atEnd()) {
			file.readLine(line,1024) ;

			if((p = strchr(line,':'))){
				*p  = 0;
				var.sprintf("%s", line);
				strtok(p+1, "\r\n");
				val.sprintf("%s", p+1);
				val.replace("\3", "\n");
				hlist.insert(var, val);
			}
		}
		file.close();
	}

	return 0;
}

int U_file::save(const QString &fname, QHash<int, QString> &hlist)
{
	QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)){
        qDebug() << "(U_file::save) File Open Error: " << fname;
		return 1;
    }
    else {
		QHashIterator<int, QString> i(hlist);
		char line[1024];
		QString val;

		while (i.hasNext()) {
			i.next();
			val = i.value();
			val.replace(QRegExp("\r|\n|\r\n"), "\3");

			sprintf(line, "%d:%s\n", i.key(), val.toAscii().data());
			file.write(line);
		}
		file.close();
	}
	return 0;
}

int U_file::save(const QString &fname, QHash<QString, QString> &hlist)
{
	QFile file(fname);
	if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "(U_file::save)1 File Open Error: " << fname;
		return 1;
    }
    else{
		QHashIterator<QString, QString> i(hlist);
		char line[1024];
		QString val;

		while (i.hasNext()) {
			i.next();
			val = i.value();
			val.replace(QRegExp("\r|\n|\r\n"), "\3");

			sprintf(line, "%s:%s\n", i.key().toAscii().data(), val.toAscii().data());
			file.write(line);
		}
		file.close();
	}
	return 0;
}









//    design_data st;

//    st.can_value1 = data[1];
//    st.can_value2 = data[2];
//    st.can_value3 = data[3];
//    st.can_value4 = data[4];
//    st.can_value5 = data[5];
//    st.can_value6 = data[6];
//    st.can_value7 = data[7];


//    stitch_list.append(st);

//    qDebug()<<"do save ok";





int U_file::load(const QString &fname, QList<design_data> &hlist)
{
//    QString str;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File Open Error: " << fname;
        return 1;
    } else {
        char c;
        hlist.clear();
        design_data st;

        while (!file.atEnd()) {
            file.getChar(&c);
            st.can_value1 = (__u8)c;

            file.getChar(&c);
            st.can_value2 = (__u8)c;

            file.getChar(&c);
            st.can_value3 = (__u8)c;

            file.getChar(&c);
            st.can_value4 = (__u8)c;
#if 0
            file.getChar(&c);
            st.can_value5 = (__u8)c;

            file.getChar(&c);
            st.can_value6 = (__u8)c;

            file.getChar(&c);
            st.can_value7 = (__u8)c;

            file.getChar(&c);
            st.can_value8 = (__u8)c;
#endif
            hlist.append(st);
        }
        file.close();
    }

    return 0;
}

int U_file::save(const QString &fname, QList<design_data> &hlist)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "File Open Error: " << fname;
        return 1;
    } else {
        int count = hlist.count();

        for (int i = 0; i < count; i++) {
            file.putChar(hlist[i].can_value1);
            file.putChar(hlist[i].can_value2);
            file.putChar(hlist[i].can_value3);
            file.putChar(hlist[i].can_value4);
#if 0
            file.putChar(hlist[i].can_value5);
            file.putChar(hlist[i].can_value6);
            file.putChar(hlist[i].can_value7);
            file.putChar(hlist[i].can_value8);
     #endif
        }
        file.close();
    }
    return 0;
}


