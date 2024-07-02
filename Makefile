.PHONY: clean test

ifeq (${DEBUG}, 1)
  LFLAGS   += --debug --trace
  CFLAGS   += -O0 -ggdb -fno-inline
  CPPFLAGS += -DDEBUG
  WRAP     := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all
else
  CFLAGS += -O3 -flto=auto -fno-stack-protector
endif

CFLAGS += -Wall -Wextra -Wpedantic

OUT := eaxhla

SOURCE.d  := source
OBJECT.d  := object

SOURCE    := main.c assembler.c
OBJECT    := $(addprefix ${OBJECT.d}/,${SOURCE})
OBJECT    := ${OBJECT:.c=.o}

GENSOURCE := eaxhla.yy.c eaxhla.tab.c
GENSOURCE := $(addprefix ${OBJECT.d}/,${GENSOURCE})
GENOBJECT := $(subst .c,.o,${GENSOURCE})

CPPFLAGS  += -I${OBJECT.d} -I${SOURCE.d}

${OBJECT.d}/%.yy.c: ${SOURCE.d}/%.l
	flex --header-file=object/$(basename $(notdir $<)).yy.h -o $@ $<

${OBJECT.d}/%.tab.c: ${SOURCE.d}/%.y
	bison --header=object/$(basename $(notdir $<)).tab.h -o $@ $<

${OBJECT.d}/%.yy.o: ${OBJECT.d}/%.yy.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.tab.o: ${OBJECT.d}/%.tab.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.o: ${SOURCE.d}/%.c
	${COMPILE.c} -o $@ $<

${OUT}: ${GENSOURCE} ${GENOBJECT} ${OBJECT}
	${LINK.c} -o $@ ${OBJECT} ${GENOBJECT} ${LDLIBS}

test: ${OUT}
	./${OUT} debug/test.hla

clean:
	-rm ${OUT} ${OBJECT} ${GENOBJECT} ${GENSOURCE}

.PHONY: test clean
