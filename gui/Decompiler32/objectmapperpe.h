#ifndef OBJECTMAPPERPE_H
#define OBJECTMAPPERPE_H

#include "objectmapper.h"

#define EXE_PE_ID 0x5A4D

struct exe_pe_dos_header
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
    unsigned short reserved1[4];
    unsigned short oem_id;
    unsigned short oem_info;
    unsigned short reserved2[10];
    uint32_t new_header;
};

struct exe_pe_header
{
    uint32_t pe_signature;
    unsigned short cpu_id;
    unsigned short num_sections;
    uint32_t date_time;
    uint32_t symbol_table;
    uint32_t num_symbols;
    unsigned short size_optional_header;
    unsigned short characteristics;
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
    static std::shared_ptr<ObjectMapper> examine_object(std::shared_ptr<QIODevice> str); ///< Examine a given file and return an object if it is valid for this class.
signals:

private:
    exe_pe_dos_header dos_header; ///< The dos header
    exe_pe_header pe_header; ///< The pe header
    unsigned int size; ///< The size specified by the pe header
};

#endif // OBJECTMAPPERPE_H
