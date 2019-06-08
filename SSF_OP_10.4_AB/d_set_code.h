#ifndef D_SET_CODE_H
#define D_SET_CODE_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
    class D_set_code;
}

class D_set_code : public QDialog
{
    Q_OBJECT

public:
    explicit D_set_code(QWidget *parent = 0, int mode=0);
    ~D_set_code();
    Ui::D_set_code *ui;
    int m_nLCnt;

private slots:
    void on_b_close_clicked();
    void do_keypad_clicked(int);
    void on_b_enter_clicked();
    void on_b_yes_clicked();
    void on_b_no_clicked();
    void rcvd_value();

    void on_b_enter_2_clicked();

private:
    QButtonGroup *keypad;
    QString tmp,comment;
    void memory_in_mc(QString code);
    void memory_in_worker(QString **Group);
    void enter_event();
    void add_key();
    int dialog_mode;

    QString *codeGroup[11];
};
extern QString buffer_code[BUFFERINDEX +1];
#endif // D_set_code_H

