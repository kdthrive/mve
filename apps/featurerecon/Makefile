MVE_ROOT := ../..
TARGET := $(shell basename `pwd`)
include ${MVE_ROOT}/Makefile.inc

CXXFLAGS += -I${MVE_ROOT}/libs ${OPENMP}
CXXFLAGS += -I/usr/include/python3.6
LDLIBS += -lpng -ltiff -ljpeg ${OPENMP}
LDLIBS += -lpython3.6m
SOURCES := $(wildcard [^_]*.cc)
${TARGET}: ${SOURCES:.cc=.o} libmve_sfm.a libmve.a libmve_util.a
	$(LINK.o) $^ $(LDLIBS) -o $@

clean:
	${RM} ${TARGET} *.o Makefile.dep

.PHONY: clean
