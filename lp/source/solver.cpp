#include "../header/solver.h"
#include "../header/logger.h"

#include <iostream>
#include <string>

#include <objscip/objscipdefplugins.h>

using namespace logger;

Solver::Solver()
{
    graph = nullptr;
    scip = nullptr;
    objValue = 0;
}

Solver::Solver(const char *filename)
{
    graph = new Graph(filename);
    scip = nullptr;
    objValue = 0;
}

Solver::~Solver()
{
    if (graph)
        delete graph;
}

void Solver::SetGraph(Graph *g)
{
    graph = g;
    objValue = 0;
}

void Solver::HandleError(SCIP_RETCODE error_code, const char *message)
{
    if (error_code != SCIP_OKAY)
        Logger::Println(LEVEL::ERR, message);
}

SCIP_RETCODE Solver::InitProblem()
{
    SCIP_CALL(SCIPcreate(&scip));
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));
    
    // Define Problem
    SCIP_CALL(SCIPcreateProbBasic(scip, "4-point node labelling"));
    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

    Logger::Println(LEVEL::INFO, "Created Problem ...");

    return SCIP_OKAY;
}

SCIP_RETCODE Solver::InitVariables(SCIP_VAR **vars)
{
    char name[SCIP_MAXSTRLEN];
    for (int i = 0; i < (int)graph->NodeCount()*MAX_SUB; i++)
    {
        (void) SCIPsnprintf(name, SCIP_MAXSTRLEN, "x#%d", i);
        // Add one x_i for all nodes for all 4 label positions => |x| = 4*|nodes|
        SCIP_CALL(SCIPcreateVarBasic(scip, &vars[i], name, 0.0, 1.0, 1.0, SCIP_VARTYPE_BINARY));
        SCIP_CALL(SCIPaddVar(scip, vars[i]));
    }

    Logger::Println(LEVEL::INFO, "Added ", graph->NodeCount()*4, " Variables ...");

    return SCIP_OKAY;
}

SCIP_RETCODE Solver::InitConstraints(SCIP_VAR **vars)
{
    char name[SCIP_MAXSTRLEN];
    // Sum of all labels for one node must be less than 1
    for (int i = 0; i < (int)graph->NodeCount(); i++)
    {
        SCIP_CONS *con;
        (void) SCIPsnprintf(name, SCIP_MAXSTRLEN, "one#label#%d", i);
        SCIP_CALL(SCIPcreateConsLinear(scip, &con, name, 0, NULL, NULL, 0, 1.0, TRUE, TRUE,
                                       TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));

        for (int j = 0; j < MAX_SUB; j++)
        {
            SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[i*MAX_SUB+j], 1.0));
        }

        SCIP_CALL(SCIPaddCons(scip, con));
        SCIP_CALL(SCIPreleaseCons(scip, &con));
    }

    int count = 0;
    // The sum of two labels for which there exists an edge in the graph must be less than 1
    for (auto it : graph->edges)
    {
        SCIP_CONS *con;
        (void) SCIPsnprintf(name, SCIP_MAXSTRLEN, "no#overlap#%d", count);
        SCIP_CALL(SCIPcreateConsLinear(scip, &con, name, 0, NULL, NULL, 0, 1.0, TRUE, TRUE,
                                       TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));

        SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[it.one * MAX_SUB + (short)it.corner_one], 1.0));
        SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[it.two * MAX_SUB + (short)it.corner_two], 1.0));

        SCIP_CALL(SCIPaddCons(scip, con));
        SCIP_CALL(SCIPreleaseCons(scip, &con));
    }

    Logger::Println(LEVEL::INFO, "Added ", graph->NodeCount()+graph->EdgeCount(), " Constraints ...");

    return SCIP_OKAY;
}

SCIP_RETCODE Solver::CopySolutionFree(SCIP_VAR **vars, bool print, bool write)
{
    SCIP_SOL *solution = SCIPgetBestSol(scip);
    if(solution != NULL)
    {
        Logger::Println(LEVEL::INFO, "Solution:");
        if (print)
            SCIP_CALL(SCIPprintSol(scip, solution, NULL, FALSE));
    }

    std::vector<Point> &points = *graph->instance->GetPoints();
    for (size_t i = 0; i < graph->NodeCount()*MAX_SUB; i++)
    {
        if (write && SCIPgetSolVal(scip, solution, vars[i]) > 0.5)
        {
            points[i/MAX_SUB].box.SetCorner(points[i].x, points[i].y, GetCorner(i%MAX_SUB));
            objValue++;
        }
        else
        {
            points[i/MAX_SUB].box.SetCorner(points[i].x, points[i].y, NONE);
        }
        SCIP_CALL(SCIPreleaseVar(scip, &vars[i]));
    }
    SCIP_CALL(SCIPfree(&scip));
}

int Solver::Solve(const char *filename, double *time, bool take_time, bool print, bool write)
{
    HandleError(InitProblem(), "Could not create problem");

    // Add Variables
    SCIP_VAR **vars = new SCIP_VAR*[graph->NodeCount()*MAX_SUB];
    HandleError(InitVariables(vars), "Could not create variables");

    // Add Constraints
    HandleError(InitConstraints(vars), "Could not create constraints");

    // Use SCIP to solve the ILP
    HandleError(SCIPsolve(scip), "Could not solve problem");

    if (take_time)
        *time = SCIPgetSolvingTime(scip);

    // Set solution values in instance
    HandleError(CopySolutionFree(vars, print, write), "Could not free SCIP data structures");

    if (write)
        graph->instance->WriteFile(filename);

    if (vars)
        delete vars;

    return objValue;
}

CORNER Solver::GetCorner(int c)
{
    switch(c)
    {
        case TOP_LEFT: return TOP_LEFT;
        case BOT_LEFT: return BOT_LEFT;
        case TOP_RIGHT: return TOP_RIGHT;
        case BOT_RIGHT: return BOT_RIGHT;
        default: return NONE;
    }
}

inline const char* Solver::CornerName(short corner)
{
    switch (corner)
    {
        case TOP_LEFT: return "top_left";
        case BOT_LEFT: return "bot_left";
        case TOP_RIGHT: return "top_right";
        case BOT_RIGHT: return "bot_right";
        default: return "none";
    }
}

int Solver::Test(void)
{
    SCIP* scip;

    SCIP_CALL(SCIPcreate(&scip));
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));

    SCIP_CALL(SCIPcreateProbBasic(scip, "test"));
    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

    SCIP_VAR *x_1, *x_2;
    SCIP_CALL(SCIPcreateVarBasic(scip, &x_1, "x#1", 0.0, SCIPinfinity(scip), 1.0, SCIP_VARTYPE_CONTINUOUS));
    SCIP_CALL(SCIPaddVar(scip, x_1));

    SCIP_CALL(SCIPcreateVarBasic(scip, &x_2, "x#2", 0.0, SCIPinfinity(scip), 2.0, SCIP_VARTYPE_CONTINUOUS));
    SCIP_CALL(SCIPaddVar(scip, x_2));

    SCIP_CONS *con;
    SCIP_CALL(SCIPcreateConsLinear(scip, &con, "constraint", 0, NULL, NULL, 0, 20.0, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));
    SCIP_CALL(SCIPaddCoefLinear(scip, con, x_1, 5.0));
    SCIP_CALL(SCIPaddCoefLinear(scip, con, x_2, 2.0));
    SCIP_CALL(SCIPaddCons(scip, con));
    SCIP_CALL(SCIPreleaseCons(scip, &con));

    SCIP_CALL(SCIPsolve(scip));

    if( SCIPgetNSols(scip) > 0 )
    {
       std::cout << "Solution:\n" << std::endl;
       SCIP_CALL( SCIPprintSol(scip, SCIPgetBestSol(scip), NULL, FALSE) );
    }

    SCIP_CALL(SCIPreleaseVar(scip, &x_1));
    SCIP_CALL(SCIPreleaseVar(scip, &x_2));
    SCIP_CALL(SCIPfree(&scip));
}