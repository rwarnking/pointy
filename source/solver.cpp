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

Solver::Solver(const char *filename, bool use_cliques)
{
	graph = new Graph(filename, use_cliques);
	scip = nullptr;
	check_cliques = use_cliques;
	objValue = 0;
}

Solver::~Solver()
{
	if (graph)
		delete graph;
}

Graph* Solver::GetGraph()
{
	return graph;
}

bool Solver::CheckInstance(void)
{
	if (graph == nullptr)
		return false;

	return graph->instance.CheckSolution();
}

void Solver::HandleError(SCIP_RETCODE error_code, const char *message)
{
	if (error_code != SCIP_OKAY)
	{
		const char *error_type = nullptr;
		switch (error_code)
		{
			case SCIP_ERROR: error_type = "unspecified error"; break;
			case SCIP_NOMEMORY: error_type = "insufficient memory error"; break;
			case SCIP_READERROR: error_type = "read error"; break;
			case SCIP_WRITEERROR: error_type = "write error"; break;
			case SCIP_NOFILE: error_type = "file not found error"; break;
			case SCIP_FILECREATEERROR: error_type = "cannot create file"; break;
			case SCIP_LPERROR: error_type = "error in LP solver"; break;
			case SCIP_NOPROBLEM: error_type = "no problem exists"; break;
			case SCIP_INVALIDCALL: error_type = "method cannot be called at this time in solution process"; break;
			case SCIP_INVALIDDATA: error_type = "method cannot be called with this type of data"; break;
			case SCIP_INVALIDRESULT: error_type = "method returned an invalid result code"; break;
			case SCIP_PLUGINNOTFOUND: error_type = "a required plugin was not found"; break;
			case SCIP_PARAMETERUNKNOWN: error_type = "the parameter with the given name was not found"; break;
			case SCIP_PARAMETERWRONGTYPE: error_type = "the parameter is not of the expected type"; break;
			case SCIP_PARAMETERWRONGVAL: error_type = "the value is invalid for the given parameter"; break;
			case SCIP_KEYALREADYEXISTING: error_type = "the given key is already existing in table"; break;
			case SCIP_MAXDEPTHLEVEL: error_type = "maximal branching depth level exceeded"; break;
			case SCIP_BRANCHERROR: error_type = "branching could not be performed (e.g. too large values in variable domain)"; break;
			default: error_type = "unknown error code"; break;
		}
		Logger::PrintlnAbort(LEVEL::ERR, error_type, ": ", message);
	}
}

SCIP_RETCODE Solver::InitProblem(bool print)
{
	objValue = 0;

	SCIP_CALL(SCIPcreate(&scip));
	SCIP_CALL(SCIPincludeDefaultPlugins(scip));

	// Define Problem
	SCIP_CALL(SCIPcreateProbBasic(scip, "4-point node labelling"));
	SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

	if (print)
		Logger::Println(LEVEL::INFO, "Created Problem ...");

	return SCIP_OKAY;
}

SCIP_RETCODE Solver::InitVariables(SCIP_VAR **vars, bool print)
{
	char name[SCIP_MAXSTRLEN];
	for (int i = 0; i < (int)graph->NodeCount()*MAX_SUB; i++)
	{
		(void)SCIPsnprintf(name, SCIP_MAXSTRLEN, "x#%d", i);
		// Add one x_i for all nodes for all 4 label positions => |x| = 4*|nodes|
		SCIP_CALL(SCIPcreateVarBasic(scip, &vars[i], name, 0.0, 1.0, 1.0, SCIP_VARTYPE_BINARY));
		SCIP_CALL(SCIPaddVar(scip, vars[i]));
	}

	if (print)
		Logger::Println(LEVEL::INFO, "Added ", graph->NodeCount() * 4, " Variables ...");

	return SCIP_OKAY;
}

