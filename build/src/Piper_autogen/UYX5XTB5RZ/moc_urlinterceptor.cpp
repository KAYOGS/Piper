/****************************************************************************
** Meta object code from reading C++ file 'urlinterceptor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ui/urlinterceptor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'urlinterceptor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14UrlInterceptorE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN14UrlInterceptorE = QtMocHelpers::stringData(
    "UrlInterceptor"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN14UrlInterceptorE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject UrlInterceptor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWebEngineUrlRequestInterceptor::staticMetaObject>(),
    qt_meta_stringdata_ZN14UrlInterceptorE.offsetsAndSizes,
    qt_meta_data_ZN14UrlInterceptorE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN14UrlInterceptorE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UrlInterceptor, std::true_type>
    >,
    nullptr
} };

void UrlInterceptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UrlInterceptor *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *UrlInterceptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UrlInterceptor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN14UrlInterceptorE.stringdata0))
        return static_cast<void*>(this);
    return QWebEngineUrlRequestInterceptor::qt_metacast(_clname);
}

int UrlInterceptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEngineUrlRequestInterceptor::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
