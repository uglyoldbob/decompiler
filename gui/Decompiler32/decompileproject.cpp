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

    std::shared_ptr<QFile> sf = std::shared_ptr<QFile>(new QFile(output_name));
    files.push_back(sf);

    if (sf->open(QIODevice::ReadOnly))
    {   //success
        DecompiledObject *obj = new DecompiledObject(rel_name, this);
        obj->give_stream(std::shared_ptr<QDataStream>(new QDataStream(sf.get())));
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

