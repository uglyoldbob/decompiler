AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = decompile16 decompile32 decompile64

decompile16_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}
decompile32_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}
decompile64_CFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}

decompile16_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}
decompile32_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}
decompile64_CPPFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86-1.7.2/ -I${top_srcdir}

if WINDOWS
decompile16_LDFLAGS = -lmingw32
decompile32_LDFLAGS = -lmingw32
decompile64_LDFLAGS = -lmingw32
endif
decompile16_SOURCES = \
variable.cpp \
operators/oper1.cpp \
operators/oper2.cpp \
operators/oper_dereference.cpp \
operators/oper_preincrement.cpp \
operators/oper_add.cpp \
operators/oper_left_shift.cpp \
operators/oper_sub.cpp \
operators/oper_assignment.cpp \
operators/oper_bitwise_and.cpp \
operators/oper_bitwise_or.cpp \
decompile.cpp \
function.cpp \
code_elements/code_element.cpp \
code_elements/code_if_else.cpp \
code_elements/code_multi_if.cpp \
code_elements/code_run.cpp \
code_elements/code_do_while_loop.cpp \
code_elements/code_while_loop.cpp \
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
operators/oper1.cpp \
operators/oper2.cpp \
operators/oper_dereference.cpp \
operators/oper_preincrement.cpp \
operators/oper_add.cpp \
operators/oper_left_shift.cpp \
operators/oper_sub.cpp \
operators/oper_assignment.cpp \
operators/oper_bitwise_and.cpp \
operators/oper_bitwise_or.cpp \
decompile.cpp \
function.cpp \
code_elements/code_element.cpp \
code_elements/code_if_else.cpp \
code_elements/code_multi_if.cpp \
code_elements/code_run.cpp \
code_elements/code_do_while_loop.cpp \
code_elements/code_while_loop.cpp \
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
operators/oper1.cpp \
operators/oper2.cpp \
operators/oper_dereference.cpp \
operators/oper_preincrement.cpp \
operators/oper_add.cpp \
operators/oper_left_shift.cpp \
operators/oper_sub.cpp \
operators/oper_assignment.cpp \
operators/oper_bitwise_and.cpp \
operators/oper_bitwise_or.cpp \
decompile.cpp \
function.cpp \
code_elements/ce_block.cpp \
code_elements/code_element.cpp \
code_elements/code_if_else.cpp \
code_elements/code_multi_if.cpp \
code_elements/code_run.cpp \
code_elements/code_do_while_loop.cpp \
code_elements/code_while_loop.cpp \
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

