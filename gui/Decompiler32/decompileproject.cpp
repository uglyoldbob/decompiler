#include "decompileproject.h"

#include <QDir>
#include <QFile>
#include <QQmlEngine>

DecompileProject::DecompileProject(QObject *parent) : QObject(parent)
{
    project_directory = "./";
    things.append(new QString("I am groot"));
    things.append(new QString("potato"));
    emit things_changed();

    bob.append("blab away");
    bob.append("blobbert");
    emit l_changed();
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
    DecompiledObject *obj = new DecompiledObject(rel_name, this);
    objects.append(obj);
}

void DecompileProject::add_object(QUrl filename)
{
    QString output_name = filename.fileName();
    add_object(filename.toLocalFile(), "", output_name);
}

QStringList DecompileProject::get_l()
{
    return QStringList(bob);
}

int DecompileProject::ThingsCount() const
{
    return things.count();
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

QString *DecompileProject::ThingsIndex(int i) const
{
    return things.at(i);
}

int DecompileProject::s_ThingsCount(QQmlListProperty<QString>* list)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ThingsCount();
}

QString *DecompileProject::s_ThingsIndex(QQmlListProperty<QString>* list, int i)
{
    return reinterpret_cast<DecompileProject*>(list->data)->ThingsIndex(i);
}

QQmlListProperty<QString> DecompileProject::get_things()
{
    return { this, this, &DecompileProject::s_ThingsCount, &DecompileProject::s_ThingsIndex };
}
