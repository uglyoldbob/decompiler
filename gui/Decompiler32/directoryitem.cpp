#include "directoryitem.h"

#include <QQmlEngine>

DirectoryItem::DirectoryItem(QObject *parent) : QObject(parent)
{

}

void DirectoryItem::qml_register()
{
    qmlRegisterType<DirectoryItem>("uglyoldbob", 1, 0, "DirectoryItem");
}
