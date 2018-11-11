#========================================================================
#
# NAME
# 
#     makefile
#
# DESCRIPTION
# 
#     Makefile for MAC OS, Ubuntu Linux Unix systems and 
#     Windows/Cygwin for compiling and testing Primpoly.
#
#     Build the executables by running
#
#         make
#
#     To clean up executables and object files,
#
#         make clean
#
#     To run regression tests,
#
#         make test
#
#  LEGAL
#
#     Primpoly Version 13.0 - A Program for Computing Primitive Polynomials.
#     Copyright (C) 1999-2018 by Sean Erik O'Connor.  All Rights Reserved.
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
#    
#     The author's address is artificer!AT!seanerikoconnor!DOT!freeservers!DOT!com
#     with the !DOT! replaced by . and the !AT! replaced by @
#
#=============================================================================

# Put this pseudo-target first so make without command line options executes it.
all: makeAllExes

platform:
	@echo $(HOSTNAME)

#============================== Configuration ================================

# Get the name of the host and figure out if we are on a Windows/PC/Cygwin 
# platform.
HOSTNAME := $(shell python -c "import platform; print( platform.node())")

# Windows 7 64-bit running in Parallels running in Mac OS X.
ifeq ($(HOSTNAME), Waring)
PLATFORM = cygwin
endif

ifeq ($(HOSTNAME), Gauss)
PLATFORM = linux
endif

# Default platform is Mac OS X
ifndef PLATFORM
PLATFORM = mac
endif

#============================== Compile and Link Flags =======================

# Mac uses the clang C++/C compiler.
ifeq ($(PLATFORM), mac)
CPP     = clang++
CPPLINK = clang++

CC      = clang++ -x c
CCLINK  = clang 

# Ubuntu Linux uses the clang C++/C compiler.
else ifeq ($(PLATFORM), linux)
CPP     = clang++
CPPLINK = clang++

CC      = clang++ -x c
CCLINK  = clang 

# Windows uses the GNU C++/C compiler.
else ifeq ($(PLATFORM), cygwin)
CPP     = g++
CPPLINK = g++

CC      = gcc
CCLINK  = gcc
endif

# on = optimized code for speed.  off = compile for debugging
OPTIMIZATION = on

# Architecture (CPU type).

ifeq ($(PLATFORM), mac)
ARCH = -arch x86_64

else ifeq ($(PLATFORM), linux)
ARCH = 

else ifeq ($(PLATFORM), cygwin)
ARCH =
endif

# Include files.

ifeq ($(PLATFORM), cygwin)
CPP_INC = -I /usr/include -I "/usr/lib/gcc/x86_64-pc-cygwin/4.9.2/include/c++"
C_INC   = -I /usr/include -I "/usr/lib/gcc/x86_64-pc-cygwin/4.9.2/include"

endif

# Compile the C++11 standard and its libraries.

ifeq ($(PLATFORM), cygwin)
CPP_LIBS =-std=c++11 -L"/usr/lib" 
C_LIBS =  -L"/usr/lib" 

else ifeq ($(PLATFORM), mac)
CPP_LIBS = -std=c++11 -stdlib=libc++
C_LIBS =

else ifeq ($(PLATFORM), linux)
CPP_LIBS = -std=c++11 -stdlib=libc++
C_LIBS =

endif

# C++ and C compile and debug flags.
# Use the highest optimization level O3 unless weird stuff happens.
ifeq ($(OPTIMIZATION), on)
CPP_DEBUG_FLAGS = -O3
else
# To debug with llvm, compile with -g3 instead of -O3 optimization.
# You can switch on the following debug macros:
#
# High level algorithmic debugging:
#     #define DEBUG_PP_POLYNOMIAL
#
# Debug the LALR(1) parser for polynomials and factor tables:
#     #define DEBUG_PP_PARSER
#
# Testing primality:
#     #define DEBUG_PP_PRIMALITY_TESTING
#
# Factoring into primes:
#     #define DEBUG_PP_FACTOR
#
# Arbitrary precision arithmetic debugging:
#     #define DEBUG_PP_BIGINT
#
# Modulo p arithmetic debugging:
#     #define DEBUG_PP_ARITH
#
# Turn on to check the unit test error messages.  Default is to leave undefined.
#
#     #define DEBUG_PP_FORCE_UNIT_TEST_FAIL
#
# Turn on to check memory exceptions.  Default is to leave it off.
# This may crash some on machines with buggy C++ compilers and OS's.
#
#     #define DEBUG_PP_FORCE_MEMORY_OVERLOAD
# 
CPP_DEBUG_FLAGS = -g3 \
	-D DEBUG_PP_POLYNOMIAL \
	-D DEBUG_PP_PARSER \
	-D DEBUG_PP_PRIMALITY_TESTING \
	-D DEBUG_PP_FACTOR \
	-D DEBUG_PP_ARITH \
	-D DEBUG_PP_BIGINT \
	-D DEBUG_PP_FORCE_UNIT_TEST_FAIL
