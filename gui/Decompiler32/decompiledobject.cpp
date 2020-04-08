#include "decompiledobject.h"

DecompiledObject::DecompiledObject(QString file, QObject *parent) : QObject(parent)
{
    name = file;
    emit name_changed();
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
