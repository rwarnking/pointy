#ifndef _SIMULATED_ANNEALING_H_
#define _SIMULATED_ANNEALING_H_

#include "instance.h"

/**
 * Solves the 4-point labelling problem for a given instance
 * by employing a simulated annealing algorithm
 */
class SimulatedAnnealing
{
public:

    SimulatedAnnealing();

    SimulatedAnnealing(size_t maxiter);

    ~SimulatedAnnealing();

    void SetIterations(size_t max_iter);

    void SetInitialSolution(bool random);

    size_t Solve(Instance *instance);

    void PrintParameters();

private:

    void ConstructSolution();

    bool StopIteration(size_t iter);

    void GenerateInitialSolution(std::vector<Box> &solution);

    CORNER GetBestOrientation(int x, int y);

    CORNER NextCorner(CORNER before, short add);

    bool UseSolution(int iter, size_t current_value, size_t next_value);

    size_t ChooseNeighbour(std::vector<Box> &solution, std::vector<Box> &next, size_t current_value);

    void DeleteOneBox(std::vector<Box> &solution);

    bool IsFeasible(std::vector<Box> &solution);

    bool IntersectsWithNone(std::vector<Box> &solution, int index);

    double Cooling(int iter, size_t current_value, size_t next_value);

    // Instance to solve
    Instance *instance;

    // Best solution found
    std::vector<Box> opt;
    size_t objective_value;

    // TODO other parameters
    size_t iterations;
    bool start_random;
    bool move_random;
};

#endif