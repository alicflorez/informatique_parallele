all: master 
	mpirun -np 1 main.out 12

master: main.cpp
	mpic++  -fopenmp main.cpp -o main.out

clean:
	rm -f main.out
