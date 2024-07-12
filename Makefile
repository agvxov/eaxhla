.PHONY: clean test
.SUFFIXES:

ifeq (${DEBUG}, 1)
  LFLAGS     += --debug --trace
  CFLAGS     += -O0 -ggdb -fno-inline
  CPPFLAGS   += -DDEBUG
  FLEXFLAGS  += --trace --debug
  BISONFLAGS += --debug -Wcounterexamples
else
  CFLAGS += -O3 -flto=auto -fno-stack-protector
endif

ifeq ($(SAN), 1)
  CFLAGS += -fsanitize=address,undefined
else
  WRAP := valgrind --show-error-list=yes --track-origins=yes --leak-check=full --show-leak-kinds=all
endif

CFLAGS   += -Wall -Wextra -Wpedantic
CPPFLAGS += -Ilibrary/tommyds
LDFLAGS  += -lm

OUT := eaxhla

SOURCE.d  := source
OBJECT.d  := object

SOURCE    := main.c assembler.c eaxhla.c
OBJECT    := $(addprefix ${OBJECT.d}/,${SOURCE})
OBJECT    := ${OBJECT:.c=.o}

# see library/, run library/bootstrap.sh
LIBS      := tommy.o
LIBS      := $(addprefix ${OBJECT.d}/,${LIBS.source})

GENSOURCE := eaxhla.yy.c eaxhla.tab.c
GENSOURCE := $(addprefix ${OBJECT.d}/,${GENSOURCE})
GENOBJECT := $(subst .c,.o,${GENSOURCE})

CPPFLAGS  += -I${OBJECT.d} -I${SOURCE.d}

LDLIBS := -lm

all: ${OUT}

${OBJECT.d}/%.yy.c: ${SOURCE.d}/%.l
	flex ${FLEXFLAGS} --header-file=object/$(basename $(notdir $<)).yy.h -o $@ $<

${OBJECT.d}/%.tab.c: ${SOURCE.d}/%.y
#~	bison ${BISONFLAGS} --header=object/$(basename $(notdir $<)).tab.h -o $@ $<
	bison ${BISONFLAGS} --defines=object/$(basename $(notdir $<)).tab.h -o $@ $<

${OBJECT.d}/%.yy.o: ${OBJECT.d}/%.yy.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.tab.o: ${OBJECT.d}/%.tab.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.o: ${SOURCE.d}/%.c
	${COMPILE.c} -o $@ $<

${OUT}: ${GENSOURCE} ${GENOBJECT} ${OBJECT} ${LIB}
	${LINK.c} -o $@ ${OBJECT} ${GENOBJECT} ${LDLIBS} library/tommyds/tommy.o

bootstrap:
	./library/bootstrap.sh

test: ${OUT}
	#fcpp -C -LL debug/xop.eax > debug/xop.eax.pp
	#${WRAP} ./${OUT} debug/xop.eax
	#./${OUT} debug/xop.eax # NOTE as of now broken, but only because of undefined instructions
	#${WRAP} ./${OUT} debug/artimetrics.eax
	#@echo -e "\033[31;1m --- ERROR TESTING BEGINS BELOW ---\033[0m"
	#debug/error_test.sh
	#./${OUT} debug/heyo_world.eax
	ORIGIN="$$(realpath .)" PATH="$$(realpath .):${PATH}" cmdtest

clean:
	-rm ${OUT} ${OBJECT} ${GENOBJECT} ${GENSOURCE}

${OBJECT.d}/%.pp: debug/%.tcl debug/instructions.tcl
	tclsh $< > $@

plug: ${OBJECT.d}/token_list.pp ${OBJECT.d}/scanner_instructions.pp ${OBJECT.d}/parser_rules.pp
	plug -u -d token_list '' -d scanner_instructions '' -d parser_rules '' source/eaxhla.l source/eaxhla.y
	plug -g -e token_list           ${OBJECT.d}/token_list.pp           source/eaxhla.y
	plug -g -e scanner_instructions ${OBJECT.d}/scanner_instructions.pp source/eaxhla.l
	plug -g -e parser_rules         ${OBJECT.d}/parser_rules.pp         source/eaxhla.y
	

.PHONY: test clean bootstrap
