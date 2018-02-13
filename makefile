#-------------------------------------------------------------------------
# Makefile
#-------------------------------------------------------------------------

LP=lp
HEURISTIC=heuristic
GENERATOR=generator
DEBUG=_d

LP_INC=header/instance.h header/graph.h header/logger.h header/solver.h header/lp_tester.h header/bitmap.h
LP_SRC=source/instance.cpp source/graph.cpp source/solver.cpp source/lp_tester.cpp source/lp.cpp

HEUR_INC=header/instance.h header/logger.h header/simulated_annealing.h header/problem.h header/bitmap.h
HEUR_SRC=source/instance.cpp source/simulated_annealing.cpp source/problem.cpp source/heuristic.cpp

GEN_INC=header/instance.h header/instance_generator.h
GEN_SRC=source/instance.cpp source/instance_generator.cpp

# Windows commands - must be changed on different OS
CC=g++
RM=del
EXE=exe

#-------------------------------------------------------------------------
# Compiler-Flags
#
#-------------------------------------------------------------------------

OPT_FLAG=-O2
DEBUG_FLAG=-g
CFLAGS=-o
# SCIP directory and libname - must be changed if it varies
LINK_SCIP=-L C:/SCIP/lib -lscip

#-------------------------------------------------------------------------
# Main-Targets
#-------------------------------------------------------------------------

all: clean $(LP) $(HEURISTIC) $(GENERATOR)
	@echo Successfully compiled all projects

all_debug: clean $(LP)$(DEBUG) $(HEURISTIC)$(DEBUG) $(GENERATOR)$(DEBUG)
	@echo Successfully compiled all projects (debug)

clean: clean_lp clean_heuristic clean_generator

clean_lp:
	@IF EXIST $(LP).$(EXE) $(RM) $(LP).$(EXE)
	@IF EXIST $(LP)$(DEBUG).$(EXE) $(RM) $(LP)$(DEBUG).$(EXE)

clean_heuristic:
	@IF EXIST $(HEURISTIC).$(EXE) $(RM) $(HEURISTIC).$(EXE)
	@IF EXIST $(HEURISTIC)$(DEBUG).$(EXE) $(RM) $(HEURISTIC)$(DEBUG).$(EXE)

clean_generator:
	@IF EXIST $(GENERATOR).$(EXE) $(RM) $(GENERATOR).$(EXE)
	@IF EXIST $(GENERATOR)$(DEBUG).$(EXE) $(RM) $(GENERATOR)$(DEBUG).$(EXE)

$(LP): clean_lp $(LP).$(EXE)
	@echo Successfully compiled $(LP)

$(LP)$(DEBUG): clean_lp $(LP)$(DEBUG).$(EXE)
	@echo Successfully compiled $(LP) (debug)

$(HEURISTIC): clean_heuristic $(HEURISTIC).$(EXE)
	@echo Successfully compiled $(HEURISTIC)

$(HEURISTIC)$(DEBUG): clean_heuristic $(HEURISTIC)$(DEBUG).$(EXE)
	@echo Successfully compiled $(HEURISTIC) (debug)

$(GENERATOR): clean_generator $(GENERATOR).$(EXE)
	@echo Successfully compiled $(GENERATOR)

$(GENERATOR)$(DEBUG): clean_generator $(GENERATOR)$(DEBUG).$(EXE)
	@echo Successfully compiled $(GENERATOR) (debug)

#-------------------------------------------------------------------------
# Sub-Targets
#-------------------------------------------------------------------------

# LP solver
$(LP).$(EXE):
	@$(CC) $(OPT_FLAG) $(CFLAGS) $(LP) $(LP_INC) $(LP_SRC) $(LINK_SCIP)

$(LP)$(DEBUG).$(EXE):
	@$(CC) $(DEBUG_FLAG) $(CFLAGS) $(LP)$(DEBUG) $(LP_INC) $(LP_SRC) $(LINK_SCIP)

# Heuristic solver
$(HEURISTIC).$(EXE):
	@$(CC) $(OPT_FLAG) $(CFLAGS) $(HEURISTIC) $(HEUR_INC) $(HEUR_SRC)

$(HEURISTIC)$(DEBUG).$(EXE):
	@$(CC) $(DEBUG_FLAG) $(CFLAGS) $(HEURISTIC)$(DEBUG) $(HEUR_INC) $(HEUR_SRC)

# Instance generator
$(GENERATOR).$(EXE):
	@$(CC) $(OPT_FLAG) $(CFLAGS) $(GENERATOR) $(GEN_INC) $(GEN_SRC)

$(GENERATOR)$(DEBUG).$(EXE):
	@$(CC) $(DEBUG_FLAG) $(CFLAGS) $(GENERATOR)$(DEBUG) $(GEN_INC) $(GEN_SRC)