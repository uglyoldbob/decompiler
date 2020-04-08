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
