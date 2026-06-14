# (C) 2026 argmaxin
# Distributed under the terms of MIT license.
# Check LICENSE.MIT.txt accompanying this source
# for additional information.

# Toolchain
CC=$$(command -v clang >/dev/null 2>&1 && echo "clang" || echo "cc") 
LD=$(CC)
AR=ar

ONAME=w32.o
BNAME=w32b.exe  # for ease of .gitignore
TNAME=w32t.exe # ^
FNAME=w32f.exe # ^
LNAME=libw32.so
ANAME=libw32.a

# Configuration
BASEFLAGS=-Wall -Wextra -pedantic -ansi
COMPFLAGS=-c -fPIC -fno-stack-protector
TESTFLAGS= -std=c99
BMRKFLAGS= -fno-stack-protector
FUZZFLAGS= -fsanitize=fuzzer,address,undefined 
LINKFLAGS=-Wl,--as-needed,-z,now,-z,relro,-z,defs

# Output Flags

OUTFLAGS=-o $(ONAME) 
LOUTFLAGS=-o $(LNAME)
AOUTFLAGS=rcs $(ANAME)
TOUTFLAGS=-o $(TNAME)
BOUTFLAGS=-o $(BNAME)
FOUTFLAGS=-o $(FNAME)

# Invocation
CFLAGS= -march=x86-64 -O2 $(COMPFLAGS)
TFLAGS= -march=x86-64 -O2 $(TESTFLAGS)
BFLAGS= -march=x86-64 -O2 $(BMRKFLAGS)
ARFLAGS = 
LDFLAGS = -shared $(LINKFLAGS) 

# Project
COMP_SOURCES=w32.c
TEST_SOURCES=w32.c tst.c test.c
BMRK_SOURCES=w32.c bench.c
FUZZ_SOURCES=w32.c fuzz.c

# SMT (Optional)
PYTHON=python3
PYTHON_EXE=$$(command -v $(PYTHON) >/dev/null 2>&1 && echo "$(PYTHON)" || echo "VENV ERR" && exit 1)
VENV_DIR = venv
VENV_INIT =  source $(VENV_DIR)/bin/activate 
VENV_SETUP = $(PYTHON_EXE) -m venv $(VENV_DIR) && $(VENV_INIT) && $(PYTHON) -m pip install z3-solver
VENV_RUN =  $(VENV_INIT) && $(PYTHON_EXE) smt.py

