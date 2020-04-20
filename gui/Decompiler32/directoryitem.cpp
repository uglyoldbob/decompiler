#include "directoryitem.h"

#include <QQmlEngine>

DirectoryItem::DirectoryItem(QObject *parent) : QObject(parent)
{
    is_dir = false;
}

void DirectoryItem::qml_register()
{
    qmlRegisterType<DirectoryItem>("uglyoldbob", 1, 0, "DirectoryItem");
}

void DirectoryItem::set_parent_directory(QList<DirectoryItem*>* p)
{
    parent = p;
}

void DirectoryItem::set_directory()
{
    is_dir = true;
}

QList<QObject *> DirectoryItem::get_dir_contents()
{
    QObjectList l;
    for(auto e: dir_elements)
        l << e;
    return l;
}

void DirectoryItem::add_dir_item(DirectoryItem* i)
{
    if (is_dir)
    {
        dir_elements.push_back(i);
    }
}
