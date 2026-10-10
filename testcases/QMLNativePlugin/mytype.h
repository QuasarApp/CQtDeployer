#ifndef MYTYPE_H
#define MYTYPE_H

#include <QObject>

// A simple native (c++) type that is exported to the qml module
// "NativePluginModule". Because the type is a native type the module
// is built as a qml plugin shared library (libNativePluginModule.so).
class MyType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
public:
    explicit MyType(QObject *parent = nullptr);

    int value() const;
    void setValue(int value);

signals:
    void valueChanged();

private:
    int _value = 0;
};

#endif // MYTYPE_H
