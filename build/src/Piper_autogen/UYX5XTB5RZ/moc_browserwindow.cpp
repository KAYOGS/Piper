/****************************************************************************
** Meta object code from reading C++ file 'browserwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ui/browserwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browserwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13BrowserWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13BrowserWindowE = QtMocHelpers::stringData(
    "BrowserWindow",
    "themeChanged",
    "",
    "changeBackgroundImage",
    "setThemeLight",
    "setThemeDark",
    "setThemeDefault",
    "applyTheme",
    "themeName",
    "goHome",
    "openPrivateWindow",
    "showHistory",
    "showDownloads",
    "handleDownload",
    "QWebEngineDownloadRequest*",
    "download",
    "createNewTab",
    "url",
    "loadSettings",
    "saveSettings",
    "currentTheme"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13BrowserWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       1,  125, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  104,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,  105,    2, 0x0a,    3 /* Public */,
       4,    0,  106,    2, 0x0a,    4 /* Public */,
       5,    0,  107,    2, 0x0a,    5 /* Public */,
       6,    0,  108,    2, 0x0a,    6 /* Public */,
       7,    1,  109,    2, 0x0a,    7 /* Public */,
       9,    0,  112,    2, 0x08,    9 /* Private */,
      10,    0,  113,    2, 0x08,   10 /* Private */,
      11,    0,  114,    2, 0x08,   11 /* Private */,
      12,    0,  115,    2, 0x08,   12 /* Private */,
      13,    1,  116,    2, 0x08,   13 /* Private */,
      16,    1,  119,    2, 0x08,   15 /* Private */,
      16,    0,  122,    2, 0x28,   17 /* Private | MethodCloned */,
      18,    0,  123,    2, 0x08,   18 /* Private */,
      19,    0,  124,    2, 0x08,   19 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::QUrl,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
      20, QMetaType::QString, 0x00015003, uint(0), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject BrowserWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN13BrowserWindowE.offsetsAndSizes,
    qt_meta_data_ZN13BrowserWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13BrowserWindowE_t,
        // property 'currentTheme'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BrowserWindow, std::true_type>,
        // method 'themeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'changeBackgroundImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setThemeLight'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setThemeDark'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setThemeDefault'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'applyTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'goHome'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openPrivateWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showHistory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showDownloads'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDownload'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineDownloadRequest *, std::false_type>,
        // method 'createNewTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'createNewTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void BrowserWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BrowserWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->themeChanged(); break;
        case 1: _t->changeBackgroundImage(); break;
        case 2: _t->setThemeLight(); break;
        case 3: _t->setThemeDark(); break;
        case 4: _t->setThemeDefault(); break;
        case 5: _t->applyTheme((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->goHome(); break;
        case 7: _t->openPrivateWindow(); break;
        case 8: _t->showHistory(); break;
        case 9: _t->showDownloads(); break;
        case 10: _t->handleDownload((*reinterpret_cast< std::add_pointer_t<QWebEngineDownloadRequest*>>(_a[1]))); break;
        case 11: _t->createNewTab((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 12: _t->createNewTab(); break;
        case 13: _t->loadSettings(); break;
        case 14: _t->saveSettings(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEngineDownloadRequest* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (BrowserWindow::*)();
            if (_q_method_type _q_method = &BrowserWindow::themeChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getTheme(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->applyTheme(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *BrowserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BrowserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN13BrowserWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int BrowserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void BrowserWindow::themeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
