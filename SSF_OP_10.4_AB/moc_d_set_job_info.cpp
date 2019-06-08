/****************************************************************************
** Meta object code from reading C++ file 'd_set_job_info.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "d_set_job_info.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'd_set_job_info.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_D_set_job_info[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      37,   15,   15,   15, 0x08,
      50,   15,   15,   15, 0x08,
      70,   15,   15,   15, 0x08,
      90,   15,   15,   15, 0x08,
     109,   15,   15,   15, 0x08,
     131,   15,   15,   15, 0x08,
     153,   15,   15,   15, 0x08,
     175,   15,   15,   15, 0x08,
     197,   15,   15,   15, 0x08,
     219,   15,   15,   15, 0x08,
     241,   15,   15,   15, 0x08,
     263,   15,   15,   15, 0x08,
     285,   15,   15,   15, 0x08,
     307,   15,   15,   15, 0x08,
     330,   15,   15,   15, 0x08,
     353,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_D_set_job_info[] = {
    "D_set_job_info\0\0on_b_close_clicked()\0"
    "rcvd_value()\0on_b_scan_clicked()\0"
    "on_b_next_clicked()\0show_next_button()\0"
    "on_process1_clicked()\0on_process2_clicked()\0"
    "on_process3_clicked()\0on_process4_clicked()\0"
    "on_process5_clicked()\0on_process6_clicked()\0"
    "on_process7_clicked()\0on_process8_clicked()\0"
    "on_process9_clicked()\0on_process10_clicked()\0"
    "on_b_replace_clicked()\0"
    "on_b_doNotReplace_clicked()\0"
};

void D_set_job_info::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        D_set_job_info *_t = static_cast<D_set_job_info *>(_o);
        switch (_id) {
        case 0: _t->on_b_close_clicked(); break;
        case 1: _t->rcvd_value(); break;
        case 2: _t->on_b_scan_clicked(); break;
        case 3: _t->on_b_next_clicked(); break;
        case 4: _t->show_next_button(); break;
        case 5: _t->on_process1_clicked(); break;
        case 6: _t->on_process2_clicked(); break;
        case 7: _t->on_process3_clicked(); break;
        case 8: _t->on_process4_clicked(); break;
        case 9: _t->on_process5_clicked(); break;
        case 10: _t->on_process6_clicked(); break;
        case 11: _t->on_process7_clicked(); break;
        case 12: _t->on_process8_clicked(); break;
        case 13: _t->on_process9_clicked(); break;
        case 14: _t->on_process10_clicked(); break;
        case 15: _t->on_b_replace_clicked(); break;
        case 16: _t->on_b_doNotReplace_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData D_set_job_info::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject D_set_job_info::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_D_set_job_info,
      qt_meta_data_D_set_job_info, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &D_set_job_info::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *D_set_job_info::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *D_set_job_info::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_D_set_job_info))
        return static_cast<void*>(const_cast< D_set_job_info*>(this));
    if (!strcmp(_clname, "QPalette"))
        return static_cast< QPalette*>(const_cast< D_set_job_info*>(this));
    return QDialog::qt_metacast(_clname);
}

int D_set_job_info::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
