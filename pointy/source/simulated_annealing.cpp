#include "../header/simulated_annealing.h"
#include "../header/logger.h"
 
#include <random>    // random uniform distribution
#include <algorithm> // für min() und max()
#include <cstring>

#define ALL_USED 15

using namespace std;
using namespace logger;

SimulatedAnnealing::SimulatedAnnealing() : SimulatedAnnealing(100000, 10000, 100, 1000, 10000, 0.05, true, false, true) {}

SimulatedAnnealing::SimulatedAnnealing(size_t maxiter,
                                       size_t maxneighbour,
                                       size_t maxindex,
                                       size_t maxtabu,
                                       size_t maximprove,
                                       double improve,
                                       bool random_start,
                                       bool random_move,
                                       bool tabu_usage)
{ 
    objective_value = 0;
    neighbour_search = maxneighbour;
    index_search = maxindex;
    iterations = maxiter;
    max_tabu = maxtabu;
    tabu_count = 0;
    threshold_max = maximprove;
    threshold_count = 0;
    threshold = improve;
    start_random = random_start;
    move_random = random_move;
    use_tabu = tabu_usage;
    instance = nullptr;
    opt = vector<Box>();
    tabu = nullptr;
}

SimulatedAnnealing::~SimulatedAnnealing()
{
    // Free allocated memory
    if (tabu) {
        delete tabu;
        tabu = nullptr;
    }
}

void SimulatedAnnealing::SetIterations(size_t max)
{
    iterations = max;
}

void SimulatedAnnealing::SetNeighbourSearch(size_t max)
{
    neighbour_search = max;
}

void SimulatedAnnealing::SetIndexSearch(size_t max)
{
    index_search = max;
}

void SimulatedAnnealing::SetRandomStart(bool random)
{
    start_random = random;
}

void SimulatedAnnealing::SetRandomMove(bool random)
{
    move_random = random;
}

void SimulatedAnnealing::SetTabuList(bool use)
{
    use_tabu = use;
}

void SimulatedAnnealing::PrintParameters()
{
    Logger::Println(LEVEL::DEBUG, "\nSimulated Annealing:");
    Logger::Println(LEVEL::DEBUG, "\t objective_value   = ", objective_value);
    Logger::Println(LEVEL::DEBUG, "\t start random      = ", (start_random ? "true" : "false"));
    Logger::Println(LEVEL::DEBUG, "\t move random       = ", (move_random ? "true" : "false"));
    Logger::Println(LEVEL::DEBUG, "\t max iterations    = ", iterations);
    Logger::Println(LEVEL::DEBUG, "\t neighbour search  = ", neighbour_search);
    Logger::Println(LEVEL::DEBUG, "\t threshold         = ", threshold);
    Logger::Println(LEVEL::DEBUG, "\t threshold count   = ", threshold_count);
    Logger::Println(LEVEL::DEBUG, "\t max threshold     = ", threshold_max);
    Logger::Println(LEVEL::DEBUG, "\t tabu count        = ", tabu_count);
    Logger::Println(LEVEL::DEBUG, "\t max tabu          = ", max_tabu , "\n");

}

size_t SimulatedAnnealing::Solve(Instance *problem_instance)
{
    if (index_search > 2*problem_instance->GetPointCount())
        index_search = 2*problem_instance->GetPointCount();

    PrintParameters();

    instance = problem_instance;

    vector<Point> &points = *instance->GetPoints();
    vector<Box> work = vector<Box>();
    work.reserve(points.size());

    // Initialize and fill tabu list
    tabu = new short[points.size()];
    fill_n(tabu, points.size(), 0);

    for (size_t i = 0; i < points.size(); i++)
    {
        work.push_back(points[i].box);
    }

    // Generate an initial solution according to solver parameters
    GenerateInitialSolution(work);

    size_t next_value, iter;
    size_t current_value = objective_value;

    Logger::Println(LEVEL::DEBUG, "\nInitial value: ", objective_value);

    // Main loop
    for (iter = 0; !StopIteration(iter); iter++)
    {
        // Generate next solution
        vector<Box> next = work;
        next_value = ChooseNeighbour(work, next, current_value);

        // Check if next solution should be taken
        if (UseSolution(iter, current_value, next_value))
        {
            work = move(next);
            current_value = next_value;

            // Update optimal solution if new one is better
            if (current_value > objective_value)
            {
                if (((double) current_value / objective_value) - 1.0 < threshold)
                    threshold_count++;
                else
                    threshold_count = 0;

                opt = work;
                objective_value = current_value;
            }
        }
    }

    Logger::Println(LEVEL::DEBUG, "\nIterations: ", iter);

    ConstructSolution();

    return objective_value;
}

bool SimulatedAnnealing::UseSolution(size_t iter, size_t current_value, size_t next_value)
{
    default_random_engine gen;
    uniform_real_distribution<double> dis(0.0, 1.0);

    return dis(gen) < min(1.0, Cooling(iter, current_value, next_value));
}

void SimulatedAnnealing::ConstructSolution()
{
    vector<Point> &points = *instance->GetPoints();

    Logger::Println(LEVEL::DEBUG);
    for (size_t i = 0; i < opt.size(); i++)
    {
        points[i].box.x = opt[i].x;
        points[i].box.y = opt[i].y;
        points[i].box.corner = opt[i].corner;
    }
    Logger::Println(LEVEL::DEBUG);
}

