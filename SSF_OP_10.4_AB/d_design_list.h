#ifndef D_DESIGN_LIST_H
#define D_DESIGN_LIST_H

#include <QDialog>
#include "mainwindow.h"




namespace Ui {
    class D_design_list;
}

class D_design_list : public QDialog
{
    Q_OBJECT

public:
    explicit D_design_list(QWidget *parent = 0);
    ~D_design_list();
    int m_nLCnt;



private slots:


    void on_b_close_clicked();

    void on_b_SCANNING_clicked();
    void showFileDetails(const QModelIndex &index);
    void on_b_connect_clicked();


    void on_b_usb_disconnect_clicked();

private:
    Ui::D_design_list *ui;

    QListWidget* m_qlw;
    QListWidgetItem* m_qlwi;

    void try_connect_db();
    void write_design_list();

    QHash<int, QString> list_design;


    void usb_mount();
    void storage_init();


    QDirModel *model;
    void fullpath(const QModelIndex &index);

    QString full_path;



};

#endif

















