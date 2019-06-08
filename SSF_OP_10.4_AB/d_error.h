#ifndef D_ERROR_H
#define D_ERROR_H

#include <QDialog>
#include "mainwindow.h"     //kimkt(120916)


namespace Ui {
    class D_error;
}

class D_error : public QDialog
{
    Q_OBJECT

public:
    explicit D_error(QWidget *parent = 0);
    ~D_error();

    void change_info(int err, QString msg);
    void change_err_list(int err);      //kimkt(131126) test

private slots:
    void on_b_close_clicked();

private:
    Ui::D_error *ui;
};

#endif // D_ERROR_H
