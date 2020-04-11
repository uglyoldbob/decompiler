#ifndef OBJECTMAPPERPE_H
#define OBJECTMAPPERPE_H

#include "objectmapper.h"

#define EXE_PE_ID 0x5A4D

struct exe_pe_header
{
    unsigned short signature; /* == 0x5a4D */
    unsigned short bytes_in_last_block;
    unsigned short blocks_in_file;
    unsigned short num_relocs;
    unsigned short header_paragraphs;
    unsigned short min_extra_paragraphs;
    unsigned short max_extra_paragraphs;
    unsigned short ss;
    unsigned short sp;
    unsigned short checksum;
    unsigned short ip;
    unsigned short cs;
    unsigned short reloc_table_offset;
    unsigned short overlay_number;
};

struct exe_pe_reloc
{
    unsigned short offset;
    unsigned short segment;
};

class ObjectMapperPe : public ObjectMapper
{
    Q_OBJECT
public:
    explicit ObjectMapperPe(QObject *parent = nullptr);
    static std::shared_ptr<ObjectMapper> examine_object(std::shared_ptr<QIODevice> str);
signals:

};

#endif // OBJECTMAPPERPE_H
