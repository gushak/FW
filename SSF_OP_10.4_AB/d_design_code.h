#ifndef D_DESIGN_CODE_H
#define D_DESIGN_CODE_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include "mainwindow.h"     //kimkt(120718)
#include "u_file.h"

namespace Ui {
    class D_design_code;
}

class D_design_code : public QDialog
{
    Q_OBJECT

public:
    //explicit D_design_code(QWidget *parent = 0);
    explicit D_design_code(QWidget *parent = 0, int group = 65);

    ~D_design_code();


private slots:
    void do_list_pressed(int);
    void do_list_released(int);
    void on_b_close_pressed();
    void on_b_list_up_pressed();
    void on_b_list_dn_pressed();

private:
    Ui::D_design_code *ui;

    U_file ufile;
    QHash<int, QString> param;
    QLabel *l_index[8];
    QButtonGroup *list;
    int current_page;
    int last_page;
    int tot_param;
    int cur_group;
    int cur_item;

    void set_button_group();
    void load_para_item(int group);
    void show_page(int page);
    void change_button_image(int idx, int img);
    __u8 send_data[8];      //kimkt(120718)

};

#endif // D_DESIGN_CODE_H
