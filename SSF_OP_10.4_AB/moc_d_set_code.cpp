/****************************************************************************
** Meta object code from reading C++ file 'd_set_code.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "d_set_code.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'd_set_code.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_D_set_code[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      33,   11,   11,   11, 0x08,
      56,   11,   11,   11, 0x08,
      77,   11,   11,   11, 0x08,
      96,   11,   11,   11, 0x08,
     114,   11,   11,   11, 0x08,
     127,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_D_set_code[] = {
    "D_set_code\0\0on_b_close_clicked()\0"
    "do_keypad_clicked(int)\0on_b_enter_clicked()\0"
    "on_b_yes_clicked()\0on_b_no_clicked()\0"
    "rcvd_value()\0on_b_enter_2_clicked()\0"
};

void D_set_code::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        D_set_code *_t = static_cast<D_set_code *>(_o);
        switch (_id) {
        case 0: _t->on_b_close_clicked(); break;
        case 1: _t->do_keypad_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_b_enter_clicked(); break;
        case 3: _t->on_b_yes_clicked(); break;
        case 4: _t->on_b_no_clicked(); break;
        case 5: _t->rcvd_value(); break;
        case 6: _t->on_b_enter_2_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData D_set_code::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject D_set_code::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_D_set_code,
      qt_meta_data_D_set_code, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &D_set_code::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *D_set_code::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *D_set_code::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_D_set_code))
        return static_cast<void*>(const_cast< D_set_code*>(this));
    return QDialog::qt_metacast(_clname);
}

int D_set_code::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
