/****************************************************************************
** Meta object code from reading C++ file 'dispatcher.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/dispatcher.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dispatcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN10DispatcherE_t {};
} // unnamed namespace

template <> constexpr inline auto Dispatcher::qt_create_metaobjectdata<qt_meta_tag_ZN10DispatcherE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Dispatcher",
        "firmwareChanged",
        "",
        "ecuId",
        "moduleNumber",
        "firmwareRejected",
        "reason",
        "connectModule",
        "disconnectModule",
        "setMode",
        "mode",
        "setPower",
        "state",
        "selectFirmware",
        "QUrl",
        "fileUrl",
        "firmwareFileName",
        "firmwareSizeBytes",
        "hasFirmware",
        "ecuAModule1",
        "ModuleSession*",
        "ecuAModule2",
        "ecuBModule1",
        "ecuBModule2",
        "signalLoader",
        "SignalFileLoader*",
        "liveSource",
        "LiveSignalSource*",
        "rawSignalLoader",
        "RawSignalLoader*"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'firmwareChanged'
        QtMocHelpers::SignalData<void(const QString &, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Signal 'firmwareRejected'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 6 },
        }}),
        // Method 'connectModule'
        QtMocHelpers::MethodData<void(const QString &, int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Method 'disconnectModule'
        QtMocHelpers::MethodData<void(const QString &, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Method 'setMode'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 10 },
        }}),
        // Method 'setPower'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 12 },
        }}),
        // Method 'selectFirmware'
        QtMocHelpers::MethodData<bool(const QString &, int, const QUrl &)>(13, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { 0x80000000 | 14, 15 },
        }}),
        // Method 'firmwareFileName'
        QtMocHelpers::MethodData<QString(const QString &, int) const>(16, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Method 'firmwareSizeBytes'
        QtMocHelpers::MethodData<qint64(const QString &, int) const>(17, 2, QMC::AccessPublic, QMetaType::LongLong, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Method 'hasFirmware'
        QtMocHelpers::MethodData<bool(const QString &, int) const>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'ecuAModule1'
        QtMocHelpers::PropertyData<ModuleSession*>(19, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'ecuAModule2'
        QtMocHelpers::PropertyData<ModuleSession*>(21, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'ecuBModule1'
        QtMocHelpers::PropertyData<ModuleSession*>(22, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'ecuBModule2'
        QtMocHelpers::PropertyData<ModuleSession*>(23, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'signalLoader'
        QtMocHelpers::PropertyData<SignalFileLoader*>(24, 0x80000000 | 25, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'liveSource'
        QtMocHelpers::PropertyData<LiveSignalSource*>(26, 0x80000000 | 27, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'rawSignalLoader'
        QtMocHelpers::PropertyData<RawSignalLoader*>(28, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Dispatcher, qt_meta_tag_ZN10DispatcherE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Dispatcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DispatcherE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DispatcherE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10DispatcherE_t>.metaTypes,
    nullptr
} };

void Dispatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Dispatcher *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->firmwareChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->firmwareRejected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->connectModule((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->disconnectModule((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->setMode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 5: _t->setPower((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: { bool _r = _t->selectFirmware((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[3])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->firmwareFileName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { qint64 _r = _t->firmwareSizeBytes((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<qint64*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->hasFirmware((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Dispatcher::*)(const QString & , int )>(_a, &Dispatcher::firmwareChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dispatcher::*)(const QString & , int , const QString & )>(_a, &Dispatcher::firmwareRejected, 1))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< LiveSignalSource* >(); break;
        case 3:
        case 2:
        case 1:
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ModuleSession* >(); break;
        case 6:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RawSignalLoader* >(); break;
        case 4:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< SignalFileLoader* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<ModuleSession**>(_v) = _t->ecuAModule1(); break;
        case 1: *reinterpret_cast<ModuleSession**>(_v) = _t->ecuAModule2(); break;
        case 2: *reinterpret_cast<ModuleSession**>(_v) = _t->ecuBModule1(); break;
        case 3: *reinterpret_cast<ModuleSession**>(_v) = _t->ecuBModule2(); break;
        case 4: *reinterpret_cast<SignalFileLoader**>(_v) = _t->signalLoader(); break;
        case 5: *reinterpret_cast<LiveSignalSource**>(_v) = _t->liveSource(); break;
        case 6: *reinterpret_cast<RawSignalLoader**>(_v) = _t->rawSignalLoader(); break;
        default: break;
        }
    }
}

const QMetaObject *Dispatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dispatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10DispatcherE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Dispatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Dispatcher::firmwareChanged(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void Dispatcher::firmwareRejected(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
