#include "mytype.h"

#include <QtQml>

MyType::MyType(QObject *parent) : QObject(parent)
{
}

int MyType::value() const
{
    return _value;
}

void MyType::setValue(int value)
{
    if (_value == value)
        return;
    _value = value;
    emit valueChanged();
}

// Auto register the type for the "NativePluginModule" qml module
// (defined in CMakeLists.txt).
QML_ELEMENT
