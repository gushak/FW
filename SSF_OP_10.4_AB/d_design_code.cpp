#include "d_design_code.h"
#include "ui_d_design_code.h"
#include "path.h"


#if 0
D_design_code::D_design_code(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::D_design_code)
{
	ui->setupUi(this);
}

D_design_code::~D_design_code()
{
	delete ui;
}
#endif


//--------------------------------------------
extern MainWindow *w;


D_design_code::D_design_code(QWidget *parent, int group) :
    QDialog(parent,Qt::FramelessWindowHint),
    ui(new Ui::D_design_code)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);

    set_button_group();
    connect(list, SIGNAL(buttonPressed(int)), this, SLOT(do_list_pressed(int)));
    connect(list, SIGNAL(buttonReleased(int)), this, SLOT(do_list_released(int)));
    //connect(w, SIGNAL(sig_parameter(int,int,int,int,int)), this, SLOT(do_parameter(int,int,int,int,int)));

    cur_group = group;
    cur_item  = 0;
    load_para_item(group);
    show_page(current_page);

    //send_data[0] = MAIN_MODE;
    //send_data[1] = K_J_SETUP;
    //send_data[2] = K_MENU;
    //send_data[3] = K_M_DESIGN;
    //w->send_can(5, send_data);  //kimkt(120718)
}


D_design_code::~D_design_code()
{
    delete ui;
}


void D_design_code::set_button_group()
{
    list = new QButtonGroup(this);

    list->addButton(ui->b_list_1_, 1);
    list->addButton(ui->b_list_2_, 2);
    list->addButton(ui->b_list_3_, 3);
    list->addButton(ui->b_list_4_, 4);
    list->addButton(ui->b_list_5_, 5);
    list->addButton(ui->b_list_6_, 6);
    list->addButton(ui->b_list_7_, 7);
    //list->addButton(ui->b_list_1_, 0);      // test kimkt(130410)
    //list->addButton(ui->b_list_2_, 1);
    //list->addButton(ui->b_list_3_, 2);
    //list->addButton(ui->b_list_4_, 3);
    //list->addButton(ui->b_list_5_, 4);
    //list->addButton(ui->b_list_6_, 5);
    //list->addButton(ui->b_list_7_, 6);

#if 0
    l_index[0] = ui->l_t1;
    l_index[1] = ui->l_index_1_;
    l_index[2] = ui->l_index_2_;
    l_index[3] = ui->l_index_3_;
    l_index[4] = ui->l_index_4_;
    l_index[5] = ui->l_index_5_;
    l_index[6] = ui->l_index_6_;
    l_index[7] = ui->l_index_7_;
#else
    l_index[0] = ui->l_index_1_;
    l_index[1] = ui->l_index_2_;
    l_index[2] = ui->l_index_3_;
    l_index[3] = ui->l_index_4_;
    l_index[4] = ui->l_index_5_;
    l_index[5] = ui->l_index_6_;
    l_index[6] = ui->l_index_7_;
#endif
}

void D_design_code::load_para_item(int group)
{
    QString fname;

//    fname.sprintf("%s/%c", OPT_PATH, group);
    fname.sprintf("%s/punchcode", OPT_PATH);
    //fname.sprintf("%s/para.%c", "opt", group);
    //fname.sprintf("%s/PNC_CODE.H", OPT_PATH);        //kimkt(120717)

    if(ufile.load(fname, param)){
        qDebug() << "dialog clsoe";
        this->close();
    }
    tot_param = param.count() - 1;
    qDebug() << "(load_para_item) param.count() =" << param.count();
    last_page = (tot_param/7) + 1;
    current_page = 1;

    //l_index[0]->setText(param.value(0, "GROUP"));
    l_index[0]->setText("PUNCH CODE");
}


