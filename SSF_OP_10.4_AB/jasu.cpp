#include "jasu.h"
#include "lcdcmd.h"
#include "path.h"
#include "iodefine.h"   //(121109)
#include "mainwindow.h"

#include "u_file.h"
#define DEBUG_CD_DATA         1
extern MainWindow *w;
//#include "u_file.h"


const QColor color[20] = {
    QColor(255,255,255), // white  0
    QColor(0,0,0),       // black  1
    QColor(192,192,192), // gray   2
    QColor(255,0,0),     // red    3
    QColor(0,255,0),     // green  4
    QColor(0,0,255),     // blue   5
    QColor(255,255,0),   // yellow 6

    QColor(215,109,194), // trim
    QColor(43,155,15),   // sec_org
    QColor(180,225,190), // pause
    QColor(145,85,215),  // sti_spd
    QColor(60,100,172),  // scl_ref
    QColor(5,60,215),    // set_opnn
    QColor(0,168,230),   // chk_ipnn
    QColor(172,105,243), // time delay
    QColor(215,240,190), // pf_control
    QColor(172,105,0),   // can_opnn
    QColor(215,0,190),   // can_opnn
};

const QColor Qt_Color[22] = {
    QColor(255,255,255), // white  0
    QColor(255,255,255), // white  1
    QColor(0,0,0),       // black  2  (O)
    QColor(255,255,255), // white  3  (O)
    QColor(169,169,169), // dark gray   4  (O)
    QColor(192,192,192), // gray   5  (O)
    QColor(211,2111,211),// light gray   6  (O)
    QColor(255,0,0),     // red    7  (O)
    QColor(0,255,0),     // green  8  (O)
    QColor(0,0,255),     // blue   9  (O)
    QColor(  0,255,255), // cyan   10 (O)
    QColor(255,  0,255), // magenta 11 (O)
    QColor(255,255,  0), // yellow  12 (O)
    QColor(139,  0,  0), // dark red   13 (O)
    QColor(0,100,0),     // dark green 14 (O)
    QColor(  0,  0,139), // dark blue  15 (O)
    QColor(0,139,139),   // dark cyan  16 (O)
    QColor(139,0,139),   // dark magenta 17 (O)
    QColor(255,0,0),     // red    18
    QColor(255,0,0),     // dark yellow 19
    QColor(255,192,203), // pink 20 (O)
    QColor(128,  0,128), // purple 21 (O)
};


Jasu::Jasu(QWidget *parent, int mode) :
    QWidget(parent)
{
    gr_pixmap = QPixmap(parentWidget()->width(), parentWidget()->height());
    resize(parentWidget()->width(), parentWidget()->height());

    gr_pixmap.fill(color[_WHITE]);

    //stitch_list_init();

    current_color  = _RED;
    cursor_color   = _BLUE;
    thumbnail_mode = mode;
    zoom_level     = 1;
    cursor_x       = 0;
    cursor_y       = 0;

    cur_cursor_x = 0;
    cur_cursor_y = 0;

}

Jasu::~Jasu( )
{

}

int Jasu::stitch_list_init(void)
{
#if 1
    stitch_data st;

    st.x = 0;
    st.y = 0;
    st.mc_flag = -1;

    read_stitch.clear();
    stitch_list.clear();
    stitch_list.append(st);

    total_stitch   = 0;

#ifdef CD_DATA_TEST
    read_stitch_cd.clear();     //(130212)
#endif
    return 0;
#endif



}




int Jasu::draw_design(int sub_prc)
{
    if(DEBUG_CD_DATA)   qDebug() << "(draw_design) start";

    if(stitch_buf.contains(sub_prc)) {
        stitch_list  = stitch_buf.value(sub_prc);
        h_info       = header_buf.value(sub_prc);
        total_stitch = h_info.total_stitch;
    }
    else{
        return -1;
    }

    current_color = _RED;
    QPainter gr_painter(&gr_pixmap);
    if(DEBUG_CD_DATA)   qDebug() << "(draw_design) quick_draw, total_stitch:" << total_stitch;

    quick_draw(&gr_painter, total_stitch);
    if(DEBUG_CD_DATA)   qDebug() << "(draw_design) move_point";

    move_point(&gr_painter, cursor_x, cursor_y);
    if(DEBUG_CD_DATA)   qDebug() << "(draw_design) repaint";

    repaint();
    repaint();
    repaint();
    this->repaint();

    return 0;
}

int Jasu::load_design(const QString &filename, int sub_prc)
{

    //qDebug() << "(load_design) no="<< filename;

    stitch_list_init();

    if(read_jasufile(filename))     return -1;

    stitch_buf.insert(sub_prc, stitch_list);
    header_buf.insert(sub_prc, h_info);

    return 0;
}


int Jasu::read_jasufile(const QString &filename)   //load design jong deok
{
#if 0
    //qDebug() << "(read_jasufile) start";
    //qDebug() << "(read_jasufile) " << filename;

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Jasu"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return -1;
    }

    QDataStream in(&file);


    design_data st;

    int x, y, mc_flag;
    __u8  ix, iy, ia;

    for(__u32 i=0; i<total_stitch; i++){
        // A,B TYPE
        //----- cd data -----
        //qDebug() << "(read_jasufile) CD rootine ";
        read_stitch_from_jasufile_cd(&in, &ix, &iy, &ia);
        //qDebug() << "(read_jasufile) cnt= " << i << "ix" << ix << "iy" << iy;     // 2


        x = ix;
        if(ia & 0x80) x = -x;
        y = iy;
        if(ia & 0x40) y = -y;
        ia &= 0x3f;

        mc_flag = set_mc_color(ia);
        //qDebug() << "(read_jasufile) mc_flag = " << mc_flag;
        //qDebug() << "(read_jasufile) i=" << i << "x=" << x << "y=" << y;

        if(mc_flag > 0){                // attri
            st.x = stitch_list[i].x;
            st.y = stitch_list[i].y;
        }
        else{                           // jump = -1
            st.x = stitch_list[i].x + x;
            st.y = stitch_list[i].y + y;
        }
        st.mc_flag = mc_flag;
        stitch_list.append(st);

        if(in.atEnd()){
            total_stitch = i + 1;
            break;
        }
    }
    file.close();



    h_info.total_stitch = total_stitch;
    h_info.xsize = h->xa;
    h_info.ysize = h->ya;

    return 0;
#endif
}


