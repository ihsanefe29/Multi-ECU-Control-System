/****************************************************************************
** Meta object code from reading C++ file 'modulesession.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/modulesession.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modulesession.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13ModuleSessionE_t {};
} // unnamed namespace

template <> constexpr inline auto ModuleSession::qt_create_metaobjectdata<qt_meta_tag_ZN13ModuleSessionE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ModuleSession",
        "connectedChanged",
        "",
        "modeChanged",
        "powerStateChanged",
        "ledStateChanged",
        "requestConnect",
        "requestDisconnect",
        "requestSetMode",
        "mode",
        "requestSetPower",
        "state",
        "applyConnected",
        "connected",
        "applyMode",
        "applyPowerState",
        "applyLedState",
        "ecuId",
        "moduleNumber",
        "powerState",
        "ledState"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectedChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'powerStateChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'ledStateChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestConnect'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestDisconnect'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestSetMode'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Signal 'requestSetPower'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'applyConnected'
        QtMocHelpers::SlotData<void(bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'applyMode'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'applyPowerState'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'applyLedState'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'ecuId'
        QtMocHelpers::PropertyData<QString>(17, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'moduleNumber'
        QtMocHelpers::PropertyData<int>(18, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'connected'
        QtMocHelpers::PropertyData<bool>(13, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'mode'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'powerState'
        QtMocHelpers::PropertyData<QString>(19, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'ledState'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ModuleSession, qt_meta_tag_ZN13ModuleSessionE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ModuleSession::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ModuleSessionE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ModuleSessionE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13ModuleSessionE_t>.metaTypes,
    nullptr
} };

void ModuleSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ModuleSession *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectedChanged(); break;
        case 1: _t->modeChanged(); break;
        case 2: _t->powerStateChanged(); break;
        case 3: _t->ledStateChanged(); break;
        case 4: _t->requestConnect(); break;
        case 5: _t->requestDisconnect(); break;
        case 6: _t->requestSetMode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->requestSetPower((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->applyConnected((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->applyMode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->applyPowerState((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->applyLedState((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::connectedChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::modeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::powerStateChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::ledStateChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::requestConnect, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)()>(_a, &ModuleSession::requestDisconnect, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)(const QString & )>(_a, &ModuleSession::requestSetMode, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModuleSession::*)(const QString & )>(_a, &ModuleSession::requestSetPower, 7))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->ecuId(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->moduleNumber(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->connected(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->mode(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->powerState(); break;
        case 5: *reinterpret_cast<QString*>(_v) = _t->ledState(); break;
        default: break;
        }
    }
}

const QMetaObject *ModuleSession::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModuleSession::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ModuleSessionE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModuleSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ModuleSession::connectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModuleSession::modeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModuleSession::powerStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ModuleSession::ledStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ModuleSession::requestConnect()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ModuleSession::requestDisconnect()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ModuleSession::requestSetMode(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ModuleSession::requestSetPower(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP
