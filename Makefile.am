
AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = udis86

lib_LIBRARIES = libgtest.a

libgtest_a_SOURCES = googletest/googletest/src/gtest-all.cc
libgtest_a_CPPFLAGS = -I${top_srcdir}/googletest/googletest -I${top_srcdir}/googletest/googletest/include

test_SOURCES = src/example.c

bin_PROGRAMS = decompile16 decompile32 decompile64 test cfgtest projecttest

decompile16_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
decompile32_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
decompile64_CFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src

decompile16_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
decompile32_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
decompile64_CPPFLAGS = -D TARGET64 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src

cfgtest_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
cfgtest_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
cfgtest_SOURCES = \
src/code_elements/code_element.cpp \
src/code_elements/ce_basic.cpp \
src/code_elements/ce_dummy.cpp \
src/code_elements/code_if_else.cpp \
src/code_elements/code_multi_if.cpp \
src/code_elements/code_run.cpp \
src/code_elements/code_do_while_loop.cpp \
src/code_elements/code_while_loop.cpp \
src/code_elements/related_code.cpp \
src/helpers.cpp \
src/variable.cpp \
src/var/type.cpp \
src/cfgtest.cpp \
src/disassembly/disassembler.cpp \
src/var/combo.cpp \
src/var/related_code_maker.cpp

projecttest_CFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
projecttest_CPPFLAGS = -D TARGET32 -std=c++0x -I${top_srcdir}/udis86/ -I${top_srcdir} -Wall -I${top_srcdir}/src
projecttest_LDADD = libgtest.a
projecttest_SOURCES = \
src/tests/projects.cpp \
src/executable/executable.cpp \
src/executable/exe_loader.cpp \
src/project/project.cpp \
src/project/build_system.cpp \
src/project/source_file.cpp \
src/function.cpp \
src/code_elements/related_code.cpp \
src/code_elements/code_element.cpp \
src/var/combo.cpp \
src/var/type.cpp \
src/variable.cpp

TESTS = cfgtest projecttest

#BUILT_SOURCES = libudis86.a

decompile16_LDADD = -ludis86
decompile32_LDADD = -ludis86
decompile64_LDADD = -ludis86

decompile16_LDFLAGS = -L./udis86/libudis86/.libs
decompile32_LDFLAGS = -L./udis86/libudis86/.libs
decompile64_LDFLAGS = -L./udis86/libudis86/.libs

if WINDOWS
decompile16_LDFLAGS += -lmingw32
decompile32_LDFLAGS += -lmingw32
decompile64_LDFLAGS += -lmingw32
endif
decompile16_SOURCES = \
src/var/type.cpp \
src/variable.cpp \
src/operators/oper1.cpp \
src/operators/oper2.cpp \
src/operators/oper_dereference.cpp \
src/operators/oper_preincrement.cpp \
src/operators/oper_add.cpp \
src/operators/oper_left_shift.cpp \
src/operators/oper_right_shift.cpp \
src/operators/oper_sub.cpp \
src/operators/oper_assignment.cpp \
src/operators/oper_bitwise_and.cpp \
src/operators/oper_bitwise_or.cpp \
src/operators/oper_segbase.cpp \
src/decompile.cpp \
src/function.cpp \
src/code_elements/ce_basic.cpp \
src/code_elements/code_element.cpp \
src/code_elements/code_if_else.cpp \
src/code_elements/code_multi_if.cpp \
src/code_elements/code_run.cpp \
src/code_elements/code_do_while_loop.cpp \
src/code_elements/code_while_loop.cpp \
src/code_elements/related_code.cpp \
src/executable/executable.cpp \
src/executable/exe_loader.cpp \
src/executable/exe_elf.cpp \
src/executable/exe_macho.cpp \
src/executable/exe_pe.cpp \
src/executable/exe_real.cpp \
src/disassembly/disassembler.cpp \
src/disassembly/disass_x86.cpp \
src/disassembly/disass_ppc.cpp \
src/project/autotools.cpp \
src/project/build_system.cpp \
src/project/project.cpp \
src/project/source_file.cpp \
src/var/combo.cpp \
src/exceptions.cpp \
src/helpers.cpp


decompile32_SOURCES = \
src/var/type.cpp \
src/variable.cpp \
src/operators/oper1.cpp \
src/operators/oper2.cpp \
src/operators/oper_dereference.cpp \
src/operators/oper_preincrement.cpp \
src/operators/oper_add.cpp \
src/operators/oper_left_shift.cpp \
src/operators/oper_right_shift.cpp \
src/operators/oper_sub.cpp \
src/operators/oper_assignment.cpp \
src/operators/oper_bitwise_and.cpp \
src/operators/oper_bitwise_or.cpp \
src/operators/oper_segbase.cpp \
src/decompile.cpp \
src/function.cpp \
src/code_elements/ce_basic.cpp \
src/code_elements/code_element.cpp \
src/code_elements/code_if_else.cpp \
src/code_elements/code_multi_if.cpp \
src/code_elements/code_run.cpp \
src/code_elements/code_do_while_loop.cpp \
src/code_elements/code_while_loop.cpp \
src/code_elements/related_code.cpp \
src/executable/executable.cpp \
src/executable/exe_loader.cpp \
src/executable/exe_elf.cpp \
src/executable/exe_macho.cpp \
src/executable/exe_pe.cpp \
src/executable/exe_real.cpp \
src/disassembly/disassembler.cpp \
src/disassembly/disass_x86.cpp \
src/disassembly/disass_ppc.cpp \
src/project/autotools.cpp \
src/project/build_system.cpp \
src/project/project.cpp \
src/project/source_file.cpp \
src/var/combo.cpp \
src/exceptions.cpp \
src/helpers.cpp

decompile64_SOURCES = \
src/var/type.cpp \
src/variable.cpp \
src/operators/oper1.cpp \
src/operators/oper2.cpp \
src/operators/oper_dereference.cpp \
src/operators/oper_preincrement.cpp \
src/operators/oper_add.cpp \
src/operators/oper_left_shift.cpp \
src/operators/oper_right_shift.cpp \
src/operators/oper_sub.cpp \
src/operators/oper_assignment.cpp \
src/operators/oper_bitwise_and.cpp \
src/operators/oper_bitwise_or.cpp \
src/operators/oper_segbase.cpp \
src/decompile.cpp \
src/function.cpp \
src/code_elements/ce_basic.cpp \
src/code_elements/code_element.cpp \
src/code_elements/code_if_else.cpp \
src/code_elements/code_multi_if.cpp \
src/code_elements/code_run.cpp \
src/code_elements/code_do_while_loop.cpp \
src/code_elements/code_while_loop.cpp \
src/code_elements/related_code.cpp \
src/executable/executable.cpp \
src/executable/exe_loader.cpp \
src/executable/exe_elf.cpp \
src/executable/exe_macho.cpp \
src/executable/exe_pe.cpp \
src/executable/exe_real.cpp \
src/disassembly/disassembler.cpp \
src/disassembly/disass_x86.cpp \
src/disassembly/disass_ppc.cpp \
src/project/autotools.cpp \
src/project/build_system.cpp \
src/project/project.cpp \
src/project/source_file.cpp \
src/var/combo.cpp \
src/exceptions.cpp \
src/helpers.cpp

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

