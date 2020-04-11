#ifndef DUMMYOBJECTMAPPER_H
#define DUMMYOBJECTMAPPER_H

#include "objectmapper.h"
#include <QObject>

/// A object mapper for demo purposes.
class DummyObjectMapper : public ObjectMapper
{
    Q_OBJECT
public:
    DummyObjectMapper();
    static std::shared_ptr<ObjectMapper> examine_object_succeed(std::shared_ptr<QDataStream> str);
    static std::shared_ptr<ObjectMapper> examine_object_fail(std::shared_ptr<QDataStream> str);
};

#endif // DUMMYOBJECTMAPPER_H
