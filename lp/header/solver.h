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

    void SetGraph(Graph *g);

    int Test(void);
    // filename, time, take time (false), print stuff (true), write soution to file (false)
    int Solve(const char *filename, double *time=nullptr, bool take_time=false, bool print=true, bool write=true);

private:

    void HandleError(SCIP_RETCODE error_code, const char *message);

    SCIP_RETCODE InitProblem();
    SCIP_RETCODE InitVariables(SCIP_VAR **vars);
    SCIP_RETCODE InitConstraints(SCIP_VAR **vars);
    SCIP_RETCODE CopySolutionFree(SCIP_VAR **vars, bool print, bool write);

    CORNER GetCorner(int c);
    inline const char* CornerName(short corner);

    Graph *graph;
    SCIP *scip;
    int objValue;
};

#endif /* _SOLVER_H_ */