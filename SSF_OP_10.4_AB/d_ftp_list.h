#ifndef D_FTP_LIST_H
#define D_FTP_LIST_H

#include <QDialog>
#include "mainwindow.h"




namespace Ui {
    class D_ftp_list;
}

class D_ftp_list : public QDialog
{
    Q_OBJECT

public:
    explicit D_ftp_list(QWidget *parent = 0);
    ~D_ftp_list();
    int m_nLCnt;



private slots:
    void on_b_close_clicked();
    void showFileDetails(const QModelIndex &index);
    void on_b_ok_clicked();

private:
    Ui::D_ftp_list *ui;
    QDirModel *model;
    void fullpath(const QModelIndex &index);

    QString full_path;


};

#endif

















