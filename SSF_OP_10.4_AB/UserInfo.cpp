#include "UserInfo.h"

#include<QFile>
#include<QDebug>
#include <QStringList>
#include <QTextCodec>
#include<QMessageBox>
#include<QTextStream>

UserInfo::UserInfo(const QString filePath, QObject *parent) :
    QObject(parent)
{
    _strFilePath = filePath;
    readConfigFile();
}

UserInfo::~UserInfo()
{

}

void UserInfo::readConfigFile()
{
    QFile file(_strFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream txtStream(&file);
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");

    while(!txtStream.atEnd())
    {
        QString strLine = txtStream.readLine().trimmed();

        if(strLine != "[GROUP]" && strLine != "[ENDGROUP]")
        {
            QStringList wList = strLine.split("/");
            //wList.removeAll("");

            if( wList.count()!=3 ) continue;

            QByteArray encodeString = wList[0].toLocal8Bit();
            QString strName = codec->toUnicode(encodeString);

            _workerList.append(Worker(strName, wList[1], wList[2].toInt()));
        }
    }
}

Worker UserInfo::getLastWorker()
{
    Worker last;
    if(_workerList.count() != 0)
    {
        last = _workerList[_workerList.count() - 1];
    }
    return last;
}

int UserInfo::getWorkerCount(const QString& workerId)
{
    foreach(Worker w, _workerList)
    {
        if(w.workerId == workerId)
        {
            return w.count;
        }
    }

    return 0;
}

void UserInfo::addNewWorker(const QString& newWorkerName, const QString& newId)
{
    _workerList.append(Worker(newWorkerName, newId, 0));
}

bool UserInfo::isThisWorkerExist(const QString& workerId)
{
    bool exist = false;
    foreach(Worker w , _workerList)
    {
        if (w.workerId == workerId)
        {
            exist = true;
        }
    }

    return exist;
}

void UserInfo::updateWorkerCount(const QString& workerId, int wCount)
{
    for(int i = 0; i < _workerList.count(); i++)
    {
        if(_workerList[i].workerId == workerId)
        {
            _workerList[i].count = wCount;
        }
    }
}

void UserInfo::updateWorkerList(const QString& workerId)
{
    int workerIndex = -1;
    Worker w;
    for( int i= 0; i < _workerList.count(); i++)
    {
        if(_workerList[i].workerId == workerId)
        {
            workerIndex = i;
            w = _workerList[i];
        }
    }

    if(_workerList.count() != 0 && workerIndex != -1)
    {
        _workerList[workerIndex] = _workerList[_workerList.count() - 1];
        _workerList[_workerList.count() - 1] = w;
    }
}

void UserInfo::saveWorkers()
{
    if( _workerList.count() == 0 )return;

    QFile file(_strFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream txtStream(&file);
    txtStream.setCodec("UTF-8");
    txtStream << "[GROUP]\n";

    foreach(Worker w, _workerList)
    {
        txtStream << w.workerName +"/"+ w.workerId + "/" + QString::number(w.count) << endl;
    }
    txtStream << "[ENDGROUP]";
    file.close();
}
