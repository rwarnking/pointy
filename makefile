CC = g++
RM = del

all: program
	@echo Successfully compiled pointy

program:
	$(CC) -o pointy header/problem.h source/problem.cpp source/pointy.cpp

clean:
	$(RM) pointy.exe