bool SimulatedAnnealing::StopIteration(size_t iter)
{
    return objective_value == instance->GetPointCount() || iter >= iterations || threshold_count >= threshold_max;
}

void SimulatedAnnealing::GenerateInitialSolution(std::vector<Box> &solution)
{
    vector<Point> &points = *instance->GetPoints();

    default_random_engine gen;
    uniform_int_distribution<short> dis(BOT_LEFT, NONE-1);

    for (size_t i = 0; i < solution.size(); i++)
    {
        solution[i].SetCorner(points[i].x, points[i].y, start_random ? (CORNER) dis(gen) : GetBestOrientation(points[i].x, points[i].y));
        
        if (IntersectsWithNone(solution, i))
        {
            ++objective_value;
            if (use_tabu)
                UpdateTabuList(i, solution[i].corner);
        }
        else
        {
            solution[i].SetCorner(points[i].x, points[i].y, NONE);
        }
    }
    opt = solution;
}

void SimulatedAnnealing::UpdateTabuList(size_t index, short corner)
{
    if (tabu_count == max_tabu)
    {
        tabu_count = 1;
        fill_n(tabu, instance->GetPointCount(), 0);
    }

    if (tabu[index] != ALL_USED)
        tabu[index] = (tabu[index] << 1) + 1;
}

CORNER SimulatedAnnealing::GetBestOrientation(int x, int y)
{
    int middle_x = instance->GetMiddleX();
    int middle_y = instance->GetMiddleY();

    if (x <= middle_x)
        return y < middle_y ? TOP_RIGHT : BOT_RIGHT;
    else
        return y < middle_y ? TOP_LEFT : BOT_LEFT;
}

size_t SimulatedAnnealing::ChooseNeighbour(std::vector<Box> &solution, std::vector<Box> &next, size_t current_value)
{
    if (current_value == solution.size())
        return current_value;

    vector<Point> &points = *instance->GetPoints();

    int index = 0;
    bool found = false;
    size_t solution_value = current_value;
    CORNER tmp_corner = NONE;
    CORNER before = solution[index].corner;

    if (move_random)
    {

        int i = 0;
        do
        {
            index = rand() % solution.size();
        } while (solution[index].corner == before && ++i < index_search);

        before = solution[index].corner;
        do
        {
            tmp_corner = (CORNER) (rand() % (NONE+1));
        } while (tmp_corner == before && !IsTabu(index));

        solution[index].SetCorner(points[index].x, points[index].y, tmp_corner);
        
        if (use_tabu)
            UpdateTabuList(index, tmp_corner);
        if (before == NONE && IntersectsWithNone(next, index))
            solution_value++;
    }
    else
    {
        // TODO: for some reason this does not work, dont ask me why FML
        //uniform_int_distribution<int> dis_index(0, solution.size()-1);
        
        for (int i = 0, j = 0; i < neighbour_search && !found; i++)
        {
            // Reset (first time does nothing, otherwise reset previous change)
            next[index].SetCorner(points[index].x, points[index].y, before);

            // Get new index (if item is not tabu and already placed)
            j = 0;
            do {
                index = rand() % solution.size(); //dis_index(gen);
                j++;
            } while ((solution[index].corner != NONE || IsTabu(index)) && j < index_search);
            
            // Set label corner to sth different than before
            before = solution[index].corner;
            tmp_corner = NextCorner(before, tabu[index]);
            
            next[index].SetCorner(points[index].x, points[index].y, tmp_corner);
            
            if (use_tabu)
                UpdateTabuList(index, tmp_corner);
            if (IntersectsWithNone(next, index))
                found = true;
        }

        if (found && before == NONE)
        {
            solution_value++;
        }
        else if (!found)
        {
            DeleteOneBox(solution);
            solution_value--;
        }
    }

    return solution_value;
}

bool SimulatedAnnealing::IsTabu(size_t index)
{
    return use_tabu && tabu[index] == ALL_USED;
}

void SimulatedAnnealing::DeleteOneBox(std::vector<Box> &solution)
{
    vector<Point> &points = *instance->GetPoints();

    int index;
    do
    {
        index = rand() % points.size();
    } while (solution[index].corner == NONE);

    solution[index].SetCorner(points[index].x, points[index].y, NONE);
    tabu[index] = 0;
}

CORNER SimulatedAnnealing::NextCorner(CORNER before, short pos)
{
    short mask = 8;
    bool tmp[4] = { false, false, false, false };
    for (short i = 0; i < 4; i++)
    {
        tmp[i] = (pos & mask) == mask;
        pos = pos >> 1;
        mask = mask >> 1;
    }

    for (short i = 0; i < 4; i++)
    {
        if (tmp[i] == false)
            return (CORNER) i;
    }

    return before;
}

bool SimulatedAnnealing::IntersectsWithNone(std::vector<Box> &solution, size_t index)
{
    for (int i = 0; i < solution.size(); i++)
    {
        if (i != index && solution[index].Intersects(solution[i]))
            return false;
    }

    return true;
}

double SimulatedAnnealing::Cooling(size_t iter, size_t current_value, size_t next_value)
{
    double denom = pow((double) iter, 2.0);

    return exp(-1.0 * ((next_value - current_value) / denom));
}
