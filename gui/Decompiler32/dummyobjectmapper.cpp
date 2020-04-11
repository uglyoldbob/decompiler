#include "dummyobjectmapper.h"

DummyObjectMapper::DummyObjectMapper()
{

}

std::shared_ptr<ObjectMapper> DummyObjectMapper::examine_object_fail(std::shared_ptr<QIODevice> str)
{
    return std::shared_ptr<ObjectMapper>(nullptr);
}


std::shared_ptr<ObjectMapper> DummyObjectMapper::examine_object_succeed(std::shared_ptr<QIODevice> str)
{
    std::shared_ptr<ObjectMapper> ret = std::shared_ptr<ObjectMapper>(static_cast<ObjectMapper*>(new DummyObjectMapper()));
    ret->set_type("Dummy");
    return ret;
}
