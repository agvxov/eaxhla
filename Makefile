.PHONY: clean test bootstrap
.SUFFIXES:

# --- Paths / files
SOURCE.d  := source
OBJECT.d  := object

SOURCE    := main.c assembler.c eaxhla.c compile.c unix.c
OBJECT    := $(addprefix ${OBJECT.d}/,${SOURCE})
OBJECT    := ${OBJECT:.c=.o}

LIBS      := tommy.o
LIBS      := $(addprefix ${OBJECT.d}/,${LIBS.source})

GENSOURCE := eaxhla.yy.c eaxhla.tab.c
GENSOURCE := $(addprefix ${OBJECT.d}/,${GENSOURCE})
GENOBJECT := $(subst .c,.o,${GENSOURCE})
PLUGLOCK  := instruction_token_list.pluglock instruction_scanner_instructions.pluglock instruction_parser_rules.pluglock register_token_list.pluglock register_scanner_instructions.pluglock register_parser_rules.pluglock
PLUGLOCK  := $(addprefix ${OBJECT.d}/,${PLUGLOCK})
PLUGFILES := ${SOURCE.d}/eaxhla.y ${SOURCE.d}/eaxhla.l

OUT := eaxhla

# --- Tools/Flags
PLUG := tool/plug

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

CFLAGS   += -Wall -Wextra -Wpedantic # --std=c2x
CPPFLAGS += -Ilibrary/ -I${OBJECT.d} -I${SOURCE.d}
LDLIBS := -lm

# --- Rule Section ---

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

${OBJECT.d}/%.pp: tool/generators/%.tcl tool/generators/instructions.tcl tool/generators/registers.tcl
	tclsh $< > $@

unplug:
	-rm ${PLUGLOCK}
	${PLUG} -u -a source/eaxhla.l source/eaxhla.y

${OBJECT.d}/%.pluglock: ${OBJECT.d}/%.pp
	${PLUG} -g -e $* $< ${PLUGFILES}
	touch $@

test: ${OUT}
	-ORIGIN="$$(realpath .)" PATH="$$(realpath .):${PATH}" cmdtest
	-${WRAP} ./${OUT} test/nop.eax

bootstrap:
	./library/bootstrap.sh

deepclean: unplug clean

clean:
	-rm ${GENSOURCE}
	-rm ${GENOBJECT}
	-rm ${OBJECT}
	-rm ${OUT}
	-rm token_dump
	-rm a.out

${OUT}: ${PLUGLOCK} ${GENSOURCE} ${GENOBJECT} ${OBJECT} ${LIB}
	${LINK.c} -o $@ ${OBJECT} ${GENOBJECT} ${LDLIBS} library/tommyds/tommy.o library/sds/sds.o
