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

    SimulatedAnnealing(size_t maxiter,
                       size_t maxneighbour,
                       size_t maxindex,
                       size_t maxtabu,
                       size_t maximprove,
                       double improve,
                       bool random_start,
                       bool random_move,
                       bool tabu_usage);

    ~SimulatedAnnealing();

    void SetIterations(size_t max);
    
    void SetNeighbourSearch(size_t max);
    
    void SetIndexSearch(size_t max);

    void SetRandomStart(bool random);

    void SetRandomMove(bool random);

    void SetTabuList(bool use);

    size_t Solve(Instance *instance);

    void PrintParameters();

private:

    void ConstructSolution();

    bool StopIteration(size_t iter);

    void GenerateInitialSolution(std::vector<Box> &solution);

    void UpdateTabuList(size_t index, short corner);

    CORNER GetBestOrientation(int x, int y);

    CORNER NextCorner(CORNER before, short pos);

    bool UseSolution(size_t iter, size_t current_value, size_t next_value);

    size_t ChooseNeighbour(std::vector<Box> &solution, std::vector<Box> &next, size_t current_value);

    bool IsTabu(size_t index);

    void DeleteOneBox(std::vector<Box> &solution);

    bool IsFeasible(std::vector<Box> &solution);

    bool IntersectsWithNone(std::vector<Box> &solution, size_t index);

    double Cooling(size_t iter, size_t current_value, size_t next_value);

    // Instance to solve
    Instance *instance;

    // Best solution found
    std::vector<Box> opt;
    size_t objective_value;

    // Help stuff
    short *tabu;

    // TODO other parameters
    bool start_random;
    bool move_random;
    bool use_tabu;

    size_t iterations;
    size_t neighbour_search;
    size_t index_search;
    size_t max_tabu;
    size_t tabu_count;

    size_t threshold_max;
    size_t threshold_count;
    double threshold;
};

#endif