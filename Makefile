.PHONY: clean test

ifeq (${DEBUG}, 1)
  LFLAGS     += --debug --trace
  CFLAGS     += -O0 -ggdb -fno-inline
  CPPFLAGS   += -DDEBUG
  FLEXFLAGS  += --trace --debug
  BISONFLAGS += --debug
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

SOURCE    := main.c assembler.c table.c
OBJECT    := $(addprefix ${OBJECT.d}/,${SOURCE})
OBJECT    := ${OBJECT:.c=.o}

# see library/, run library/bootstrap.sh
LIBS      := tommy.o
LIBS      := $(addprefix ${OBJECT.d}/,${LIBS.source})

GENSOURCE := eaxhla.yy.c eaxhla.tab.c
GENSOURCE := $(addprefix ${OBJECT.d}/,${GENSOURCE})
GENOBJECT := $(subst .c,.o,${GENSOURCE})

CPPFLAGS  += -I${OBJECT.d} -I${SOURCE.d}

all: ${OUT}

${OBJECT.d}/%.yy.c: ${SOURCE.d}/%.l
	flex ${FLEXFLAGS} --header-file=object/$(basename $(notdir $<)).yy.h -o $@ $<

${OBJECT.d}/%.tab.c: ${SOURCE.d}/%.y
	bison ${BISONFLAGS} --header=object/$(basename $(notdir $<)).tab.h -o $@ $<

${OBJECT.d}/%.yy.o: ${OBJECT.d}/%.yy.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.tab.o: ${OBJECT.d}/%.tab.c
	${COMPILE.c} -o $@ $<

${OBJECT.d}/%.o: ${SOURCE.d}/%.c
	${COMPILE.c} -o $@ $<

${OUT}: ${GENSOURCE} ${GENOBJECT} ${OBJECT} ${LIB}
	${LINK.c} -o $@ ${OBJECT} ${GENOBJECT} ${LDLIBS}

bootstrap:
	./library/bootstrap.sh

test: ${OUT}
	#fcpp -C -LL debug/xop.eax > debug/xop.eax.pp
	${WRAP} ./${OUT} debug/xop.eax

clean:
	-rm ${OUT} ${OBJECT} ${GENOBJECT} ${GENSOURCE}

.PHONY: test clean bootstrap
