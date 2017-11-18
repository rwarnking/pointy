# Pointy

C++ application to solve the 4-position-labelling problem for 2D integer points

### Solver

The solver is located in the folder *pointy* and can be compiled using the accompanying makefile.

Usage:
```
// Evaluates the solution in the file
pointy.exe -eval <filename>

// Computes a solution for the instance in infile
// and writes it to outfile
pointy.exe -in <infile> -out <outfile> [-p] [-d|-t]

-p              saves solution to <infile>_solution.bmp
-d              uses decent heuristic
-t              uses idiot heuristic
```

If your are using another OS than windows, you will need to configure the makefile such that is uses the 
correct commands and builds the right type of executable file.

### Instance Generator

The instance generator is located in the folder *generator* and can be compiled using the accompanying makefile.

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
