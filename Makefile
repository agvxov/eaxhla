.PHONY: clean test bootstrap
.SUFFIXES:

# --- Paths / files
SOURCE.d  := source
OBJECT.d  := object

SOURCE    := main.c assembler.c eaxhla.c unix.c
OBJECT    := $(addprefix ${OBJECT.d}/,${SOURCE})
OBJECT    := ${OBJECT:.c=.o}

LIBS      := tommy.o
LIBS      := $(addprefix ${OBJECT.d}/,${LIBS.source})

GENSOURCE := eaxhla.yy.c eaxhla.tab.c
GENSOURCE := $(addprefix ${OBJECT.d}/,${GENSOURCE})
GENOBJECT := $(subst .c,.o,${GENSOURCE})
PLUGLOCK  := token_list.pluglock scanner_instructions.pluglock parser_rules.pluglock
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

CFLAGS   += -Wall -Wextra -Wpedantic
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

${OBJECT.d}/%.pp: tool/instruction_generator/%.tcl tool/instruction_generator/instructions.tcl
	tclsh $< > $@

unplug:
	-rm ${PLUGLOCK}
	${PLUG} -u -d token_list '' -d scanner_instructions '' -d parser_rules '' source/eaxhla.l source/eaxhla.y

#plug: ${PLUGDEF}
#	${PLUG} -g -e token_list           ${OBJECT.d}/token_list.pp           source/eaxhla.y
#	${PLUG} -g -e scanner_instructions ${OBJECT.d}/scanner_instructions.pp source/eaxhla.l

${OBJECT.d}/%.pluglock: ${OBJECT.d}/%.pp
	${PLUG} -g -e $* $< ${PLUGFILES}
	touch $@

test: ${OUT}
	ORIGIN="$$(realpath .)" PATH="$$(realpath .):${PATH}" cmdtest
	${WRAP} ./${OUT} test/nop.eax

bootstrap:
	./library/bootstrap.sh

clean: unplug
	-rm ${OUT} ${OBJECT} ${GENOBJECT} ${GENSOURCE}

${OUT}: ${PLUGLOCK} ${GENSOURCE} ${GENOBJECT} ${OBJECT} ${LIB}
	${LINK.c} -o $@ ${OBJECT} ${GENOBJECT} ${LDLIBS} library/tommyds/tommy.o
