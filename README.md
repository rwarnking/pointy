# Pointy

C++ application to solve the 4-position-labelling problem for 2D integer points

### Solver

The instance solver is located in the folder *pointy* and can be compiled using the accompanying makefile.

Usage:
```
pointy.exe -eval filename // Evaluates the solution in the file

pointy.exe -in infile -out outfile [-p] [-d|-t] // Computes a solution for the instance in infile and writes it to outfile

-p          saves solution to a .bmp
-d          uses decent heuristic
-t          uses idiot heuristic
```

### Instance Generator

The instance generator is located in the folder *generator* and can be compiled using the accompanying makefile.

### Instances

All instances used for testing can be found in the *data* folder.
