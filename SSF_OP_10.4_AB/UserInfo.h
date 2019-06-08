#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include<QStringList>

struct Worker
{
    int count;
    QString workerId;
    QString workerName;

    Worker(const QString& strWorkerName,const QString& strWorkerId, int nCount)
    {
        count = nCount;
        workerName = strWorkerName;
        workerId = strWorkerId;
    }
    Worker()
    {
        count = 0;
        workerName = "";
        workerId = "";
    }
};//181109

class UserInfo : public QObject
{
    Q_OBJECT
public:
    UserInfo(const QString filePath, QObject *parent = 0);
    ~UserInfo();

    //Add new worker.
    void addNewWorker(const QString& newWorkerName, const QString& newId);

    //Get last worker count.
    Worker getLastWorker();

    //Get worker count by name.
    int getWorkerCount(const QString& workerId);

    //Get all workers .
    QList<Worker> getWorkerList() const
    {
        return _workerList;
    }

    //Check if woker exist or not.
    bool isThisWorkerExist(const QString& workerId);

    //Update worker count.
    void updateWorkerCount(const QString& workerId, int wCount);

    //Update worker list.
    void updateWorkerList(const QString& workerId);

    //Save workers to file.
    void saveWorkers();

private:

    //Read a configuration file.
    void readConfigFile();

private:

    QString _strFilePath;
    QList<Worker> _workerList;
};

#endif // USERINFO_H
