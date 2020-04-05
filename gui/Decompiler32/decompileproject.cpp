#include "decompileproject.h"

#include <QQmlEngine>

DecompileProject::DecompileProject(QObject *parent) : QObject(parent)
{
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
    qmlRegisterSingletonType<DecompileProject>("uglyoldbob", 1, 0, "SDecompileProject",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        DecompileProject *nd = new DecompileProject();
        return nd;
    });
}

QStringList DecompileProject::get_l()
{
    return QStringList(bob);
}

int DecompileProject::ThingsCount() const
{
    return things.count();
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
