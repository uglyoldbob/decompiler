#include "filecontainer.h"

#include <QQmlEngine>

#include "containerbincue.h"

std::vector<std::function<std::shared_ptr<FileContainer>(std::shared_ptr<QIODevice>)>> FileContainer::examiners;

FileContainer::FileContainer(QObject *parent) : QObject(parent)
{

}

void FileContainer::qml_register()
{
    qmlRegisterType<FileContainer>("uglyoldbob", 1, 0, "FileContainer");
}

void FileContainer::setup_examiners(void)
{
    examiners.push_back(ContainerBinCue::examine_object);
}

QStringList FileContainer::get_filenames()
{
    return QStringList(filenames);
}
