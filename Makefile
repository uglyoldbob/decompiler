
all: decompile32 decompile64

TEST_DIR="./test"

OPERATOR_OBJS = variable.o oper1.o oper2.o oper_preincrement.o oper_add.o \
	oper_sub.o oper_assignment.o oper_bitwise_or.o

DECOMPILE_OBJS = ce_block.o code_element.o code_if_else.o decompile.o function.o \
	code_multi_if.o code_run.o code_do_while_loop.o code_while_loop.o executable.o \
	exe_loader.o exe_elf.o exe_macho.o disassembler.o disass_x86.o disass_ppc.o \
	exceptions.o helpers.o $(OPERATOR_OBJS)
	

DECOMPILE32_OBJS = $(DECOMPILE_OBJS:%.o=%32.o)
DECOMPILE64_OBJS = $(DECOMPILE_OBJS:%.o=%64.o)

DECOMPILE32_DEPS := $(DECOMPILE32_OBJS:.o=.d)
DECOMPILE64_DEPS := $(DECOMPILE64_OBJS:.o=.d)

-include $(DECOMPILE32_DEPS)
-include $(DECOMPILE64_DEPS)

CC=g++

EXTRA_FLAGS=-g -std=c++0x
32_CFLAGS =-c -Wall -D TARGET32
64_CFLAGS =-c -Wall -D TARGET64

decompile32: $(DECOMPILE32_OBJS)
	$(CC) $(EXTRA_FLAGS) $(DECOMPILE32_OBJS) $(LFLAGS) $(LDADD) -o decompile32

decompile64: $(DECOMPILE64_OBJS)
	$(CC) $(EXTRA_FLAGS) $(DECOMPILE64_OBJS) $(LFLAGS) $(LDADD) -o decompile64

clean:
	rm -f *.o *.d decompile32 decompile64

$(TEST_DIR)/decompile32: decompile32
	cp decompile32 $(TEST_DIR)/decompile32

$(TEST_DIR)/decompile64: decompile64
	cp decompile64 $(TEST_DIR)/decompile64

testobjs: $(TEST_DIR)/decompile32

test: decompile32 decompile64 testobjs
	echo "Running test routine"
	-for file in $(TEST_DIR)/*; do echo "\n$$file\n\n" && ./decompile32 $$file ; done
	-for file in $(TEST_DIR)/*; do echo "\n$$file\n\n" && ./decompile64 $$file ; done

%32.o: %.cpp
	@if [ ! -d $(@D) ]; then\
		echo mkdir $(@D);\
		mkdir $(@D);\
	fi
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(32_CFLAGS) $(INCLUDE)$(@D) $< -o $@
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(32_CFLAGS) $(INCLUDE)$(@D) $< -MM -MF $(@D)/$(*F)32.d
	sed -i -e's/$(*F)\.o/$(*F)32\.o/g' $(@D)/$(*F)32.d

%64.o: %.cpp
	@if [ ! -d $(@D) ]; then\
		echo mkdir $(@D);\
		mkdir $(@D);\
	fi
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(64_CFLAGS) $(INCLUDE)$(@D) $< -o $@
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(64_CFLAGS) $(INCLUDE)$(@D) $< -MM -MF $(@D)/$(*F)64.d
	sed -i -e's/$(*F)\.o/$(*F)64\.o/g' $(@D)/$(*F)64.d
