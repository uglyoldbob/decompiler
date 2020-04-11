#include "decompiledobject.h"

#include <QQmlEngine>

DecompiledObject::DecompiledObject(QString file, QObject *parent) : QObject(parent)
{
    name = file;
    type = "Invalid";
    emit name_changed();
    emit type_changed();
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

void DecompiledObject::give_device(std::shared_ptr<QIODevice> ds)
{
    device = ds;
    mapper = ObjectMapper::examine_object(ds);
    connect(mapper.get(), &ObjectMapper::type_changed, this, &DecompiledObject::type_changed);
}

QString DecompiledObject::get_type()
{
    if (mapper.get() != nullptr)
    {
        return mapper->get_type();
    }
    return type;
}
