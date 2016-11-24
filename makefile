all: master coordinator slave
	mpirun -np 1 master.out

master: master.cpp
	mpic++ master.cpp -o master.out

coordinator: coordinator.cpp
	mpic++ coordinator.cpp -o coordinator.out

slave: slave.cpp
	mpic++ slave.cpp -o slave.out

clean:
	rm -f slave.out
	rm -f coordinator.out
	rm -f master.out
