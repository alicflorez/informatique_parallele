all: master coordinator slave
	mpirun -np 1 master.out ${f} ${c}

master: master.cpp
	mpic++ master.cpp Plaque_Metal.cpp Case_Plateau.cpp -o master.out

coordinator: coordinator.cpp
	mpic++ coordinator.cpp -o coordinator.out -I/usr/local/include  -lGL -lGLU -lSDL2

slave: slave.cpp
	mpic++ slave.cpp -o slave.out

clean:
	rm -f slave.out
	rm -f coordinator.out
	rm -f master.out
