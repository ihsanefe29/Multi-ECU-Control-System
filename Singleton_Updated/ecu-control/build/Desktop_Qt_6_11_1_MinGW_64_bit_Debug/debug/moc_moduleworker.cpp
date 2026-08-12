/****************************************************************************
** Meta object code from reading C++ file 'moduleworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/moduleworker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'moduleworker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12ModuleWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto ModuleWorker::qt_create_metaobjectdata<qt_meta_tag_ZN12ModuleWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ModuleWorker",
        "connectedChanged",
        "",
        "connected",
        "modeChanged",
        "mode",
        "powerStateChanged",
        "state",
        "ledStateChanged",
        "onRequestConnect",
        "onRequestDisconnect",
        "onRequestSetMode",
        "onRequestSetPower",
        "onFrameReceived",
        "canId",
        "data",
        "onCcpTimeout"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectedChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'modeChanged'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'powerStateChanged'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'ledStateChanged'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onRequestConnect'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRequestDisconnect'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRequestSetMode'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onRequestSetPower'
        QtMocHelpers::SlotData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onFrameReceived'
        QtMocHelpers::SlotData<void(quint32, QByteArray)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 14 }, { QMetaType::QByteArray, 15 },
        }}),
        // Slot 'onCcpTimeout'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ModuleWorker, qt_meta_tag_ZN12ModuleWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ModuleWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModuleWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModuleWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12ModuleWorkerE_t>.metaTypes,
    nullptr
} };

void ModuleWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ModuleWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectedChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->modeChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->powerStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->ledStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onRequestConnect(); break;
        case 5: _t->onRequestDisconnect(); break;
        case 6: _t->onRequestSetMode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onRequestSetPower((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onFrameReceived((*reinterpret_cast<std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 9: _t->onCcpTimeout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ModuleWorker::*)(bool )>(_a, &ModuleWorker::connectedChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleWorker::*)(const QString & )>(_a, &ModuleWorker::modeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleWorker::*)(const QString & )>(_a, &ModuleWorker::powerStateChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleWorker::*)(const QString & )>(_a, &ModuleWorker::ledStateChanged, 3))
            return;
    }
}

const QMetaObject *ModuleWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModuleWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModuleWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModuleWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
    return _id;
}

// SIGNAL 0
void ModuleWorker::connectedChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ModuleWorker::modeChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ModuleWorker::powerStateChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ModuleWorker::ledStateChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
