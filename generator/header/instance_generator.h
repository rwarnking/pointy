#ifndef _INSTANCE_GENERATOR_H_
#define _INSTANCE_GENERATOR_H_

#define DATA_DIR "../data/generated/"

#include <random>
#include <ctime>

#include "instance.h"

void PrintHelp();

void GenerateInstance(const char *filename, int point_count, int min=-50, int max=50, bool uniform=true);

#endif // _INSTANCE_GENERATOR_H_