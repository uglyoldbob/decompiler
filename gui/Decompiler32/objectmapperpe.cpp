#include "objectmapperpe.h"

ObjectMapperPe::ObjectMapperPe(QObject *parent) : ObjectMapper(parent)
{

}

std::shared_ptr<ObjectMapper> ObjectMapperPe::examine_object(std::shared_ptr<QIODevice> str)
{
    str->seek(0);
    unsigned short signature;
    str->read((char*)&signature, 2);
    std::shared_ptr<ObjectMapper> ret;
    if (signature == EXE_PE_ID)
    {
        ObjectMapperPe *tmp = new ObjectMapperPe();
        ret.reset(tmp);
        ret->set_type("PE Executable");
    }
    return ret;
}
