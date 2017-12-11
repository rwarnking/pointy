#include "../header/simulated_annealing.h"
#include <random>

using namespace std;

SimulatedAnnealing::SimulatedAnnealing()
{
    SimulatedAnnealing(0);
}

SimulatedAnnealing::SimulatedAnnealing(size_t maxiter) :
    objective_value(0),
    iterations(maxiter),
    start_random(true),
    move_random(true),
    instance(nullptr),
    opt(vector<Box>()) {}

SimulatedAnnealing::~SimulatedAnnealing()
{
    // TODO ?
}

void SimulatedAnnealing::SetIterations(size_t max_iter)
{
    iterations = max_iter;
}

void SimulatedAnnealing::SetInitialSolution(bool random)
{
    start_random = random;
}

size_t SimulatedAnnealing::Solve(Instance *problem_instance)
{
    instance = problem_instance;

    vector<Point> &points = *instance->GetPoints();
    vector<Box> work = vector<Box>();
    work.reserve(points.size());

    // Construct initial solution representation
    for (size_t i = 0; i < points.size(); i++)
    {
        work.push_back(points[i].box);
    }

    // Generate an initial solution according to solver parameters
    GenerateInitialSolution(work);

    size_t next_value;
    size_t current_value = objective_value;

    cout << "initial value: " << current_value << endl;
    for (size_t iter = 0; !StopIteration(iter); iter++) {

        // Generate next solution
        vector<Box> next = work;
        next_value = ChooseNeighbour(work, next, current_value);

        // Check if next solution should be taken
        if (UseSolution(iter, current_value, next_value))
        {
            cout << "took solution " << iter << " with " << next_value << endl;
            work = move(next);
            current_value = next_value;

            // Update optimal solution if new one is better
            if (current_value > objective_value)
            {
                opt = work;
                objective_value = current_value;
            }
        }
    }

    ConstructSolution();

    return objective_value;
}

bool SimulatedAnnealing::UseSolution(int iter, size_t current_value, size_t next_value)
{
    default_random_engine gen;
    uniform_real_distribution<double> dis(0.0, 1.0);

    return dis(gen) < min(1.0, Cooling(iter, current_value, next_value));
}

void SimulatedAnnealing::ConstructSolution()
{
    vector<Point> &points = *instance->GetPoints();

    for (size_t i = 0; i < opt.size(); i++)
    {
        points[i].box.x = opt[i].x;
        points[i].box.y = opt[i].y;
        points[i].box.corner = opt[i].corner;
    }
}

bool SimulatedAnnealing::StopIteration(size_t iter)
{
    // TODO: add more options
    return iter >= iterations;
}

void SimulatedAnnealing::GenerateInitialSolution(std::vector<Box> &solution)
{
    vector<Point> &points = *instance->GetPoints();

    if (start_random)
    {
        default_random_engine gen;
        uniform_int_distribution<short> dis(BOT_LEFT, NONE-1);
        
        for (size_t i = 0; i < solution.size(); i++)
        {
            solution[i].corner = (CORNER) dis(gen);
            
            if (IntersectsWithNone(solution, i))
                objective_value++;
            else
                solution[i].corner = NONE;
        }
        opt = solution;
    }
    else
    {
        for (size_t i = 0; i < points.size(); i++)
        {
            solution[i].corner = GetBestOrientation(points[i].x, points[i].y);
            objective_value++;
            for (size_t j = 0; j < i; j++)
            {
                if (solution[i].Intersects(solution[j]))
                {
                    solution[i].corner = NONE;
                    objective_value--;
                    break;
                }
            }
        }
    }
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
    vector<Point> &points = *instance->GetPoints();

    if (move_random)
    {
        default_random_engine gen;
        uniform_int_distribution<int> dis_index(0, solution.size()-1);
        uniform_int_distribution<short> dis_corner(0, 5);

        int index = 0;
        bool found = false;
        size_t solution_value;
        CORNER tmp_corner = NONE;
        CORNER before = solution[index].corner;
        
        for (int i = 0; i < iterations/10 && !found; i++)
        {
            // Reset (first time does nothing, otherwise reset previous change)
            next[index].corner = before;

            // Get new index and set label corner to sth different than before
            index = dis_index(gen);
            before = solution[index].corner;
            do
            {
                tmp_corner = (CORNER) dis_corner(gen);  //NextCorner(before, dis_corner(gen));
            } while(tmp_corner == before && before != NONE);
            next[index].corner = tmp_corner;

            if (!IntersectsWithNone(next, index))
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

        return solution_value;
    }
    else
    {
        // TODO: do sth smarter

        // suggestion: count elements per quadrant, take those with less elements
    }

    return 0;
}

void SimulatedAnnealing::DeleteOneBox(std::vector<Box> &solution)
{
    default_random_engine gen;
    uniform_int_distribution<int> dis_index(0, solution.size()-1);

    int index;
    do
    {
        index = dis_index(gen);
    } while (solution[index].corner != NONE);

    solution[index].corner == NONE;
}

CORNER SimulatedAnnealing::NextCorner(CORNER before, short add)
{
    if (before + add <= NONE)
        return (CORNER) (before + add);
    else
        return (CORNER) (before + add - NONE);
}

bool SimulatedAnnealing::IsFeasible(std::vector<Box> &solution)
{
    for (int i = 0; i < solution.size(); i++)
    {
        for (int j = i+1; j < solution.size(); j++)
        {
            if (solution[i].Intersects(solution[j]))
                return false;
        }
    }

    return true;
}

bool SimulatedAnnealing::IntersectsWithNone(std::vector<Box> &solution, int index)
{
    for (int i = 0; i < solution.size(); i++)
    {
        if (i != index && solution[index].Intersects(solution[i]))
            return false;
    }

    return true;
}

double SimulatedAnnealing::Cooling(int iter, size_t current_value, size_t next_value)
{
    double denom = pow((double) iter, 2.0);

    return exp(-1.0 * ((next_value - current_value) / denom));
}
