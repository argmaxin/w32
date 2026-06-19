# (C) 2026 argmaxin
# Distributed under the terms of MIT license.
# Check LICENSE.MIT.txt accompanying this source
# for additional information.

.POSIX:

include config.mk

all: shared static bench bench-mp exe test fuzz

.SUFFIXES: .c
.SUFFIXES: .py

test: $(TEST_SOURCES)
	$(CC) $(TEST_SOURCES) $(TOUTFLAGS) $(BASEFLAGS) $(TFLAGS)

fuzz: $(FUZZ_SOURCES)
	$(CC) $(FUZZ_SOURCES) $(FOUTFLAGS) $(BASEFLAGS) $(FUZZFLAGS)

bench: $(BMRK_SOURCES)
	$(CC) $(BMRK_SOURCES) $(BOUTFLAGS) $(BASEFLAGS) $(BFLAGS)

exe: $(EXEC_SOURCES)
	$(CC) $(EXEC_SOURCES) $(EOUTFLAGS) $(BASEFLAGS) $(EFLAGS)

bench-mp: $(BMRK_SOURCES)
	$(CC) $(BMRK_SOURCES) $(MPBOUTFLAGS) $(BASEFLAGS) $(OMPBFLAGS)

object: $(COMP_SOURCES)
	$(CC) $(COMP_SOURCES) $(OUTFLAGS) $(BASEFLAGS) $(CFLAGS)

static: object
	$(AR) $(AOUTFLAGS) $(ONAME) $(ARFLAGS) 

shared: object
	$(LD) $(ONAME) $(LOUTFLAGS) $(BASEFLAGS) $(LDFLAGS)

smt-setup:
	$(VENV_SETUP)

smt: smt.py
	$(VENV_RUN)

clean:
	rm -rf $(ONAME)
	rm -rf $(ANAME) 
	rm -rf $(LNAME)
	rm -rf $(BNAME)
	rm -rf $(ENAME)
	rm -rf $(FNAME)
	rm -rf $(TNAME)
	rm -rf $(MPNAM)