void D_design_code::show_page(int page)
{
QString str;
int key;
int max = param.count();
#if 0
    for(int i=1; i<8; i++){
        key = i + ((page - 1)*7);

        if(key < max){
            str.sprintf("%02d", key);
            l_index[i]->setText(str);

            str.sprintf("    ");
            str.append(param.value(key, " "));
            list->button(i)->setText(str);
        }
        else{
            l_index[i]->setText(" ");
            list->button(i)->setText(" ");
        }
        str.sprintf("QPushButton{background-image:url(%s/txt box5.png);}", IMAGE_BU_PATH);
        list->button(i)->setStyleSheet(str);
#else
    qDebug() << "1";
    //for(int i=1; i<8; i++){
    //    key = i + ((page - 1)*7);
    for(int i=0; i<7; i++){
        key = i + ((page - 1)*7);
        //qDebug() << "2";
        if(key < max){
            //qDebug() << "3";
            str.sprintf("%02d", key);
            l_index[i]->setText(str);
            //qDebug() << "4";
            str.sprintf("    ");
            //qDebug() << "4.1";
            str.append(param.value(key, " "));
            //qDebug() << "4.2";
            list->button(i+1)->setText(str);
            //list->button(i)->setText(str);    //test kimkt(130410)
            qDebug() << "show_page 1. i = " << i;
        }
        else{
            l_index[i]->setText(" ");
            list->button(i+1)->setText(" ");
            //list->button(i)->setText(" ");      //test kimkt(130410)
            qDebug() << "show_page 2. i = " << i;
        }
#endif
        str.sprintf("QPushButton{background-image:url(%s/txt box5.png);}", IMAGE_BU_PATH);
        list->button(i+1)->setStyleSheet(str);
        //list->button(i)->setStyleSheet(str);      //test kimkt(130410)
    }
}

void D_design_code::on_b_close_pressed()
{
    //w->send_key(K_P_G_EXIT);
    this->close();
}

void D_design_code::on_b_list_up_pressed()
{
    current_page--;

    if (current_page < 1)     current_page = 1;

    show_page(current_page);
}

void D_design_code::on_b_list_dn_pressed()
{
    current_page++;

    if (current_page > last_page)    current_page = last_page;

    show_page(current_page);
}

void D_design_code::change_button_image(int idx, int img)
{
    QString str;

    str.sprintf("QPushButton{background-image:url(%s/txt box%d.png);}", IMAGE_ICON_PATH, img);
    list->button(idx)->setStyleSheet(str);
}

void D_design_code::do_list_pressed(int idx)
{
    QPalette pal(QColor(255,255,255));

    pal.setColor(QPalette::WindowText, QColor(255,255,255));
    pal.setColor(QPalette::ButtonText, QColor(0,0,0));

    change_button_image(idx, 4);

    l_index[idx]->setPalette(pal);
    list->button(idx)->setPalette(pal);
}

void D_design_code::do_list_released(int idx)
{
    cur_item = idx + ((current_page - 1) * 7);
    //w->send_key(cur_group, cur_item);

    w->Code_Val = cur_item - 1;     //kimkt(121226)

    qDebug() << "group:" << cur_group << "cur_item:" << cur_item;
    qDebug() << "cur_item-1:" << cur_item-1;

    QPalette pal(QColor(255,255,255));

    pal.setColor(QPalette::WindowText, QColor(255,220,0));
    pal.setColor(QPalette::ButtonText, QColor(255,255,255));

    change_button_image(idx, 5);

    l_index[idx]->setPalette(pal);
    list->button(idx)->setPalette(pal);
    //----- kimkt(120718) -----
    send_data[0] = MAIN_MODE;
    send_data[1] = K_J_SETUP;
    send_data[2] = K_MENU;
    send_data[3] = K_M_DESIGN;
    send_data[4] = K_ENTER;
    send_data[5] = cur_item - 1;
    w->send_can(6, send_data);
    this->close();              //kimkt(120718)
    //-------------------------
    //send_data[0] = MAIN_MODE;
    //send_data[1] = K_J_SETUP;
    //send_data[2] = K_MENU;
    //send_data[3] = K_M_DESIGN;
    //w->send_can(5, send_data);  //kimkt(120718)

}

