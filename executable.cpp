#include "executable.h"
#include "exe_elf.h"

executable::executable()
{
	rbo = 0;
	exe_type = EXEC_TYPE_UNKNOWN;
	exe_object = 0;
}

executable::~executable()
{
	if (exe_object != 0)
		delete exe_object;
}

void executable::reverse(unsigned int *in)
{
	if (rbo)
	{
		unsigned int temp;
		temp = ( ((*in&0xFF000000)>>24) |
				 ((*in&0x00FF0000)>>16) | 
				 ((*in&0x0000FF00)<<8) |
				 ((*in&0x000000FF)<<24) );
		*in = temp;
	}
}

void executable::reverse(unsigned short *in)
{
	if (rbo)
	{
		unsigned short temp;
		temp = (*in>>8) | ((*in&0xFF)<<8);
		*in = temp;
	}
}

int executable::check_elf(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0x464C457F)
		{
			printf("ELF executable detected\n");
			exe_type = EXEC_TYPE_ELF;
			return 1;
		}
		else if (signature == 0x7F454C46)
		{
			printf("BACKWARDS? ELF executable detected\n");
			exe_type = EXEC_TYPE_ELF;
			rbo = 1;
			return 1;
		}
	}

	return 0;
}

int executable::check_macho64(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0xFEEDFACF)
		{
			printf("64-bit MACH-O executable detected\n");
			exe_type = EXEC_TYPE_MACHO64;
			return 1;
		}
		else if (signature == 0xCFFAEDFE)
		{
			printf("BACKWARDS 64-bit MACH-O executable detected\n");
			exe_type = EXEC_TYPE_MACHO64;
			rbo = 1;
			return 1;
		}
	}

	return 0;
}

int executable::check_macho32(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0xFEEDFACE)
		{
			printf("32-bit MACH-O executable detected\n");
			exe_type = EXEC_TYPE_MACHO32;
			return 1;
		}
		else if (signature == 0xCEFAEDFE)
		{
			printf("BACKWARDS 32-bit MACH-O executable detected\n");
			exe_type = EXEC_TYPE_MACHO32;
			rbo = 1;
			return 1;
		}
	}

	return 0;
}

int executable::check_pe(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0x3C, SEEK_SET);
	if (fread(&signature, 4, 1, me) != 1)
	{
		printf("error reading offset location\n");
		return -1;
	}
	fseek(me, signature, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0x4550)
		{
			printf("PE executable detected\n");
			exe_type = EXEC_TYPE_PE;
			return 1;
		}
		else if (signature == 0x00004550)
		{
			printf("BACKWARDS? PE executable detected\n");
			exe_type = EXEC_TYPE_PE;
			rbo = 1;
			return 1;
		}
	}

	return 0;
}

int executable::load(char *bin_name)
{
	int processed = 0;
	m = fopen(bin_name, "rb");
	if (m != NULL)
	{
		printf("Loaded file %s\n", bin_name);
	}
	else
	{
		printf("Failed to open executable %s\n", bin_name);
		return -1;
	}
	
	int reverse;
	if ((reverse = exe_elf::check(m)) != 0)
	{
		exe_type = EXEC_TYPE_ELF;
		exe_object = new exe_elf();
		if (reverse < 0)
			rbo = 1;
	}

	check_pe(m);
	check_macho32(m);
	check_macho64(m);

	switch (exe_type)
	{
		case EXEC_TYPE_UNKNOWN:
			printf("Unknown executable format\n");
			return -1;
			break;
		case EXEC_TYPE_PE:
			printf("STUB process PE executable\n");
			break;
		case EXEC_TYPE_MACHO32:
			printf("STUB process MACHO32 executable\n");
			break;
		case EXEC_TYPE_MACHO64:
			printf("STUB process MACHO64 executable\n");
			break;
		default:
			break;
	}

	if (exe_object == 0)
	{
		return -1;
	}
	if (exe_object->process(m) == 0)
	{
		printf("Successfully processed the executable\n");
	}
	else
	{
		printf("There was a problem processing the executable\n");
		return -1;
	}

	function *themain;
	themain = new function;
	funcs.push_back(themain);
	funcs[0]->set_name(exe_object->entry_name());

	printf("Created entry point function %s at address %x\n", funcs[0]->get_name(), exe_object->entry_addr());

	std::vector<uint32_t> blocks;	//indicates the starting point of blocks
	blocks.push_back(exe_object->entry_addr());
	while (blocks.size() > 0)
	{
		printf("Working on block starting at %x\n", blocks[0]);
		//does this block already exist?
		//gather an instruction
		//add it to the block
		//if the next instruction does not belong to this block, move to another block
		blocks.erase(blocks.begin());
	}
	

	printf("STUB Add lines of code to entry point function\n");
	

	fclose(m);
	return processed;
}
