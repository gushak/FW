#include <QtGui>
#include "lib.h"

void echo_hex(int cnt, char *str)
{
    char msg[4096];

    sprintf(msg, "[hex]");
    for (int i=0; i<cnt; i++)
        sprintf(msg, "%s %02x", msg, str[i]);

    qDebug() << msg;
}
