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

PLUG := tool/plug

SOURCE.d  := source
OBJECT.d  := object

SOURCE    := main.c assembler.c eaxhla.c unix.c
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
	#${WRAP} ./${OUT} debug/xop.eax
	ORIGIN="$$(realpath .)" PATH="$$(realpath .):${PATH}" cmdtest

clean: unplug
	-rm ${OUT} ${OBJECT} ${GENOBJECT} ${GENSOURCE}

${OBJECT.d}/%.pp: debug/%.tcl debug/instructions.tcl
	tclsh $< > $@

unplug:
	${PLUG} -u -d token_list '' -d scanner_instructions '' -d parser_rules '' source/eaxhla.l source/eaxhla.y

plug: ${OBJECT.d}/token_list.pp ${OBJECT.d}/scanner_instructions.pp ${OBJECT.d}/parser_rules.pp
	${PLUG} -g -e token_list           ${OBJECT.d}/token_list.pp           source/eaxhla.y
	${PLUG} -g -e scanner_instructions ${OBJECT.d}/scanner_instructions.pp source/eaxhla.l
	${PLUG} -g -e parser_rules         ${OBJECT.d}/parser_rules.pp         source/eaxhla.y
	

.PHONY: test clean bootstrap
