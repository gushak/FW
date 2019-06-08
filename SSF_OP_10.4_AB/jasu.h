#ifndef JASU_H
#define JASU_H


#include <QtGui>
#include "iodefine.h"   //(121109)
//#include "mainwindow.h" //(121110)
//#include "mainwindow.h"


typedef unsigned char  __u8;
typedef unsigned short __u16;
typedef unsigned int   __u32;


class stitch_data           //draw design
{// 절대 좌표
public:
    int x;
    int y;
    int mc_flag;
};

class read_buf
{// 상대 좌
public:
    __u16 dx;
    __u16 dy;
    __u16 attr;
};


class read_data
{
public:
    char one_stitch[6];
};


#if 0       //
class stitch_data_cd    //(121108)     designe buffer
{   // 절대 좌표
public:
    unsigned char x_cd;
    unsigned char y_cd;
    unsigned char mc_flag_cd;
};
#endif

#ifdef CD_DATA_TEST
class read_buf_cd
{// 상대 좌
public:
    __u8 dx;
    __u8 dy;
    __u8 attr;
};


class read_data_cd
{
public:
    //char one_stitch[3];
    char one_stitch_cd[3];
};

#endif



class stitch_header
{
public:
    __u16 pn;       // 1
    __u16 ts;       // 2
    __u16 x2;       // 3
    __u16 y2;       // 4
    __u16 xs;       // 5
    __u16 ys;       // 6
    __u16 mx;       // 7
    __u16 xp;       // 8
    __u16 yp;       // 9
    __u16 wt;       // 10
    __u16 rv;       // 11
    __u16 cs;       // 12
    __u16 nc;       // 13
    __u16 we;       // 14
    __u32 ca;       // 15~16
    __u16 bc;       // 17
    __u16 cf;       // 18
    __u16 ps;       // 19
    __u8  xa;       // 20
    __u8  ya;       // 21
    __u16 ct;       // 22
    __u16 sc;       // 23
    __u16 es;       // 24
    __u16 pc;       // 25
    __u16 pl;       // 26
};


class header_info
{
public:
    __u32 total_stitch;
    int   xsize;
    int   ysize;
};


class Jasu : public QWidget
{
    Q_OBJECT
public:
    explicit Jasu(QWidget *parent = 0, int mode = 0);
    ~Jasu();

    QList <read_data> read_stitch;
#ifdef CD_DATA_TEST
    QList <read_data_cd> read_stitch_cd;
#endif
    char read_header[256];

    QHash <int, int> warnig_position;

    QHash <int, header_info> header_buf;
    header_info h_info;

    __u32 total_stitch;
    int  stop;

    //bool scale_no_fg;     //(121112)
    int  load_design(const QString &filename, int sub_prc);
    int  draw_design(int sub_prc);
    void change_color(int col);
    void clear_screen(int col);


    void show_zoom(int updown);
    void show_zoom_new(int updown,int tot_stitch);

    int current_total_stitch;
    void move_origin_point(void);
    void zoom_move_origin_point(int x, int y,int custom_zoom_level);


