#include "objectmapper.h"

#include "dummyobjectmapper.h"
#include "objectmapperpe.h"

#include <QQmlEngine>

std::vector<std::function<std::shared_ptr<ObjectMapper>(std::shared_ptr<QIODevice>)>> ObjectMapper::examiners;

ObjectMapper::ObjectMapper(QObject *parent) : QObject(parent)
{
    type = "Invalid";
    emit type_changed();
}

void ObjectMapper::setup_examiners(void)
{
    examiners.push_back(ObjectMapperPe::examine_object);
    examiners.push_back(DummyObjectMapper::examine_object_fail);
    examiners.push_back(DummyObjectMapper::examine_object_succeed);
}

void ObjectMapper::qml_register()
{
    qmlRegisterType<ObjectMapper>("uglyoldbob", 1, 0, "ObjectMapper");
}

std::shared_ptr<ObjectMapper> ObjectMapper::examine_object(std::shared_ptr<QIODevice> str)
{
    std::shared_ptr<ObjectMapper> ret = std::shared_ptr<ObjectMapper>(nullptr);
    for (unsigned int i = 0; i < ObjectMapper::examiners.size(); i++)
    {
        ret = examiners[i](str);
        if (ret.get() != nullptr)
        {
            break;
        }
    }
    return ret;
}
