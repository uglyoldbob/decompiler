#ifndef __EXE_LOADER_H__
#define __EXE_LOADER_H__

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "config.h"

#include "code_elements/code_element.h"

class disassembler;
class exe_loader;
typedef exe_loader* (*exe_checker)(std::shared_ptr<std::ifstream> me);

/// Class that describes how data is gathered from a generic executable.
/** Class that describes how data is gathered from a generic executable. Inherit this class and implement the virtual methods in order to add support for a new type of executable format. */
class exe_loader
{
	public:
		exe_loader(int reverse); ///< Create an exe_loader, specify if data should be byte reversed or not (little-endian vs big-endian data).
		virtual ~exe_loader();
		virtual int process(std::shared_ptr<std::ifstream> me) = 0; ///< Perform basic processing of the executable file. Used to do things such as interpret executable header information.
		virtual const char *entry_name() = 0; ///< The name of the function for the entry point of the program.
		virtual address entry_addr() = 0; ///< The starting point of execution for the program.
		virtual int goto_address(address addr) = 0; ///< Used to seek the input stream to the byte that corresponds to the given address in memory.
		virtual void read_memory(void *dest, int len) = 0; ///< Used to read a given number of bytes from the executable input stream.
		disassembler *get_disasm(); ///< Retrieve the disassembler use to interpret machine code in the executable.
		virtual std::vector<code_element *> gather_instructions(address start_address) = 0; //< This function starts decompiling at the specified address, dividing up code into code_elements based on conditional branching in the code.
		static void register_checker(exe_checker a); ///< Push a function to the list of exe_checkers. This list is used to determine which exe_loader knows how to interpret an executable.
		static exe_loader* check(std::shared_ptr<std::ifstream> me); ///< Call all registered exe_checker functions to determine which exe_loader to use for the specified input stream (of executable binary contents)
	protected:
		std::shared_ptr<std::istream> exe; ///< The input stream for reading the executable
		disassembler *disasm; ///< The disassembler used to interpret machine instructions in the executable.
		int rbo; ///< A variable that defines if the executable is little-endian or big-endian.
		static std::vector<exe_checker> *checker; ///< The list of functions to try to understand the format of an executable file.
};

#endif