#-D DEBUG_PP_FORCE_MEMORY_OVERLOAD
endif

ifeq ($(OPTIMIZATION), on)
C_DEBUG_FLAGS = -O3
else
C_DEBUG_FLAGS = -g3 \
	-D DEBUG_PP_PRIMPOLY
endif

CPP_FLAGS = $(ARCH) $(CPP_DEBUG_FLAGS) $(CPP_INC) $(CPP_LIBS)
C_FLAGS   = $(ARCH) $(C_DEBUG_FLAGS)   $(C_INC)   $(C_LIBS)

# Link flags.

ifeq ($(PLATFORM), mac)
LFLAGS = $(ARCH)

else ifeq ($(PLATFORM), linux)
LFLAGS = -lm

endif


#============================== Root directories =============================

# Source file root directories.
CPP_SRC_DIR = ../SourceCode/Primpoly
C_SRC_DIR   = ../SourceCode/PrimpolyC

# Object file root directories.
CPP_OBJ_DIR = Bin
C_OBJ_DIR   = BinC

# Executable file root directories.
CPP_BIN_DIR = Bin
C_BIN_DIR   = BinC


#============================== List of Files ================================

# Object files.
CPP_OBJ =		$(CPP_OBJ_DIR)/ppArith.o \
				$(CPP_OBJ_DIR)/ppBigInt.o \
				$(CPP_OBJ_DIR)/ppOperationCount.o \
				$(CPP_OBJ_DIR)/ppFactor.o \
				$(CPP_OBJ_DIR)/ppParser.o \
				$(CPP_OBJ_DIR)/ppPolynomial.o \
				$(CPP_OBJ_DIR)/ppUnitTest.o

C_OBJ = 		$(C_OBJ_DIR)/ppArith.o \
				$(C_OBJ_DIR)/ppFactor.o \
				$(C_OBJ_DIR)/ppHelperFunc.o \
				$(C_OBJ_DIR)/ppIO.o \
				$(C_OBJ_DIR)/ppOrder.o \
				$(C_OBJ_DIR)/ppPolyArith.o

# Main program object files.
CPP_OBJ_MAIN = $(CPP_OBJ_DIR)/Primpoly.o
C_OBJ_MAIN   = $(C_OBJ_DIR)/Primpoly.o

# Main program executables.
CPP_BIN_MAIN = $(CPP_BIN_DIR)/Primpoly.exe
C_BIN_MAIN   = $(C_BIN_DIR)/PrimpolyC.exe


#============================== Implicit Rules ===============================

# Link all object files to executables.
$(CPP_BIN_MAIN): $(CPP_OBJ_MAIN) $(CPP_OBJ)
	$(CPPLINK) $(LFLAGS) $(CPP_OBJ_MAIN) $(CPP_OBJ) -o $(CPP_BIN_MAIN) $(CPP_LIBS)

$(C_BIN_MAIN): $(C_OBJ_MAIN) $(C_OBJ)
	$(CCLINK) $(LFLAGS) $(C_OBJ_MAIN) $(C_OBJ) -o $(C_BIN_MAIN) $(C_LIBS)


# C++ and C source file compilation to object files.
$(CPP_BIN_DIR)/%.o : $(CPP_SRC_DIR)/%.cpp
	$(CPP) -c $(CPP_FLAGS) $< -o $@

$(C_BIN_DIR)/%.o : $(C_SRC_DIR)/%.c
	$(CC) -c $(C_FLAGS) $< -o $@

