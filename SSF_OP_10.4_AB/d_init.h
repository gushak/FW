#ifndef D_INIT_H
#define D_INIT_H

#include <QDialog>
#include "mainwindow.h"     //kimkt(120916)


namespace Ui {
    class D_init;
}

class D_init : public QDialog
{
    Q_OBJECT

public:
    explicit D_init(QWidget *parent = 0);
    ~D_init();


private slots:


    void on_b_close_clicked();


    void on_b_yes_clicked();

    void on_b_no_clicked();

private:
    Ui::D_init *ui;
};

#endif // D_INIT_H
