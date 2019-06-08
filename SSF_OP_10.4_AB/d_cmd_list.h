#ifndef D_CMD_LIST_H
#define D_CMD_LIST_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class D_cmd_list;
}

class D_cmd_list : public QDialog
{
    Q_OBJECT

public:
    explicit D_cmd_list(QWidget *parent = 0);
    ~D_cmd_list();

private slots:
    void on_b_close_clicked();
    void on_b_refresh_clicked();

    void on_b_dataview_clicked();

    void on_pushButton_clicked();
    //void cmd_selected(QListWidgetItem * item);
    void cmd_selected(const QModelIndex &index);
    //void showFileDetails(const QModelIndex &index);


private:
    Ui::D_cmd_list *ui;

    int m_nLCnt;

    QString selected_command;

    QHash<int, QString> list_design;
    QHash<int, QString> job_instruction;
    QListWidget* m_qlw;
    QListWidgetItem* m_qlwi;
    void list_view();
    void try_connect_db();
    void write_command_list();
    void view_mode();
    void list_mode();
    void write_command(QString name);
};

#endif

















