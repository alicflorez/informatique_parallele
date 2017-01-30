# informatique_parallele
Projet d'informatique parallèle de 3ème année - INFRES7

Jonathan BARONCI
Aymeric COSSARD
Alicia FLOREZ

## Install

Télécharger et installer MPI

```
$ wget https://www.open-mpi.org/software/ompi/v2.0/downloads/openmpi-2.0.1.tar.gz
$ tar -xvzf openmpi-2.0.1.tar.gz 
$ cd openmpi-2.0.1/
$ ./configure 
$ make all
# make install
```

Télécharger et installer DSL2 https://www.libsdl.org/download-2.0.php (procédure dans install.txt)

## Lancer

```
$ make all f="path_relatif_du_fichier" c="nombre_de_cycles" m="taille_cote_matrice_chaque_case" t="temperature_ambiante" v="vitesse_defilement"

vitesse:
	0 = 1 seconde de pause entre iterations
	1 = 0.7 secondes de pause entre iterations
	2 = 0.4 secondes de pause entre iterations
	3 = 0.1 secondes de pause entre iterations

```
