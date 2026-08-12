/****************************************************************************
** Meta object code from reading C++ file 'rawsignalloader.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/rawsignalloader.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rawsignalloader.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15RawSignalLoaderE_t {};
} // unnamed namespace

template <> constexpr inline auto RawSignalLoader::qt_create_metaobjectdata<qt_meta_tag_ZN15RawSignalLoaderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RawSignalLoader",
        "dataChanged",
        "",
        "lastErrorChanged",
        "loadParameterFile",
        "QUrl",
        "fileUrl",
        "loadRawDataFile",
        "loaded",
        "lastError",
        "parameterCount",
        "rawByteCount",
        "parsedParameters",
        "QVariantList",
        "rawByteValues"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'dataChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastErrorChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'loadParameterFile'
        QtMocHelpers::MethodData<bool(const QUrl &)>(4, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Method 'loadRawDataFile'
        QtMocHelpers::MethodData<bool(const QUrl &)>(7, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 5, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'loaded'
        QtMocHelpers::PropertyData<bool>(8, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'lastError'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'parameterCount'
        QtMocHelpers::PropertyData<int>(10, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'rawByteCount'
        QtMocHelpers::PropertyData<int>(11, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'parsedParameters'
        QtMocHelpers::PropertyData<QVariantList>(12, 0x80000000 | 13, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'rawByteValues'
        QtMocHelpers::PropertyData<QVariantList>(14, 0x80000000 | 13, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RawSignalLoader, qt_meta_tag_ZN15RawSignalLoaderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RawSignalLoader::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RawSignalLoaderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RawSignalLoaderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15RawSignalLoaderE_t>.metaTypes,
    nullptr
} };

void RawSignalLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RawSignalLoader *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->dataChanged(); break;
        case 1: _t->lastErrorChanged(); break;
        case 2: { bool _r = _t->loadParameterFile((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { bool _r = _t->loadRawDataFile((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RawSignalLoader::*)()>(_a, &RawSignalLoader::dataChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RawSignalLoader::*)()>(_a, &RawSignalLoader::lastErrorChanged, 1))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->loaded(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->lastError(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->parameterCount(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->rawByteCount(); break;
        case 4: *reinterpret_cast<QVariantList*>(_v) = _t->parsedParameters(); break;
        case 5: *reinterpret_cast<QVariantList*>(_v) = _t->rawByteValues(); break;
        default: break;
        }
    }
}

const QMetaObject *RawSignalLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RawSignalLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RawSignalLoaderE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RawSignalLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
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
void RawSignalLoader::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RawSignalLoader::lastErrorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