    int current_screen_x, current_screen_y;
    void move_cursor(int x, int y, char type, char col);
    void move_cursor(__u32 mc_stitch);
    void draw_sewing_area(void);
    void zoom_draw_sewing_area(void);
    void zoom_set_sewing_area(__u16 nx, __u16 px, __u16 ny, __u16 py,int custom_zoom_level,int cur_x,int cur_y);
    void set_sewing_area(__u16 nx, __u16 px, __u16 ny, __u16 py);
    //void draw_circle(int x, int y,  unsigned char type, unsigned char size);  //(121215)
    void draw_circle(int x, int y, unsigned char type, unsigned char color, unsigned char size);     //(130216)
    void draw_circle2(int x, int y, unsigned char type, unsigned char color, unsigned char size);
    void draw_line(int x, int y, unsigned char type, unsigned char size);     //(121218)
    void draw_line2(int x, int y, unsigned char type, unsigned char size);
    void draw_code(unsigned char code);        //(130306)
    void draw_code_abs(unsigned char code, int dx, int dy);        //(130511)
    void draw_jump_line(int x, int y, unsigned char type, unsigned char color, unsigned char size);  //(130418)
    void color_draw_jump_line2(int x, int y, unsigned char color, unsigned char size, bool save,bool operation);
    void draw_jump_line2(int x, int y, unsigned char color, unsigned char size);
    void painting_draw_jump_line2(int x, int y, unsigned char type, unsigned char color, unsigned char size);
    void move_jump(int x, int y, unsigned char type, unsigned char color, unsigned char size);
    void relxy_origin();                    //(130425)
    unsigned char E_data_fg;                //(121108)
    int cnt_p, cnt_b, scale_x, scale_y;     //(130527)
    int cursor_x,         cursor_y;         //(130531)
    void caculate_center();
    void move_zoom_point(int zoom_x,int zoom_y);

    int zoom_cordinatation(int x, int y);
    void zoom_set_factor(float sc);

    void drow_icon(bool painter,int cur);                    //(130425)
    void history_drow_icon();                    //(130425)

    int cur_cursor_x;int cur_cursor_y;
    void clear_warning_point();

    int set_jump_origin_x;
    int set_jump_origin_y;



protected:
    void paintEvent(QPaintEvent *event);

private:
    int  stitch_list_init(void);
    int  set_mc_color(char attr);
    int  read_jasufile(const QString &filename);
    int  read_stitch_from_jasufile(QDataStream *in, __u8 *ix, __u8 *iy, __u8 *ia);
    int  read_stitch_from_jasufile_cd(QDataStream *in, __u8 *ix, __u8 *iy, __u8 *ia);       //(121113)
    int  draw_stitch(QPainter *p, __u32 cs);
    void quick_draw(QPainter *p, __u32 stitch_idx);
    void set_factor(float sc);
#ifdef SCALE_TEST       //(121112)
    void set_factor_xyscale(float sc);      //(121112)
#endif
    void get_origin_point(__u16 w, __u16 h);
    void zoom_get_origin_point(__u16 w, __u16 h);
    int  get_screen_x(int x);
    int  get_screen_y(int y);

    void move_origin_x(int x);
    void move_origin_y(int y);
    int  move_point(QPainter *p, int x, int y);

    void draw_cursor(QPainter *p);
    void draw_cursor_(QPainter *p, unsigned char color);   //(130219)
    void draw_cross(QPainter *p);
    void draw_origin(QPainter *p);

    void draw_mc(QPainter *p, __u32 cs);
    void draw_jump(QPainter *p, QLineF line);
    void draw_line(QPainter *p, QLineF line, __u32 cs);
    int cordinatation(int x, int y);
    int cordinatation_(int x, int y);   //(130617)

    QHash <int, QList <stitch_data> > stitch_buf;
    QList <stitch_data> stitch_list;

    //#ifdef CD_DATA_TEST
    //QHash <int, QList <stitch_data_cd> > stitch_buf_cd;
    //QList <stitch_data_cd> stitch_list_cd;
    //#endif

    int prev_screen_x,    prev_screen_y;
    int origin_x,         origin_y;
    //int cursor_x,         cursor_y;

    __u16 negative_x;
    __u16 positive_x;
    __u16 negative_y;
    __u16 positive_y;

    float factor; // scale * zoomlevel
#ifdef SCALE_TEST
    float factor_x, factor_y;   //(121110)
    //int x_scale, y_scale;     //(121111)
#endif
    float scale;
    qint8 zoom_level;

    qint8 current_color;
    qint8 backgroup_color;
    qint8 cursor_color;
    bool  thumbnail_mode;

    QPixmap gr_pixmap;
    //	QPainter *gr_painter;

    int PenCol;
};

#endif // JASU_H

