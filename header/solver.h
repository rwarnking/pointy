#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "graph.h"
#include "instance.h"

#include <objscip/objscip.h>

class Solver
{
public:

	Solver();
	Solver(const char *filename);
	~Solver();

	bool CheckInstance(void);

	// filename, print stuff, write soution to file, draw solution bitmap, draw count (for filenames)
	int Solve(const char *filename, bool print = true, bool write = true, bool draw = false, int draw_count=1);

private:

	void HandleError(SCIP_RETCODE error_code, const char *message);

	SCIP_RETCODE InitProblem(bool print);
	SCIP_RETCODE InitVariables(SCIP_VAR **vars, bool print);
	SCIP_RETCODE InitConstraints(SCIP_VAR **vars, bool print);
	SCIP_RETCODE CopySolutionFree(SCIP_VAR **vars, bool print, bool write);

	CORNER GetCorner(int c);

	Graph *graph;
	SCIP *scip;
	int objValue;
};

#endif /* _SOLVER_H_ */