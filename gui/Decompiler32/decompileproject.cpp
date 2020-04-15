#include "decompileproject.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QQmlEngine>

DecompileProject::DecompileProject(QObject *parent) : QObject(parent)
{
    project_directory = "./default_project/";
    emit objects_changed();
}

void DecompileProject::qml_register()
{
    qRegisterMetaType<QQmlListProperty<QString>>("QQmlListProperty<QString>");
    qRegisterMetaType<QQmlListProperty<DecompiledObject>>("QQmlListProperty<DecompiledObject>");
    qmlRegisterSingletonType<DecompileProject>("uglyoldbob", 1, 0, "DecompileProject",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        DecompileProject *nd = new DecompileProject();
        return nd;
    });
}

void DecompileProject::add_object(QString filename, QString rpath, QString rel_name)
{
    QString output_name = project_directory + QString("object/") + rpath + rel_name;
    QDir dir(project_directory + QString("object/") + rpath);
    dir.mkpath("./");
    if (QFile::exists(output_name))
    {
        QFile::remove(output_name);
    }
    QFile::copy(filename, output_name);

    std::shared_ptr<QIODevice> sf = std::shared_ptr<QFile>(new QFile(output_name));
    devices.push_back(sf);

    if (sf->open(QIODevice::ReadOnly))
    {   //success
        DecompiledObject *obj = new DecompiledObject(rel_name, this);
        obj->give_device(sf);
        objects.append(obj);
        emit objects_changed();
    }
    else
    {   //fail

    }

}

void DecompileProject::add_object(QUrl filename)
{
    QString output_name = filename.fileName();
    add_object(filename.toLocalFile(), "", output_name);
}


void DecompileProject::add_container(QString filename, QString rpath, QString rel_name)
{
    QString output_name = project_directory + QString("container/") + rpath + rel_name;
    QDir dir(project_directory + QString("container/") + rpath);
    dir.mkpath("./");
    if (QFile::exists(output_name))
    {
        QFile::remove(output_name);
    }
    QFile::copy(filename, output_name);

    std::shared_ptr<QIODevice> sf = std::shared_ptr<QFile>(new QFile(output_name));
    devices.push_back(sf);

    if (sf->open(QIODevice::ReadOnly))
    {   //success
        DecompiledObject *obj = new DecompiledObject(rel_name, this);
        obj->give_device(sf);
        objects.append(obj);
        emit objects_changed();
    }
    else
    {   //fail

    }

}

void DecompileProject::add_container(QUrl filename)
{
    QString output_name = filename.fileName();
    add_object(filename.toLocalFile(), "", output_name);
}

int DecompileProject::ObjectsCount() const
{
    return objects.count();
}

DecompiledObject *DecompileProject::ObjectsIndex(int i) const
{
    return objects.at(i);
}

int DecompileProject::s_ObjectsCount(QQmlListProperty<DecompiledObject>* list)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ObjectsCount();
}

DecompiledObject *DecompileProject::s_ObjectsIndex(QQmlListProperty<DecompiledObject>* list, int i)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ObjectsIndex(i);
}

QQmlListProperty<DecompiledObject> DecompileProject::get_objects()
{
    return { this, this, &DecompileProject::s_ObjectsCount, &DecompileProject::s_ObjectsIndex };
}


int DecompileProject::ContainersCount() const
{
    return containers.size();
}

FileContainer *DecompileProject::ContainersIndex(int i) const
{
    return containers.at(i).get();
}

int DecompileProject::s_ContainersCount(QQmlListProperty<FileContainer>* list)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ContainersCount();
}

FileContainer *DecompileProject::s_ContainersIndex(QQmlListProperty<FileContainer>* list, int i)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ContainersIndex(i);
}

QQmlListProperty<FileContainer> DecompileProject::get_containers()
{
    return { this, this, &DecompileProject::s_ContainersCount, &DecompileProject::s_ContainersIndex };
}
