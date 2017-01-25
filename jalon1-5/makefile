all: master coordinator slave
	mpirun -np 1 master.out 12

master: master.cpp
	mpic++ master.cpp -o master.out

coordinator: coordinator.cpp
	mpic++ coordinator.cpp -o coordinator.out -I/usr/local/include  -lGL -lGLU -lSDL2

slave: slave.cpp
	mpic++ slave.cpp -o slave.out

clean:
	rm -f slave.out
	rm -f coordinator.out
	rm -f master.out

graphic: graphic.cpp
	mpic++ graphic.cpp -o graphic.out -I/usr/local/include  -lGL -lGLU -lSDL2
	mpirun -np 1 graphic.out 