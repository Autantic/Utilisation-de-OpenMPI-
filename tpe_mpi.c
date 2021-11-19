#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#include <time.h>


int somme_matricielle();

void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator);

void mpi();

//*****************************************************************************
//*****************************************************************************


int main(int argc, char** argv) {

  double r_sec, t_sec = CLOCKS_PER_SEC;
  clock_t t1, t2;
  t_sec, t1, t2 = 0.0;

  mpi();

  printf("\n\t\t-----------------------\n");

  t1 = clock();
  somme_matricielle();
  t2 = clock();

  r_sec = (t2 - t1) / t_sec;

  printf("\n>> Temps pour le programme sequentiel: %lf secconde(s) <<\n", r_sec);

  printf("\n");

  exit(EXIT_SUCCESS);

  
}

//************************************************************************************
//************************************************************************************

int somme_matricielle(){
  int maxt[5][5];

  int max1[5][5] = {{0, 1, 4, 7, 8}, 
                    {1, 2, 3, 6, 5}, 
                    {7, 8, 9, 4, 5}, 
                    {0, 8, 9, 6, 7}, 
                    {9, 8, 6, 7, 3}};

  int max2[5][5] = {{9, 8, 6, 7, 3},
                    {0, 8, 9, 6, 7},
                    {7, 8, 9, 4, 5},
                    {1, 2, 3, 6, 5},
                    {0, 1, 4, 7, 8}};

  //--------

  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 5; j++){
      maxt[i][j] = max1[i][j] + max2[i][j];
      //printf("Somme: %d", maxt[i][j]);
    }
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
  int world_rank;
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;
  MPI_Comm_size(communicator, &world_size);

  // Si je suis le processus 0(le root), j'envoie la data à tous les autres processus
  if (world_rank == root) {
    int i;
    for (i = 0; i < world_size; i++) {
      if (i != world_rank) {
        MPI_Send(data, count, datatype, i, 0, communicator);
      }
    }
  } else {
    // Si je ne suis pas le processus 0(le root), je recoit la data du processus 0(root)
    MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
  }
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

void mpi(){
  double t_sec = CLOCKS_PER_SEC;

  MPI_Init(NULL, NULL);

  double r_time, r_time1, r_time2 = 0.0;
  r_time1, r_time2, r_time = 0.0;

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  
  MPI_Barrier(MPI_COMM_WORLD);
  r_time1 = MPI_Wtime();

  int data;
  if (world_rank == 0) {
    data = somme_matricielle();
    printf("Process 0 broadcasting data %d\n", data);
    my_bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    my_bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Processus %d recoit la data %d du processus root\n", world_rank, data);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  r_time2 = MPI_Wtime();

  r_time = (r_time2 - r_time1) / t_sec;

  printf("\n>> Temps Pour le programme MPI: %lf seconde(s) <<\n", r_time);

  MPI_Finalize();
}