int Jasu::read_stitch_from_jasufile(QDataStream *in, __u8 *ix, __u8 *iy, __u8 *ia)
{
    __u8 x, y, a;
    __u8 x8, y8;

    read_buf  *sp;
    read_data buf;

    int r;

    r = in->readRawData(buf.one_stitch, 6);

    if(r != 6)	return r;

    if(!thumbnail_mode)     read_stitch.append(buf);

    sp = (read_buf *) buf.one_stitch;

    x  = sp->dx & 0xff;
    x8 = sp->dx >> 8;

    y  = sp->dy & 0xff;
    y8 = sp->dy >> 8;

    a = sp->attr & 0x3f;
    if(x8) a |= 0x80;
    if(y8) a |= 0x40;

    *ix = x;
    *iy = y;
    *ia = a;

    return (0);
}


#ifdef CD_DATA_TEST //THIS
int Jasu::read_stitch_from_jasufile_cd(QDataStream *in, __u8 *ix, __u8 *iy, __u8 *ia)
{
    __u8 x, y, a;
    __u8 x8, y8;

    int r;

    //qDebug() << "(read_stitch_from_jasufile_cd) start";
    //----- D series data -----
    read_buf_cd  *sp;
    read_data_cd buf;

    r = in->readRawData(buf.one_stitch_cd, 3);

    //qDebug() << "test2";

    if(r != 3)	return r;

    //qDebug() << "(read_stitch_from_jasufile_cd) thumbnail_mode=" << thumbnail_mode;
    //if(!thumbnail_mode)     qDebug() << "(read_stitch_from_jasufile_cd)1";
    if(!thumbnail_mode)     read_stitch_cd.append(buf);

    //qDebug() << "test3";
    sp = (read_buf_cd *)buf.one_stitch_cd;

    //qDebug() << "(read_stitch_from_jasufile_cd) dx:" << sp->dx << "dy:" << sp->dy << "attr:" << sp->attr;

    x  = sp->dx & 0x7f;
    x8 = sp->dx & 0x80;     //x8 = sp->dx >> 7;

    y  = sp->dy & 0x7f;
    y8 = sp->dy & 0x80;     //y8 = sp->dy >> 7;

    a = sp->attr & 0x3f;
    if(x8) a |= 0x80;
    if(y8) a |= 0x40;

    //qDebug() << "(read_stitch_from_jasufile_cd) end";
    *ix = x;
    *iy = y;
    *ia = a;

    return (0);
}
#endif