SCIP_RETCODE Solver::InitConstraints(SCIP_VAR **vars, bool print)
{
	char name[SCIP_MAXSTRLEN];
	// Sum of all labels for one node must be less than or equal to 1
	for (int i = 0; i < (int)graph->NodeCount(); i++)
	{
		SCIP_CONS *con;
		(void)SCIPsnprintf(name, SCIP_MAXSTRLEN, "one#label#%d", i);
		SCIP_CALL(SCIPcreateConsLinear(scip, &con, name, 0, NULL, NULL, 0, 1.0, TRUE, TRUE,
			TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));

		for (int j = 0; j < MAX_SUB; j++)
		{
			SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[i*MAX_SUB + j], 1.0));
		}

		SCIP_CALL(SCIPaddCons(scip, con));
		SCIP_CALL(SCIPreleaseCons(scip, &con));
	}

	int count = 0;
	// The sum of two labels for which there exists an edge in the graph must be less than or equal to 1
	for (auto it : graph->edges)
	{
		SCIP_CONS *con;
		(void)SCIPsnprintf(name, SCIP_MAXSTRLEN, "no#overlap#%d", count);
		SCIP_CALL(SCIPcreateConsLinear(scip, &con, name, 0, NULL, NULL, 0, 1.0, TRUE, TRUE,
			TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));

		SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[it.from.node * MAX_SUB + (short)it.from.corner], 1.0));
		SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[it.to.node * MAX_SUB + (short)it.to.corner], 1.0));

		SCIP_CALL(SCIPaddCons(scip, con));
		SCIP_CALL(SCIPreleaseCons(scip, &con));
	}

	count = 0;
	if (check_cliques)
	{
		for (auto it : graph->cliques)
		{
			SCIP_CONS *con;
			(void)SCIPsnprintf(name, SCIP_MAXSTRLEN, "no#cliques#%d", count);
			SCIP_CALL(SCIPcreateConsLinear(scip, &con, name, 0, NULL, NULL, 0, 1.0, TRUE, TRUE,
				TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));

			for (Node node : it)
			{
				SCIP_CALL(SCIPaddCoefLinear(scip, con, vars[node.node * MAX_SUB + (short)node.corner], 1.0));
			}

			SCIP_CALL(SCIPaddCons(scip, con));
			SCIP_CALL(SCIPreleaseCons(scip, &con));
		}
	}

	if (print)
		Logger::Println(LEVEL::INFO, "Added ", graph->NodeCount() + graph->EdgeCount(), " Constraints ...");

	return SCIP_OKAY;
}

SCIP_RETCODE Solver::CopySolutionFree(SCIP_VAR **vars, bool print, bool write)
{
	SCIP_SOL *solution = SCIPgetBestSol(scip);

	if (solution != NULL)
	{
		if (print)
		{
			Logger::Println(LEVEL::INFO, "Solution:");
			SCIP_CALL(SCIPprintSol(scip, solution, NULL, FALSE));
		}

		for (int i = 0; i < (int)graph->NodeCount(); i++)
		{
			for (short j = 0; j < MAX_SUB; j++)
			{
				if (SCIPgetSolVal(scip, solution, vars[i*MAX_SUB + j]) > 0.9)
				{
					graph->instance.SetBox(i, GetCorner(j));
					objValue++;
				}
			}
		}
	}
	for (int i = 0; i < (int)graph->NodeCount()*MAX_SUB; i++)
	{
		SCIP_CALL(SCIPreleaseVar(scip, &vars[i]));
	}

	SCIPfreeBlockMemoryArray(scip, &vars, graph->NodeCount() * MAX_SUB);
	SCIP_CALL(SCIPfree(&scip));

	return SCIP_OKAY;
}

int Solver::Solve(const char *filename, bool print, bool write, bool draw, int draw_count)
{
	HandleError(InitProblem(print), "Could not create problem");

	if (!print)
		HandleError(SCIPsetMessagehdlr(scip, NULL), "Could not disable SCIP output");

	SCIP_VAR **vars = nullptr;
	SCIPallocBlockMemoryArray(scip, &vars, graph->NodeCount() * MAX_SUB);

	// Add Variables
	HandleError(InitVariables(vars, print), "Could not create variables");

	// Add Constraints
	HandleError(InitConstraints(vars, print), "Could not create constraints");

	// Use SCIP to solve the ILP
	HandleError(SCIPsolve(scip), "Could not solve problem");

	// Set solution values in instance
	HandleError(CopySolutionFree(vars, print, write), "Could not free SCIP data structures");

	if (write)
		graph->instance.WriteFile(filename);

	if (draw)
	{
		std::string draw_file = string(graph->instance.Filename()) + "_sol_" + to_string(draw_count) + ".bmp";
		graph->instance.WriteToBMP(draw_file.c_str());
	}

	return objValue;
}

CORNER Solver::GetCorner(int c)
{
	switch (c)
	{
		case TOP_LEFT: return TOP_LEFT;
		case BOT_LEFT: return BOT_LEFT;
		case TOP_RIGHT: return TOP_RIGHT;
		case BOT_RIGHT: return BOT_RIGHT;
		default: return NONE;
	}
}