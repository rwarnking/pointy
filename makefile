#-------------------------------------------------------------------------
# NMAKE-Makefile
#-------------------------------------------------------------------------

PROJECT=pointy
GEN=generator
CC=g++
RM=del

#-------------------------------------------------------------------------
# Compiler-Flags
#-------------------------------------------------------------------------

CFLAGS=-o

#-------------------------------------------------------------------------
# Main-Targets
#-------------------------------------------------------------------------

$(PROJECT): $(PROJECT).exe
	@echo Successfully compiled $(PROJECT)

$(GEN): $(GEN).exe
	@echo Successfully compiled $(GEN)

all: $(PROJECT) $(GEN)

clean:
	$(RM) $(PROJECT).exe $(GEN).exe

#-------------------------------------------------------------------------
# Sub-Targets
#-------------------------------------------------------------------------

$(PROJECT).exe:
	$(CC) $(CFLAGS) $(PROJECT) header/color.h header/bitmap.h header/instance.h header/problem.h source/problem.cpp source/instance.cpp source/pointy.cpp

$(GEN).exe:
	$(CC) $(CFLAGS) $(GEN) header/instance_generator.h header/instance.h source/instance_generator.cpp source/instance.cpp