bin_PROGRAMS = decompile16 decompile32 decompile64

decompile16_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include
decompile32_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include
decompile64_CFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include

decompile16_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include
decompile32_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include
decompile64_CPPFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}/src -I${top_srcdir}/include

if WINDOWS
decompile16_LDFLAGS = -lmingw32
decompile32_LDFLAGS = -lmingw32
decompile64_LDFLAGS = -lmingw32
endif
decompile16_SOURCES = \
variable.cpp \
oper1.cpp \
oper2.cpp \
oper_preincrement.cpp \
oper_add.cpp \
oper_sub.cpp \
oper_assignment.cpp \
oper_bitwise_or.cpp \
ce_block.cpp \
code_element.cpp \
code_if_else.cpp \
decompile.cpp \
function.cpp \
code_multi_if.cpp \
code_run.cpp \
code_do_while_loop.cpp \
code_while_loop.cpp \
executable.cpp \
exe_loader.cpp \
exe_elf.cpp \
exe_macho.cpp \
exe_pe.cpp \
disassembler.cpp \
disass_x86.cpp \
disass_ppc.cpp \
exceptions.cpp \
helpers.cpp


decompile32_SOURCES = \
variable.cpp \
oper1.cpp \
oper2.cpp \
oper_preincrement.cpp \
oper_add.cpp \
oper_sub.cpp \
oper_assignment.cpp \
oper_bitwise_or.cpp \
ce_block.cpp \
code_element.cpp \
code_if_else.cpp \
decompile.cpp \
function.cpp \
code_multi_if.cpp \
code_run.cpp \
code_do_while_loop.cpp \
code_while_loop.cpp \
executable.cpp \
exe_loader.cpp \
exe_elf.cpp \
exe_macho.cpp \
exe_pe.cpp \
disassembler.cpp \
disass_x86.cpp \
disass_ppc.cpp \
exceptions.cpp \
helpers.cpp

decompile64_SOURCES = \
variable.cpp \
oper1.cpp \
oper2.cpp \
oper_preincrement.cpp \
oper_add.cpp \
oper_sub.cpp \
oper_assignment.cpp \
oper_bitwise_or.cpp \
ce_block.cpp \
code_element.cpp \
code_if_else.cpp \
decompile.cpp \
function.cpp \
code_multi_if.cpp \
code_run.cpp \
code_do_while_loop.cpp \
code_while_loop.cpp \
executable.cpp \
exe_loader.cpp \
exe_elf.cpp \
exe_macho.cpp \
exe_pe.cpp \
disassembler.cpp \
disass_x86.cpp \
disass_ppc.cpp \
exceptions.cpp \
helpers.cpp

if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3/

dist_man_MANS = $(directory)/man_page_1.3 $(directory)/man_page_2.3
$(directory)/man_page_1.3: doxyfile.stamp
$(directory)/man_page_2.3: doxyfile.stamp

doxyfile.stamp: decompile16 decompile32 decompile64
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/man
	rm -rf installer
endif

