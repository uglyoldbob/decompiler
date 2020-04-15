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
        tmp->device = str;
        ret->set_type("PE Executable");

        str->seek(0);
        str->read((char*)(&tmp->dos_header), sizeof(exe_pe_dos_header));
        exe_pe_dos_header dh = tmp->dos_header;
        tmp->size = (tmp->dos_header.blocks_in_file - 1) * 512;

        str->seek(tmp->dos_header.new_header);
        str->read((char*)(&tmp->pe_header), sizeof(exe_pe_header));
        exe_pe_header dh2 = tmp->pe_header;
        str->seek(0);
    }
    return ret;
}