# List of all file.o : file.h dependencies.
Primpoly.o:			Primpoly.h ppArith.h ppOperationCount.h ppBigInt.h ppFactor.h ppParser.h ppPolynomial.h ppUnitTest.h
ppArith.o:			Primpoly.h ppArith.h ppOperationCount.h ppBigInt.h ppFactor.h ppParser.h ppPolynomial.h ppUnitTest.h
ppBigInt.o:			Primpoly.h ppOperationCount.h ppBigInt.h ppArith.h ppFactor.h ppParser.h ppPolynomial.h ppUnitTest.h
ppOperationCount.o:	Primpoly.h ppOperationCount.h ppBigInt.h ppArith.h ppFactor.h ppParser.h ppPolynomial.h ppUnitTest.h
ppFactor.o:			Primpoly.h ppFactor.h ppArith.h ppOperationCount.h ppBigInt.h ppParser.h ppPolynomial.h ppUnitTest.h
ppParser.o:			Primpoly.h ppParser.h ppArith.h ppOperationCount.h ppBigInt.h ppFactor.h ppPolynomial.h ppUnitTest.h
ppUnitTest.o:		Primpoly.h ppUnitTest.h ppArith.h ppOperationCount.h ppBigInt.h ppFactor.h ppParser.h ppPolynomial.h

#============================== Targets ======================================

# Make the executables.
# Also copy the factor tables to the location of the executable.
c:	$(C_BIN_MAIN)

cpp:	$(CPP_BIN_MAIN)
	cp ../SourceCode/Primpoly/FactorTables/c02minus.txt $(CPP_BIN_DIR)
	cp ../SourceCode/Primpoly/FactorTables/c03minus.txt $(CPP_BIN_DIR)
	cp ../SourceCode/Primpoly/FactorTables/c05minus.txt $(CPP_BIN_DIR)
	cp ../SourceCode/Primpoly/FactorTables/c07minus.txt $(CPP_BIN_DIR)
	cp ../SourceCode/Primpoly/FactorTables/c11minus.txt $(CPP_BIN_DIR)

# Make all executables.
makeAllExes: c cpp

# Run all tests.
test: testcpp testc

# Test the C++ program against known good results.
#     Change directory to the executable and run it there so it can find the factor table files it needs.
#     @ in front of the shell command line prevents it from being printed to the console as make runs.
#     - in front of the tells make to ignore a non-zero return code from a shell command.
#     Use sed to remove version and copyright lines which would make the comparision fail every time we change versions.
testcpp:
	@echo "Running automated tests for Primpoly, C++ version."
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    2  2 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  > ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    2 17 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    2 43 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    2 62 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    3 30 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    5 22 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)    7 16 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)   17 14 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)  137  7 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)  223  8 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN)  557  6 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -sc 2 4 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -sc 3 5 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -a  2 5 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -t "x 1"| sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" | sed -e "s/line[ ]*[0-9][0-9]*//" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -t "x^4 + x + 1"           | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -t "x^3 + x + 2, 3"        | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	-@cd $(CPP_BIN_DIR) && ../$(CPP_BIN_MAIN) -t "x ^ 2 + x + 10, 32749" | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d" >> ../test.txt
	diff test.txt knownGood.txt
	-@cd ..
	-@echo "Done!"

# Test the C program against known good results.
testc:
	-@echo "Running automated tests for Primpoly, C version."
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     2  2 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"   > ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     2 17 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     2 43 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     2 62 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     3 30 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     5 22 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)     7 16 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)    17 14 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)   137  7 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)   223  8 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)   557  6 | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN) -sc 2 4  | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN) -sc 3 5  | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN)  -a 2 5  | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	-@cd $(C_BIN_DIR) && ../$(C_BIN_MAIN) -t "x 1" | sed -e "s/Primpoly Version[ ]*[0-9]*\.[0-9]*//" | sed -e "/Copyright.*/d"  >> ../testc.txt
	diff testc.txt knownGoodC.txt
	-@echo "Done!"

# Remove all C and C++ object files and binaries and factor tables.  Ignore rm return status.
clean:
	-rm $(CPP_OBJ) $(CPP_OBJ_MAIN) $(CPP_BIN_MAIN) $(CPP_OBJ_TEST)
	-rm $(C_OBJ) $(C_OBJ_MAIN) $(C_BIN_MAIN) 
	-rm test.txt testc.txt unitTest.log
	-rm $(CPP_BIN_DIR)/*.txt