int Jasu::draw_stitch(QPainter *p, __u32 cs)
{
    //qDebug()<<"thisthis";
    int mc_dot, jp_dot;


    mc_dot = 7;
    jp_dot = 5;

    //    if(stitch_list[cs].mc_flag > 0)
    //    {
    //        p->setPen(QPen(color[stitch_list[cs].mc_flag], mc_dot,  Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    //        p->drawPoint(current_screen_x, current_screen_y);

    //        //qDebug() <<"draw stitch 1";
    //        if(!thumbnail_mode){
    //            QString str;
    //            str.sprintf("%s", IMAGE_PATH);
    //            str.append("/icon/");     //
    //            if (stitch_list[cs].mc_flag == _TRIM){
    //                str.append("code_trim.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _SET_ORG){
    //                str.append("code_2nd.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _PAUSE){
    //                str.append("code_pause.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _SET_OPnn){
    //                str.append("code_output.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _CHK_IPnn){
    //                str.append("code_input.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _TIME_DELAY){
    //                str.append("code_delay.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _REV){      //(121212)
    //                str.append("code_rev.png");
    //            }
    //            else if (stitch_list[cs].mc_flag == _PF_CONTROL){      //(130718)
    //                str.append("code_pf_dn.png");
    //            }

    //            QPixmap img(str);
    //            //p->drawPixmap(current_screen_x, current_screen_y - 10, img);
    //            p->drawPixmap(current_screen_x, current_screen_y + 6, img);
    //        }
    //    }
    //    else{

    //qDebug() << "1";


    QLineF line(prev_screen_x, prev_screen_y, current_screen_x, current_screen_y);
    //qDebug() << "2";
    //qDebug()<<prev_screen_x<<"|"<< prev_screen_y<<"|"<<  current_screen_x<<"|"<<  current_screen_y;

    p->setPen(QPen(color[current_color], 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //qDebug() << "3";
    p->drawLine(line);
    //qDebug() << "4";
    p->setPen(QPen(color[_BLACK], jp_dot, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //qDebug() << "5";
    p->drawPoint(current_screen_x, current_screen_y);
    //qDebug() << "6";
    // if(stitch_list[cs-1].mc_flag == -1)    p->drawPoint(prev_screen_x, prev_screen_y);
    //  }

    prev_screen_x = current_screen_x;
    prev_screen_y = current_screen_y;

    return 0;
}

void Jasu::clear_screen(int col)
{
    gr_pixmap.fill(color[col]);

}

void Jasu::draw_cross(QPainter *p)
{
    //if(DEBUG_CD_DATA)   qDebug() << "draw_cross   factor =" << factor;

    int x, y;
    int x1, y1, x2, y2;

    x = parentWidget()->width() - 1;
    y = parentWidget()->height() - 1;


    x1 = origin_x + (int)(-negative_x * factor);
    y1 = origin_y - (int)(positive_y * factor);

    x2 = origin_x + (int)(positive_x * factor);
    y2 = origin_y - (int)(-negative_y * factor);

    if (x1 < 0) x1 = 0;
    if (x2 > x) x2 = x;
    if (y1 < 0) y1 = 0;
    if (y2 > y) y2 = y;

    clear_screen(_WHITE);

    p->setPen(QPen(Qt::gray, 0, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));

    p->drawLine(x1, y1, x1, y2);
    p->drawLine(x2, y1, x2, y2);

    p->drawLine(x1, y1, x2, y1);
    p->drawLine(x1, y2, x2, y2);

    //qDebug() << "draw_origin 3";

    p->drawLine(x1, origin_y, x2, origin_y);
    p->drawLine(origin_x, y1, origin_x, y2);
}

void Jasu::draw_origin(QPainter *p)
{
    if(zoom_level==1){
        draw_cross(p);
    }else if(zoom_level==2)
    {
        ;
    }
    //draw_cross(p);

    prev_screen_x = origin_x;
    prev_screen_y = origin_y;

}




void Jasu::draw_sewing_area(void)
{
    //warnig_position.clear();
    QPainter gr_painter(&gr_pixmap);

    //qDebug() << "(draw_sewing_area) start";

    __u16   w, h;
    float x, y, tmp1, tmp2;

    w = negative_x + positive_x;
    h = negative_y + positive_y;

    //qDebug() << "(draw_s_a) w : "<< w  << "negative_x : "<< negative_x << "positive_x : "<< positive_x;
    //qDebug() << "(draw_s_a) h : "<< h  << "negative_y : "<< negative_y << "positive_y : "<< positive_y;

    x = (float)parentWidget()->width();
    y = (float)parentWidget()->height();

    //qDebug() << "widget size x : "<< x  << "y : "<< y;

    tmp1 = x / (float)w;
    tmp2 = y / (float)h;

    //qDebug() << "tmp1 : "<< tmp1  << "tmp2 : "<< tmp2;

    (tmp1 < tmp2) ? set_factor(tmp1) : set_factor(tmp2);
    //(tmp1 < tmp2) ? set_factor_xyscale(tmp1) : set_factor_xyscale(tmp2);


    get_origin_point(w, h);

    draw_origin(&gr_painter);
    move_point(&gr_painter, 0, 0);

    //qDebug() << "(draw_sewing_area) end";
}

void Jasu::set_sewing_area(__u16 nx, __u16 px, __u16 ny, __u16 py)
{
    //qDebug() << "set_sewing_area" << nx << px << ny << py <<  (nx+px)/10 << (ny+py)/10;
    zoom_level = 1;

    negative_x = nx * 20;
    positive_x = px * 20;
    positive_y = ny * 20;
    negative_y = py * 20;

    draw_sewing_area();
}


void Jasu::quick_draw(QPainter *p, __u32 stitch_idx)
{

    qDebug() << "(quick draw) start";

    draw_origin(p);

    qDebug() << "(quick draw) total sti = " << stitch_idx;

    for(__u32 i=0; i<stitch_idx; i++)
    {
        //qDebug() << "(quick draw) cnt" << i << "x = " << stitch_list[i].x << "y = " << stitch_list[i].y;

        //qDebug() <<"origin_x"<< origin_x <<"origin_y"<< origin_y<<"factor"<<factor;
        //        qDebug() <<"x"<<  stitch_list[i].x<<"origin_y"<< stitch_list[i].y;

        //        x = stitch_list[i].can_value1;
        //        y = stitch_list[i].can_value3;

        //qDebug()<<"1";

        //qDebug()<<"stitlist x : "<<w->stitch_list[i].can_value1<<"stitlist y :"<<w->stitch_list[i].can_value3;

        current_screen_x = get_screen_x(w->stitch_list[i].can_value1);

        // qDebug()<<"2";
        current_screen_y = get_screen_y(w->stitch_list[i].can_value3);

        // qDebug()<<"3";
        //if(DEBUG_CD_DATA)   qDebug() << "(quick draw) sti=" << i;
        draw_stitch(p, i);

        // qDebug()<<"ok!";
    }
    qDebug() << "(quick draw) end";
}


void Jasu::set_factor(float sc)
{
    scale  = sc;
    //factor = scale * w->custom_zoom_level;
    factor = scale * zoom_level;
    //qDebug() << "scale:" << scale << "zoom:" << zoom_level << "factor:" << factor;
    // zoom in, zoom out : seperate x, y
}

int  Jasu::get_screen_x(int x)
{
    //qDebug()<<"origin_x"<<origin_x<<"x"<<x<<"factor"<<factor;
    return origin_x + (int)(x * factor);
}

int  Jasu::get_screen_y(int y)
{
    //qDebug()<<"origin_y"<<origin_y<<"y"<<y<<"factor"<<factor;

    return origin_y - (int)(y * factor);
}


void Jasu::get_origin_point(__u16 w, __u16 h)
{
    //qDebug() << "get_origin_point w=" << w << "h=" << h << "factor" << factor;






    origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
    origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);


    //qDebug()<<(parentWidget()->width() - w*factor)/2;
    //qDebug()<<negative_x*factor;



    //qDebug() << "get_origin_point   (parentWidget()->width() =" << parentWidget()->width();
    //qDebug()<<"w :"<<w<<"factor :"<<factor<<"negative_x : "<<negative_x;


    //qDebug() << "get_origin_point   (parentWidget()->height() =" << parentWidget()->height();




    //qDebug() << "get_origin_point origin_x =" << origin_x << "origin_y =" << origin_y;

    //get_origin_point origin_x = 283 origin_y = 281

}





void Jasu::move_origin_point()
{
    //qDebug() << "set_sewing_area" << nx << px << ny << py <<  (nx+px)/10 << (ny+py)/10;
    zoom_level = 1;
    cursor_x=0;
    cursor_y=0;
    negative_x = 400 * 20;
    positive_x = 400 * 20;
    positive_y = 400 * 20;
    negative_y = 400 * 20;

    //draw_sewing_area();
}

void Jasu::move_origin_x(int x)
{
    //qDebug()<<"move_origin_x st";
    int lx, rx; // left, right

    origin_x = origin_x + parentWidget()->width()/2 - x;

    rx = get_screen_x(positive_x);
    lx = get_screen_x(-negative_x);

    if((rx - lx) < parentWidget()->width()){
        origin_x = ((parentWidget()->width() - (rx - lx))/2) + (negative_x*factor);
    }
    else if(lx > 0){
        origin_x -= lx;\
    }
    else if(rx < parentWidget()->width()){
        origin_x += (parentWidget()->width() - rx - 1);
    }
    //qDebug()<<"move_origin_x end";
}

void Jasu::move_origin_y(int y)
{
    //qDebug()<<"move_origin_y st";

    int ty, by; // top, bottom

    origin_y = origin_y + parentWidget()->height()/2 - y;

    ty = get_screen_y(positive_y);
    by = get_screen_y(-negative_y);

    if((by - ty) < parentWidget()->height()){
        origin_y = ((parentWidget()->height() - (by - ty))/2) + (positive_y*factor);
    }
    else if(ty > 0) {
        origin_y -= ty;
    }
    else if(by < parentWidget()->height()) {
        origin_y += (parentWidget()->height() - by -1);
    }
    //qDebug()<<"move_origin_y end";
}


int  Jasu::move_point(QPainter *p, int x, int y)
{
    int tmp_x, tmp_y;
    int moved_origin = 0;

    //    if(x < -negative_x || x > positive_x) return 1;
    //    if(y < -negative_y || y > positive_y) return 1;


    cursor_x = x;
    cursor_y = y;

    //qDebug()<<"move point cursor_x"<<cursor_x<<"cursor_y"<<cursor_y;

    //qDebug() << "move_point 2";

    tmp_x = get_screen_x(x);
    //    if(tmp_x < 0 || tmp_x > parentWidget()->width()){
    //        move_origin_x(tmp_x);
    //        moved_origin = 1;
    //    }

    tmp_y = get_screen_y(y);
    //    if(tmp_y < 0 || tmp_y > parentWidget()->height()){
    //        move_origin_y(tmp_y);
    //        moved_origin = 1;
    //    }

    //qDebug() << "move_point 3";

    if(moved_origin){
        //quick_draw(p, total_stitch);
        //        tmp_x = get_screen_x(x);
        //        tmp_y = get_screen_y(y);
    }

    current_screen_x = tmp_x;
    current_screen_y = tmp_y;
    //qDebug()<<"move_point current screent x:"<< current_screen_x<<"y:"<<current_screen_y;

    return 0;
}

void Jasu::move_cursor(int x, int y, char type, char col)
{
    cursor_color = col;
    //qDebug() << "move_cursor start color:" << col << "cursor_color" << cursor_color;

    QPainter gr_painter(&gr_pixmap);

    //qDebug() << "move_cursor start";

    if(type == ABSXY)	move_point(&gr_painter, x, y);
    else        		move_point(&gr_painter, cursor_x+x, cursor_y+y);

    update();
}

void Jasu::move_cursor(__u32 mc_stitch)
{


    QPainter gr_painter(&gr_pixmap);
    //move_point(&gr_painter, stitch_list[mc_stitch].x, stitch_list[mc_stitch].y);

    //move_point(&gr_painter, 283, 281);//original point

    //qDebug()<<w->stitch_list[0].can_value1;
    //qDebug()<<w->stitch_list[0].can_value2;
    move_point(&gr_painter, w->stitch_list[mc_stitch].can_value1 ,w->stitch_list[mc_stitch].can_value2);
    //move_point(&gr_painter, w->stitch_list[mc_stitch].can_value1 ,w->stitch_list[mc_stitch].can_value5);




    //qDebug() << "move_cursor : " << mc_stitch;

    //update();
}

int  Jasu::set_mc_color(char attr)
{
    switch(attr){
    case 0  :
        return _TRIM;
    case 1  :
        return _SET_ORG;    // 8
    case 2  :
        return _PAUSE;
    case 4	:
        return -1;
    case 12 :
        return _STI_SPD;
    case 49 :               //(0x31 rev)  //(121212)
        return _REV;
    case 56 :
        return _SCL_REF;
    case 57 :
        return _SET_OPnn;
    case 58 :
        return _CHK_IPnn;
    case 59 :
        return _TIME_DELAY;
    case 60 :
        return _PF_CONTROL;
    case 61 :
        return _CAN_OPnn;
    case 62 :
        return _CAN_IPnn;
    }
    return 0;
}


//void Jasu::show_zoom(int updown)
//{
//    zoom_level += updown;

//    if (updown == 0 || zoom_level < 1)		zoom_level = 1;

//    if (zoom_level > 100)		zoom_level = 100;

//    factor = scale * zoom_level;

//    move_origin_x(get_screen_x(cursor_x));
//    move_origin_y(get_screen_y(cursor_y));

//    QPainter gr_painter(&gr_pixmap);
//    quick_draw(&gr_painter, total_stitch);
//    move_point(&gr_painter, cursor_x, cursor_y);
//    update();
//}

void Jasu::show_zoom(int updown)
{
    zoom_level += updown;
    if (updown == 0 || zoom_level < 1)		zoom_level = 1;
    if (zoom_level > 100)		zoom_level = 100;


    factor = scale * zoom_level;

    move_origin_x(get_screen_x(cursor_x));
    move_origin_y(get_screen_y(cursor_y));

    QPainter gr_painter(&gr_pixmap);

    //quick_draw(&gr_painter, total_stitch);
    quick_draw(&gr_painter, total_stitch);

    move_point(&gr_painter, cursor_x, cursor_y);
    update();
}

void Jasu::show_zoom_new(int updown,int tot_stitch)
{
    zoom_level += updown;
    if (updown == 0 || zoom_level < 1)		zoom_level = 1;
    if (zoom_level > 100)		zoom_level = 100;

    factor = scale * zoom_level;

    move_origin_x(get_screen_x(cursor_x));
    move_origin_y(get_screen_y(cursor_y));

    QPainter gr_painter(&gr_pixmap);

    current_total_stitch=tot_stitch;

    //quick_draw(&gr_painter, total_stitch);
    quick_draw(&gr_painter, tot_stitch);

    move_point(&gr_painter, cursor_x, cursor_y);
    update();
}


void Jasu::draw_cursor(QPainter *p)
{
    p->setPen(QPen(Qt::blue, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p->drawLine(current_screen_x-12, current_screen_y, current_screen_x+12, current_screen_y);
    p->drawLine(current_screen_x, current_screen_y-12, current_screen_x, current_screen_y+12);
}


void Jasu::draw_cursor_(QPainter *p, unsigned char color)   //(130219)
{
    p->setPen(QPen(Qt_Color[color], 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p->drawLine(current_screen_x-12, current_screen_y, current_screen_x+12, current_screen_y);
    p->drawLine(current_screen_x, current_screen_y-12, current_screen_x, current_screen_y+12);
}


void Jasu::change_color(int col)
{
    current_color = col;

    QPainter gr_painter(&gr_pixmap);

    quick_draw(&gr_painter, total_stitch);
    move_point(&gr_painter, cursor_x, cursor_y);
    update();
}


void Jasu::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, gr_pixmap);

    //if(!thumbnail_mode)	draw_cursor(&painter);
#if 0 //remove cursor
    if(!thumbnail_mode)     draw_cursor_(&painter, cursor_color);   //(130219)
#endif
}


void Jasu::draw_circle(int x, int y, unsigned char type, unsigned char color, unsigned char size)     //(121215)
{
    if(type == ABSXY)	cordinatation(x*2, y*2);
    else        		cordinatation(cursor_x + x*2, cursor_y + y*2);

    QPainter gr_painter(&gr_pixmap);     //QPainter *p;
    //gr_painter.setPen(QPen(Qt::blue, size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
    gr_painter.drawPoint(current_screen_x, current_screen_y);
    //update();     //(130313)
}






























void Jasu::draw_line(int x, int y, unsigned char type, unsigned char size)     //(121215)
{
    if(type == ABSXY)	cordinatation(x*2, y*2);
    else        		cordinatation(cursor_x + x*2, cursor_y + y*2);

    QPainter gr_painter(&gr_pixmap);

    QLineF line(prev_screen_x, prev_screen_y, current_screen_x, current_screen_y);

    gr_painter.setPen(QPen(color[_GRAY], 0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    gr_painter.drawLine(line);

    prev_screen_x = current_screen_x;
    prev_screen_y = current_screen_y;
}




void Jasu::draw_code(unsigned char code)        //(130306)
{
    //qDebug() <<"draw code start";
    QPainter gr_painter(&gr_pixmap);

    QString str;
    str.sprintf("%s", IMAGE_PATH);      // :/images/icon
    str.append("/icon/");     //
    //str.sprintf("%s", IMAGE_ICON_PATH);

    if(code == TRIM){               // 0
        str.append("code_trim.png");
    }
    else if(code == SEC_ORG){       // 1
        str.append("code_2nd.png");
    }
    else if(code == PAUSE){         // 2
        str.append("code_pause.png");
        //qDebug() << "(draw_code) pause";
    }
    else if(code == EMPTY){         // 3
        return;
    }
    else if(code == STI_SPD){       // 12 kimkt(130511)
        str.append("code_output.png");
    }
    else if(code == REVERSE_SET){   // 49     //(130511)
        str.append("code_rev.png");
    }
    else if(code == SET_OPnn){      // 57  kimkt(130624)
        str.append("code_output.png");
    }
    else if(code == TIME_DELAY){    // 59
        str.append("code_delay.png");
    }
    else if(code == TR3_SET){       // 63
        str.append("code_tr3.png");
    }
    else if(code == PF_CONTROL){    // 60
        str.append("code_pf_dn.png");
        //qDebug() << "(draw_code) code_pf_dn.png";
    }
    //#ifdef SPS_1507				//(121210)
    //    #define	DEV_BACK_S1507	66
    //#endif

    QPixmap img(str);
    gr_painter.drawPixmap(current_screen_x, current_screen_y + 6, img);

    update();       //(130312)
}


void Jasu::draw_code_abs(unsigned char code, int dx, int dy)        //(130511)
{
    //qDebug() <<"draw code start";
    QPainter gr_painter(&gr_pixmap);

    QString str;
    str.sprintf("%s", IMAGE_PATH);      // :/images/icon
    str.append("/icon/");     //
    //str.sprintf("%s", IMAGE_ICON_PATH);

    if(code == TRIM){               // 0
        str.append("code_trim.png");
    }
    else if(code == SEC_ORG){       // 1
        str.append("code_2nd.png");
    }
    else if(code == PAUSE){         // 2
        str.append("code_pause.png");
        //qDebug() << "(draw_code) pause";
    }
    else if(code == EMPTY){         // 3
        return;
    }
    else if(code == STI_SPD){       // 12 kimkt(130511)
        str.append("code_output.png");
    }
    else if(code == REVERSE_SET){   // 49     //(130511)
        str.append("code_rev.png");
    }
    else if(code == SET_OPnn){      // 57  kimkt(130624)
        str.append("code_output.png");
    }
    else if(code == TIME_DELAY){    // 59
        str.append("code_delay.png");
    }
    else if(code == TR3_SET){       // 63
        str.append("code_tr3.png");
    }
    else if(code == PF_CONTROL){    // 60
        str.append("code_pf_dn.png");
        //qDebug() << "(draw_code_abs) code_pf_dn.png";
    }
    //#ifdef SPS_1507				//(121210)
    //    #define	DEV_BACK_S1507	66
    //#endif

    cordinatation(dx, dy);

    QPixmap img(str);
    gr_painter.drawPixmap(current_screen_x, current_screen_y + 6, img);
    update();       //(130312)
}


void Jasu::draw_jump_line(int x, int y, unsigned char type, unsigned char color, unsigned char size)     //(130418)
{
    if(type == ABSXY)	cordinatation(x*2, y*2);
    else        		cordinatation(cursor_x + x*2, cursor_y + y*2);

    QPainter gr_painter(&gr_pixmap);     //QPainter *p;

    //gr_painter.setPen(QPen(Qt_Color[color], size, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
    gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130419)
    gr_painter.drawPoint(current_screen_x, current_screen_y);
    //update();     //(130313)
}



void Jasu::draw_line2(int x, int y, unsigned char type, unsigned char size)
{
    //if(type == ABSXY)	cordinatation(x, y);
    /*else*/        		cordinatation(cursor_x + x, cursor_y + y);

    //qDebug()<<"current_screen_x"<<current_screen_x<<"current_screen_y"<<current_screen_y;


    QPainter gr_painter(&gr_pixmap);

    QLineF line(prev_screen_x, prev_screen_y, current_screen_x, current_screen_y);

    gr_painter.setPen(QPen(color[_GRAY], 0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    gr_painter.drawLine(line);

    prev_screen_x = current_screen_x;
    prev_screen_y = current_screen_y;
}





void Jasu::relxy_origin()       //(130425)
{
    //qDebug() << "relxy_origin()";
    stitch_list_init();

    current_color  = _RED;
    cursor_color   = _BLUE;
    //thumbnail_mode = mode;
    zoom_level     = 1;
    cursor_x       = 0;
    cursor_y       = 0;
}


void Jasu::move_jump(int x, int y, unsigned char type, unsigned char color, unsigned char size)     //(130503)
{
    if(type == ABSXY)	cordinatation(x, y);
    else        		cordinatation(cursor_x + x, cursor_y + y);

    QPainter gr_painter(&gr_pixmap);     //QPainter *p;

    //gr_painter.setPen(QPen(Qt_Color[color], size, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
    gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130419)
    gr_painter.drawPoint(current_screen_x, current_screen_y);
    //update();     //(130313)
}


















































///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////s t a r t     //////////////////////////////////////////////////////







//-- outof range, store data --
int Jasu::cordinatation_(int x, int y)   //draw line parkjd
{
    //    if(DEBUG_CD_DATA)   qDebug() << "move_point start";
    //qDebug() << "move_point start";
    int tmp_x, tmp_y;
    int moved_origin = 0;

    //if(x < -negative_x || x > positive_x) return 1;
    //if(y < -negative_y || y > positive_y) return 1;
    cursor_x = x;
    cursor_y = y;
    //qDebug() << "move_point 2";

    tmp_x = get_screen_x(x);
    tmp_y = get_screen_y(y);

    //    if(tmp_x < 0){   //if(tmp_x < 0 || tmp_x > parentWidget()->width()){
    //        //qDebug() << "tmp_x=" << tmp_x;
    //        move_origin_x(tmp_x);
    //        moved_origin = 1;
    //        //qDebug() << "1.1 x move_origin";
    //    }

    //    if(tmp_y < 0){  //if(tmp_y < 0 || tmp_y > parentWidget()->height()){
    //        //qDebug() << "tmp_y=" << tmp_y;
    //        move_origin_y(tmp_y);
    //        moved_origin = 1;
    //        //qDebug() << "1.2 y move_origin";
    //    }
    //    //qDebug() << "move_point 3";

    //    if(moved_origin){
    //        //quick_draw(p, total_stitch);
    //        tmp_x = get_screen_x(x);
    //        tmp_y = get_screen_y(y);
    //        //qDebug() << "1.3 move_origin 100";
    //    }

    current_screen_x = tmp_x;
    current_screen_y = tmp_y;
    //    if(DEBUG_CD_DATA)   qDebug() << "move_point end";
    //qDebug() << "move_point 4";

    //    if(x < -negative_x || x > positive_x) return 1;     //move kimkt(130617)
    //    if(y < -negative_y || y > positive_y) return 1;

    return 0;
}



void Jasu::draw_circle2(int x, int y, unsigned char type, unsigned char color, unsigned char size)     //draw line parkjd
{

    // if(type == ABSXY){
    //     if(cordinatation_(x, y) == 1){
    //        qDebug() << "d_c2 return  1.1";
    //       return;
    //   }
    // }
    // else{
    /*if(*/cordinatation_(cursor_x + x, cursor_y + y);  /* == 1){*/
    //        qDebug() << "d_c2 return  1.2";
    //       return;
    //  }
    //}
    //qDebug()<<"cursor_x"<<cursor_x<<"cursor_y"<<cursor_y;
    QPainter gr_painter(&gr_pixmap);     //QPainter *p;
    //gr_painter.setPen(QPen(Qt::blue, size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)

    if(current_screen_x>0 && current_screen_y>0)
    {

        //qDebug()<<"==========drow design_screen x : "<<current_screen_x<<"y : "<<current_screen_y;
        gr_painter.drawPoint(current_screen_x, current_screen_y);
    }

    //update();     //(130313)
}





int Jasu::cordinatation(int x, int y) //draw jump parkjd
{
    int tmp_x, tmp_y;
    int moved_origin = 0;

    //if(x < -negative_x || x > positive_x) return 1;
    //if(y < -negative_y || y > positive_y) return 1;

    cursor_x = x;
    cursor_y = y;

    tmp_x = get_screen_x(x);
    //    if(tmp_x < 0 || tmp_x > parentWidget()->width()){
    //        move_origin_x(tmp_x);
    //        moved_origin = 1;
    //    }

    tmp_y = get_screen_y(y);
    //    if(tmp_y < 0 || tmp_y > parentWidget()->height()){
    //        move_origin_y(tmp_y);
    //        moved_origin = 1;
    //    }

    //    if(moved_origin){
    //        //quick_draw(p, total_stitch);
    //        tmp_x = get_screen_x(x);
    //        tmp_y = get_screen_y(y);
    //    }

    current_screen_x = tmp_x;
    current_screen_y = tmp_y;

    //qDebug()<<"current_screen_x"<<current_screen_x<<"current_screen_y"<<current_screen_y;

    return 0;
}

















void Jasu::draw_jump_line2(int x, int y, unsigned char color, unsigned char size)     // moving draw jump parkjd
{
    //qDebug()<<"111 cursor_x :"<<cursor_x<<"x:"<<x<<" cursor_y : "<<cursor_y<<" y : "<<y;
    cordinatation(cursor_x + x, cursor_y + y);
    //qDebug()<<"222 cursor_x :"<<cursor_x<<"x:"<<x<<" cursor_y : "<<cursor_y<<" y : "<<y;

    //qDebug()<<"origin x?"<<origin_x<<"origin y?"<<origin_y;


    QPainter gr_painter(&gr_pixmap);     //QPainter *p;

    //gr_painter.setPen(QPen(Qt_Color[color], size, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
    gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130419)


    //qDebug()<<"current_screen_x"<<current_screen_x<<"current_screen_y"<<current_screen_y;
    if(current_screen_x>0 && current_screen_y>0)
    {
        gr_painter.drawPoint(current_screen_x, current_screen_y);

        //qDebug()<<"//////////draw_jump_line2 current_screen x : "<<current_screen_x<<"y : "<<current_screen_y;
        //w->zoom_save_fg=false;
    }

    //update();     //(130313)
}
void Jasu::color_draw_jump_line2(int x, int y, unsigned char color, unsigned char size,bool save,bool operation)     // moving draw jump parkjd
{
    //qDebug()<<"111 cursor_x :"<<cursor_x<<"x:"<<x<<" cursor_y : "<<cursor_y<<" y : "<<y;
    cordinatation(cursor_x + x, cursor_y + y);
    //qDebug()<<"222 cursor_x :"<<cursor_x<<"x:"<<x<<" cursor_y : "<<cursor_y<<" y : "<<y;

    //qDebug()<<"origin x?"<<origin_x<<"origin y?"<<origin_y;

    if(save)
    {
        set_jump_origin_x=current_screen_x;
        set_jump_origin_y=current_screen_y;
    }

    if(operation)
    {
        QPainter gr_painter(&gr_pixmap);     //QPainter *p;

        //gr_painter.setPen(QPen(Qt_Color[color], size, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
        gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130419)


        //qDebug()<<"current_screen_x"<<current_screen_x<<"current_screen_y"<<current_screen_y;
        if(current_screen_x>0 && current_screen_y>0)
        {
            QLineF line(set_jump_origin_x,set_jump_origin_y,current_screen_x,current_screen_y);
            //gr_painter.drawPoint(current_screen_x, current_screen_y);
            gr_painter.drawLine(line);
            //qDebug()<<"//////////draw_jump_line2 current_screen x : "<<current_screen_x<<"y : "<<current_screen_y;
            //w->zoom_save_fg=false;
        }
    }
    //update();     //(130313)
}































void Jasu::zoom_move_origin_point(int x, int y,int custom_zoom_level)
{
    //qDebug() << "set_sewing_area" << nx << px << ny << py <<  (nx+px)/10 << (ny+py)/10;
    zoom_level = custom_zoom_level;
    cursor_x=x;
    cursor_y=y;
    negative_x = 400 * 20;
    positive_x = 400 * 20;
    positive_y = 400 * 20;
    negative_y = 400 * 20;

    //draw_sewing_area();
}


int Jasu::zoom_cordinatation(int x, int y)
{
    //    if(DEBUG_CD_DATA)   qDebug() << "move_point start";
    //qDebug() << "move_point start";
    int tmp_x, tmp_y;
    int moved_origin = 0;

    if(x < -negative_x || x > positive_x) return 1;
    if(y < -negative_y || y > positive_y) return 1;

    cursor_x = x;
    cursor_y = y;
    //qDebug() << "move_point 2";

    tmp_x = get_screen_x(x);
    if(tmp_x < 0 || tmp_x > parentWidget()->width()){
        move_origin_x(tmp_x);
        moved_origin = 1;
        //qDebug() << "x move_origin";
    }

    tmp_y = get_screen_y(y);
    if(tmp_y < 0 || tmp_y > parentWidget()->height()){
        move_origin_y(tmp_y);
        moved_origin = 1;
        //qDebug() << "y move_origin";
    }
    //qDebug() << "move_point 3";

    if(moved_origin){
        //quick_draw(p, total_stitch);
        tmp_x = get_screen_x(x);
        tmp_y = get_screen_y(y);
        qDebug() << "move_origin 100";
    }
    current_screen_x = tmp_x;
    current_screen_y = tmp_y;
    //    if(DEBUG_CD_DATA)   qDebug() << "move_point end";
    //qDebug() << "move_point 4";

    return 0;
}





void Jasu::zoom_set_sewing_area(__u16 nx, __u16 px, __u16 ny, __u16 py,int custom_zoom_level,int cur_x,int cur_y)
{
    cur_cursor_x=cur_x;
    cur_cursor_y=cur_y;


    //qDebug() << "set_sewing_area" << nx << px << ny << py <<  (nx+px)/10 << (ny+py)/10;
    zoom_level = custom_zoom_level;

    negative_x = nx * 20;
    positive_x = px * 20;
    positive_y = ny * 20;
    negative_y = py * 20;

    zoom_draw_sewing_area();
}


void Jasu::zoom_draw_sewing_area(void)
{
    QPainter gr_painter(&gr_pixmap);

    //qDebug() << "(draw_sewing_area) start";

    __u16   w, h;
    float x, y, tmp1, tmp2;

    w = negative_x + positive_x;
    h = negative_y + positive_y;

    x = (float)parentWidget()->width();
    y = (float)parentWidget()->height();

    tmp1 = x / (float)w;
    tmp2 = y / (float)h;


    (tmp1 < tmp2) ? zoom_set_factor(tmp1) : zoom_set_factor(tmp2);
    //(tmp1 < tmp2) ? set_factor_xyscale(tmp1) : set_factor_xyscale(tmp2);


    zoom_get_origin_point(w, h);       //get origin point in jasu label

    draw_origin(&gr_painter);

    //move_point(&gr_painter, 0, 0);

    //qDebug()<<"111move_point ::"<<origin_x<<"   "<<origin_y;
    move_point(&gr_painter, origin_x, origin_y);
    //qDebug()<<"change origin ::"<<origin_x<<"   "<<origin_y;

    //qDebug() << "(draw_sewing_area) end";
}


void Jasu::zoom_set_factor(float sc)
{
    //qDebug()<<"zoom_level"<<zoom_level;
    scale  = sc;
    //zoom_level += 1;
    //factor = scale * w->custom_zoom_level;

    factor = scale * zoom_level;
    //factor = scale * (1 + (float)(zoom_level/5));




   qDebug() << "scale:" << scale << "zoom:" << zoom_level << "factor:" << factor;
    // zoom in, zoom out : seperate x, y
}

void Jasu::zoom_get_origin_point(__u16 w, __u16 h)
{
    //qDebug()<<"cur_cursor_x"<<cur_cursor_x;
    //qDebug()<<"cur_cursor_y"<<cur_cursor_y;


    if(zoom_level==1){
        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);




    }else if(zoom_level==2){
#if 0
        //qDebug()<<"111origin_x"<<origin_x<<"origin_y"<<origin_y;
        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        //        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        //        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        origin_x += origin_x-(cur_cursor_x-214); //
        origin_y += origin_y-(cur_cursor_y-21);  //
        //qDebug()<<"zoom_origin_x"<<origin_x<<"zoom_origin_y"<<origin_y;






        //qDebug()<<"cur_cursor_x-214 :::"<<cur_cursor_x-214<<"cur_cursor_y-21   :::"<<cur_cursor_y-21;

        //        qDebug()<<"parentWidget()->width()"<<parentWidget()->width();
        //        qDebug()<<"w"<<w;
        //        qDebug()<<"factor"<<factor;
        //        qDebug()<<"negative_x"<<negative_x;
        //        qDebug()<<"factor"<<factor;


        //        qDebug()<<"(parentWidget()->width() - w*factor)/2"<<(parentWidget()->width() - w*factor)/2;
        //        qDebug()<<"negative_x*factor"<<negative_x*factor;

#else
#if 0
        //check
        //qDebug()<<"111origin_x"<<origin_x<<"origin_y"<<origin_y;
        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        //        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        //        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        //origin_x += origin_x-(cur_cursor_x-214); //
        //origin_y += origin_y-(cur_cursor_y-21);  //

        origin_x = origin_x + (int)(cur_cursor_x*factor); //
        origin_y = origin_y - (int)(cur_cursor_y*factor);  //
#endif
        //thisthisthisthis!!!
        //qDebug()<<"111origin_x"<<origin_x<<"origin_y"<<origin_y;
        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        //        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        //        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        origin_x += origin_x-(cur_cursor_x-214); //
        origin_y += origin_y-(cur_cursor_y-21);  //
        //qDebug()<<"zoom_origin_x"<<origin_x<<"zoom_origin_y"<<origin_y;




#endif


    }else if(zoom_level==6){
        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);
        //        origin_x = ((parentWidget()->width() - w*factor)/2) + (negative_x*factor);
        //        origin_y = ((parentWidget()->height() - h*factor)/2) + (positive_y*factor);


        // check
        //        origin_x += origin_x-(cur_cursor_x-214); //
        //        origin_y += origin_y-(cur_cursor_y-21);  //
        //////////////////////////////////////////////////////////////////////////////
        //        int  Jasu::get_screen_x(int x)
        //        {

        //            return origin_x + (int)(x * factor);
        //        }

        origin_x += origin_x-(cur_cursor_x-214); //
        origin_y += origin_y-(cur_cursor_y-21);  //


        //qDebug()<<"origin_x"<<origin_y<<"origin_x"<<origin_y;

        //        qDebug()<<"parentWidget()->width()"<<parentWidget()->width();
        //        qDebug()<<"w"<<w;
        //        qDebug()<<"factor"<<factor;
        //        qDebug()<<"negative_x"<<negative_x;
        //        qDebug()<<"factor"<<factor;


        //        qDebug()<<"(parentWidget()->width() - w*factor)/2"<<(parentWidget()->width() - w*factor)/2;
        //        qDebug()<<"negative_x*factor"<<negative_x*factor;

    }






    //qDebug()<<"333origin_x"<<origin_x<<"origin_y"<<origin_y;



}

void Jasu::move_zoom_point(int zoom_x,int zoom_y)
{

    //qDebug()<<"move zoom point st";
    QPainter gr_painter(&gr_pixmap);
    //move_point(&gr_painter, stitch_list[mc_stitch].x, stitch_list[mc_stitch].y);

    //move_point(&gr_painter, 283, 281);//original point

    //qDebug()<<w->stitch_list[0].can_value1;
    //qDebug()<<w->stitch_list[0].can_value2;







    //    qDebug()<< "x: "<<w->stitch_list[0].can_value1<< "y: "<<w->stitch_list[0].can_value2;

    //    move_point(&gr_painter, w->stitch_list[0].can_value1 ,w->stitch_list[0].can_value2);

    //qDebug()<< "x: "<<zoom_x<< "y: "<<zoom_y;

    // move_point(&gr_painter, zoom_x ,zoom_y);
    move_point(&gr_painter, get_screen_x(0) ,get_screen_y(0));
    //qDebug()<<"zoom move : x:"<<get_screen_x(0)<<"y:"<<get_screen_y(0);







    //move_point(&gr_painter, w->stitch_list[mc_stitch].can_value1 ,w->stitch_list[mc_stitch].can_value5);
    //qDebug()<<"move zoom point end";




    //qDebug() << "move_cursor : " << mc_stitch;

    //update();
}

#if 1
void Jasu::clear_warning_point()
{
    warnig_position.clear();
}

void Jasu::drow_icon(bool painter, int cur)
{
    qDebug()<<"drow icon st===================================================";
    if(painter)
    {
        qDebug()<<"insert cur :"<<cur;
        warnig_position.insert(warnig_position.count(),cur);
        //qDebug()<<warnig_position.count();
        //qDebug()<<warnig_position.count();
    }

    //get_screen_x(w->stitch_list[i].can_value1);

    QPainter gr_painter(&gr_pixmap);

    gr_painter.setPen(QPen(Qt::red,3,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int count;
    int second_count;
    second_count=0;
    count=0;
    int cus_x; cus_x=0;//cus_x=origin_x;
    int cus_y; cus_y=0;//cus_y=origin_y;
    int t_x; int t_y;
    int temp_x;
    temp_x=0;
    int temp_y;
    temp_y=0;
    int result_x;
    int result_y;

    int cursor_x;cursor_x=0;
    int cursor_y;cursor_y=0;
    int storage_x=0;int storage_y=0;



    if(warnig_position.count()!=0)
    {
        for(second_count=0; second_count < warnig_position.count() ; second_count++)
        {
            for(count=0;count<w->stitch_list.count();count++)
            {

                t_x = w->stitch_list[count].can_value1;
                t_y = w->stitch_list[count].can_value2;

                if(w->stitch_list[count].can_value3 & 0x01) {t_x = -t_x;}
                if(w->stitch_list[count].can_value3 & 0x02) {t_y = -t_y;}

                cursor_x = cursor_x + t_x;
                cursor_y = cursor_y + t_y;


                if(count==warnig_position.value(second_count))
                {
                    storage_x = origin_x + (int)(cursor_x * factor);
                    storage_y = origin_y - (int)(cursor_y * factor);

                    if(w->custom_zoom_level == 2 || w->custom_zoom_level == 3)
                    {
                        storage_x += (int)(origin_x * factor);
                        storage_y -= (int)(origin_y * factor);
                    }


                    QPointF p1(storage_x,storage_y);
                    gr_painter.drawEllipse(p1, 4, 4);
                    cursor_x=0;cursor_y=0;

                    break;
                }
            }
            cus_x=0;
            cus_y=0;
        }
    }else{
        //qDebug()<<"else-> warnig_position.count()"<<warnig_position.count();
    }

    //qDebug()<<"origin_x"<<origin_x<<"origin_y"<<origin_y;
    update();

    //qDebug()<<"drow icon end===================================================";

}
#else

void Jasu::drow_icon(bool painter, int cur)
{
    qDebug()<<"drow icon st===================================================";
    if(painter)
    {
        qDebug()<<"insert cur :"<<cur;
        warnig_position.insert(warnig_position.count(),cur);
        //qDebug()<<warnig_position.count();
        //qDebug()<<warnig_position.count();
    }

    //get_screen_x(w->stitch_list[i].can_value1);

    QPainter gr_painter(&gr_pixmap);

    gr_painter.setPen(QPen(Qt::red,2,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int count;
    int second_count;
    second_count=0;
    count=0;
    int cus_x; cus_x=0;//cus_x=origin_x;
    int cus_y; cus_y=0;//cus_y=origin_y;
    int t_x; int t_y;
    int temp_x;
    temp_x=0;
    int temp_y;
    temp_y=0;
    int result_x;
    int result_y;


    if(warnig_position.count()!=0)
    {
        for(second_count=0; second_count < warnig_position.count() ; second_count++)
        {
            for(count=0;count<w->stitch_list.count();count++)
            {

                t_x = w->stitch_list[count].can_value1;
                t_y = w->stitch_list[count].can_value2;

                if(w->stitch_list[count].can_value3 & 0x01) t_x = -t_x;
                if(w->stitch_list[count].can_value3 & 0x02) t_y = -t_y;

                if(count==warnig_position.value(second_count))
                {
                    temp_x = origin_x + ((int)cus_x * factor);
                    temp_y = origin_y - ((int)cus_y * factor);

                    if(zoom_level==1)
                    {
                        QPointF p1(temp_x,temp_y);
                        gr_painter.drawEllipse(p1, 3, 3);
                    }else if(zoom_level==2)
                    {
                        //result_x=get_screen_x(temp_x);
                        //result_y=get_screen_y(temp_y);





                        QPointF p1(result_x,result_y);

                        gr_painter.drawEllipse(p1, 3, 3);
                    }

                    qDebug()<<"draw icon origin_x:"<<origin_x<<"origin_y"<<origin_y<<"temp_x:"<<temp_x<<"y:"<<temp_y<<"result_x"<<result_x<<"result_y"<<result_y;

                    break;
                }else{
                    cus_x = cus_x + t_x;
                    cus_y = cus_y + t_y;
                    //break;
                }
            }
            cus_x=0;
            cus_y=0;
        }
    }else{
        qDebug()<<"else-> warnig_position.count()"<<warnig_position.count();
    }
    update();

    qDebug()<<"drow icon end===================================================";

}
#endif
void Jasu::history_drow_icon()
{
    QPainter gr_painter(&gr_pixmap);

    //    QString str;
    //    str.sprintf(":/new_images/warning.png");

    if(warnig_position.count() !=0)
    {
#if 0
        QPainter gr_painter(&gr_pixmap);     //QPainter *p;
        gr_painter.drawEllipse(QPointF(x,y), radius, radius);

        //gr_painter.setPen(QPen(Qt_Color[color], size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));     //(130217)
        gr_painter.drawPoint(current_screen_x, current_screen_y);
#endif

        gr_painter.setPen(QPen(Qt::red,2,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        for(int num=1;num < warnig_position.count()+1;num++)
        {
            //        QPixmap img(str);
            //        //gr_painter.drawPixmap(current_screen_x-21, current_screen_y-42, img);
            //        gr_painter.drawPixmap(get_screen_x(cur)-21, get_screen_y(cur)-42, img);
            //qDebug()<<warnig_position.value(num);
            gr_painter.drawEllipse(QPointF(get_screen_x(warnig_position.value(num)),get_screen_y(warnig_position.value(num))), 10, 10);
        }

        update();
    }
}



////////////////////////////////e      n        d      ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////




























