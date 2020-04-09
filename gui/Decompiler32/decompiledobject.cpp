#include "decompiledobject.h"

#include <QQmlEngine>

DecompiledObject::DecompiledObject(QString file, QObject *parent) : QObject(parent)
{
    name = file;
    emit name_changed();
}

void DecompiledObject::qml_register()
{
    qmlRegisterType<DecompiledObject>("uglyoldbob", 1, 0, "DecompiledObject");
}

DecompiledObject::DecompiledObject(QObject *parent) : QObject(parent)
{
    name = "blob";
    emit name_changed();
}

void DecompiledObject::give_stream(std::shared_ptr<QDataStream> ds)
{
    stream = ds;
}
