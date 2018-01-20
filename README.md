# Pointy

C++ applications to solve the 4-position-labelling problem for 2D integer points

The command *make* compiles all three subprojects, while *make clean* removes all three subproject executables.

### Exact Solver (LP)

The lp solver can be compiled or removed by using one of the following commands:
 - *make lp*
 - *make lp_d* (debug)
 - *clean lp*
 - *clean lp_d* (debug)

Usage:
```
// Generates an exact solution for the instance
// in infile and writes it to outfile
lp.exe <infile> [outfile]

// Computes solutions for all files in dir
// and writes solving statistics to outfile
// ... by default number is 1
lp.exe -test [-p] <dir> <outfile> [number]

-p              saves all solutions to <infile>_sol_[1-number].bmp
```

If your are using another OS than windows, you will need to configure the makefile such that is uses the 
correct commands and builds the right type of executable file.

### Heurstic Solver

The heurstic solver can be compiled or removed by using one of the following commands:
 - *make heuristic*
 - *make heuristic_d* (debug)
 - *clean heuristic*
 - *clean heuristic_d* (debug)

Usage:
```
// Evaluates the solution in the file
heuristic.exe -eval <filename>

// Computes a solution for the instance in infile
// and writes it to outfile
heuristic.exe -in <infile> -out <outfile> [-p] [-d|-t]

-p              saves solution to <infile>_solution.bmp
-g              uses graphic heuristic
-s              uses simulated annealing
-i              uses idiot heuristic
```

If your are using another OS than windows, you will need to configure the makefile such that is uses the 
correct commands and builds the right type of executable file.

### Instance Generator

The instance generator can be compiled or removed by using one of the following commands:
 - *make generator*
 - *make generator_d* (debug)
 - *clean generator*
 - *clean generator_d* (debug)

Usage:
```
generator.exe <filename> <number of points> [min value max value] [distribution]

min value       minimum value a point can have (default -50)
max value       maximum value a point can have (default  50)
distribution    distribution to use (0 = pseudo random, 1 = uniform)
```

If your are using another OS than windows, you will need to configure the makefile such that is uses the 
correct commands and builds the right type of executable file.

### Instances

All instances used for testing can be found in the *data* folder.
