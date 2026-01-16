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
struct qt_meta_tag_ZN8HomeViewE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN8HomeViewE = QtMocHelpers::stringData(
    "HomeView"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN8HomeViewE[] = {

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

Q_CONSTINIT const QMetaObject HomeView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN8HomeViewE.offsetsAndSizes,
    qt_meta_data_ZN8HomeViewE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN8HomeViewE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HomeView, std::true_type>
    >,
    nullptr
} };

void HomeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HomeView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *HomeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HomeView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN8HomeViewE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HomeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN13BrowserWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13BrowserWindowE = QtMocHelpers::stringData(
    "BrowserWindow",
    "changeBackgroundImage",
    "",
    "applyDarkTheme",
    "addFavorite",
    "showFavoritesMenu",
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
    "handleUrlEntered",
    "showAbout",
    "showMainMenu",
    "toggleTabSwitcher",
    "checkTabActivity",
    "clearAllTabs"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13BrowserWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  128,    2, 0x0a,    1 /* Public */,
       3,    0,  129,    2, 0x0a,    2 /* Public */,
       4,    0,  130,    2, 0x0a,    3 /* Public */,
       5,    0,  131,    2, 0x0a,    4 /* Public */,
       6,    0,  132,    2, 0x08,    5 /* Private */,
       7,    0,  133,    2, 0x08,    6 /* Private */,
       8,    0,  134,    2, 0x08,    7 /* Private */,
       9,    0,  135,    2, 0x08,    8 /* Private */,
      10,    1,  136,    2, 0x08,    9 /* Private */,
      13,    1,  139,    2, 0x08,   11 /* Private */,
      13,    0,  142,    2, 0x28,   13 /* Private | MethodCloned */,
      15,    0,  143,    2, 0x08,   14 /* Private */,
      16,    0,  144,    2, 0x08,   15 /* Private */,
      17,    0,  145,    2, 0x08,   16 /* Private */,
      18,    0,  146,    2, 0x08,   17 /* Private */,
      19,    0,  147,    2, 0x08,   18 /* Private */,
      20,    0,  148,    2, 0x08,   19 /* Private */,
      21,    0,  149,    2, 0x08,   20 /* Private */,
      22,    0,  150,    2, 0x08,   21 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::QUrl,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject BrowserWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN13BrowserWindowE.offsetsAndSizes,
    qt_meta_data_ZN13BrowserWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13BrowserWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BrowserWindow, std::true_type>,
        // method 'changeBackgroundImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'applyDarkTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addFavorite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showFavoritesMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
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
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleUrlEntered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showMainMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleTabSwitcher'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'checkTabActivity'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearAllTabs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void BrowserWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BrowserWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->changeBackgroundImage(); break;
        case 1: _t->applyDarkTheme(); break;
        case 2: _t->addFavorite(); break;
        case 3: _t->showFavoritesMenu(); break;
        case 4: _t->goHome(); break;
        case 5: _t->openPrivateWindow(); break;
        case 6: _t->showHistory(); break;
        case 7: _t->showDownloads(); break;
        case 8: _t->handleDownload((*reinterpret_cast< std::add_pointer_t<QWebEngineDownloadRequest*>>(_a[1]))); break;
        case 9: _t->createNewTab((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 10: _t->createNewTab(); break;
        case 11: _t->loadSettings(); break;
        case 12: _t->saveSettings(); break;
        case 13: _t->handleUrlEntered(); break;
        case 14: _t->showAbout(); break;
        case 15: _t->showMainMenu(); break;
        case 16: _t->toggleTabSwitcher(); break;
        case 17: _t->checkTabActivity(); break;
        case 18: _t->clearAllTabs(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEngineDownloadRequest* >(); break;
            }
            break;
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
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
