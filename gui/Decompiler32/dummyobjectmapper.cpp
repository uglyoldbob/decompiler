#include "dummyobjectmapper.h"

DummyObjectMapper::DummyObjectMapper()
{

}

std::shared_ptr<ObjectMapper> DummyObjectMapper::examine_object(std::shared_ptr<QDataStream> str)
{
    std::shared_ptr<ObjectMapper> ret = std::shared_ptr<ObjectMapper>(static_cast<ObjectMapper*>(new DummyObjectMapper()));
    ret->set_type("Dummy");
    return ret;
}
