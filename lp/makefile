#-------------------------------------------------------------------------
# Makefile
#-------------------------------------------------------------------------

PROJECT=lp_solver
PROJECT_D=lp_solver_d
HEADER=header/*.h
SOURCE=source/*.cpp

CC=g++
RM=del
EXE=exe

#-------------------------------------------------------------------------
# Compiler-Flags
#
#-------------------------------------------------------------------------

OPT=-O2
DEBUG=-g
CFLAGS=-o
LINK=-L C:/SCIP/lib -lscip

#-------------------------------------------------------------------------
# Main-Targets
#-------------------------------------------------------------------------

$(PROJECT): clean $(PROJECT).$(EXE)
	@echo Successfully compiled $(PROJECT)

debug: clean_debug $(PROJECT_D).$(EXE)
	@echo Successfully compiled $(PROJECT_D)

clean_all: clean clean_debug

clean:
	@IF EXIST $(PROJECT).$(EXE) $(RM) $(PROJECT).$(EXE)

clean_debug:
	@IF EXIST $(PROJECT_D).$(EXE) $(RM) $(PROJECT_D).$(EXE)

#-------------------------------------------------------------------------
# Sub-Targets
#-------------------------------------------------------------------------

$(PROJECT).$(EXE):
	@$(CC) $(OPT) $(CFLAGS) $(PROJECT) $(HEADER) $(SOURCE) $(LINK)

$(PROJECT_D).$(EXE):
	@$(CC) $(DEBUG) $(CFLAGS) $(PROJECT_D) $(HEADER) $(SOURCE) $(LINK)