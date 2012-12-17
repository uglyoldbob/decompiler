
all: decompile

DECOMPILE_OBJS = ce_block.o code_element.o code_if_else.o decompile.o function.o \
	code_multi_if.o code_run.o code_do_while_loop.o code_while_loop.o executable.o \
	exe_loader.o exe_elf.o disassembler.o disass_x86_32.o

DECOMPILE_DEPS := $(DECOMPILE_OBJS:.o=.d)

-include $(DECOMPILE_DEPS)


CC=g++

EXTRA_FLAGS=-g
CFLAGS =-c -Wall

decompile: $(DECOMPILE_OBJS)
	$(CC) $(EXTRA_FLAGS) $(DECOMPILE_OBJS) $(LFLAGS) $(LDADD) -o decompile

clean:
	rm -f *.o *.d decompile

test: decompile
	echo "Running test routine"
	./decompile

.cpp.o:
	@if [ ! -d $(@D) ]; then\
		echo mkdir $(@D);\
		mkdir $(@D);\
	fi
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(INCLUDE)$(@D) $< -o $@
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(INCLUDE)$(@D) $< -MM -MF $(@D)/$(*F).d
