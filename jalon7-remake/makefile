all: master coordinator slave
	mpirun -np 1 master.out ${f} ${c} ${m} ${t} ${v}

master: master.cpp
	mpic++ master.cpp Plaque_Metal.cpp -o master.out -L/rep/SDL2/lib -I/usr/local/include  -lGL -lGLU -lSDL2 -Wno-write-strings

coordinator: coordinator.cpp
	mpic++ coordinator.cpp IHM.cpp Plaque_Metal.cpp -o coordinator.out -I/usr/local/include  -lGL -lGLU -lSDL2

slave: slave.cpp
	mpic++ slave.cpp -o slave.out

clean:
	rm -f slave.out
	rm -f coordinator.out
	rm -f master.out
