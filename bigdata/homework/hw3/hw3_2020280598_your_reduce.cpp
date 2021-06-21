#include "your_reduce.h"
#include <math.h> 

// You may add your functions and variables here

void YOUR_Reduce(const int* sendbuf, int* recvbuf, int count) {
    /*
        Modify the code here.
        Your implementation should have the same result as this MPI_Reduce
        call. However, you MUST NOT use MPI_Reduce (or like) for your hand-in
        version. Instead, you should use MPI_Send and MPI_Recv (or like). See
        the homework instructions for more information.
    */
    //MPI_Reduce(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	int rank, size;
	int i, j, k;
	int tag = 0;

	int *tmpbuf;  
    // initialize
	tmpbuf = new int [count];
	for (i=0; i<count; i++) {
		recvbuf[i]=sendbuf[i];
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// binary tree implementation
	for (i = 0; i < log2(size); i++) {
		for (j = 0; j < size; j += 1 << (i + 1)) {
			const int receiver = j;
			const int sender = j + (1 << i);
			if (rank == receiver) {
				MPI_Recv(tmpbuf, count, MPI_INT, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for(k=0; k<count; k++) {
					recvbuf[k]+=tmpbuf[k];
				}
			}
			else if (rank == sender) {
				MPI_Send(recvbuf, count, MPI_INT, receiver, tag, MPI_COMM_WORLD);
			}
		}
	}

    /*
        You may assume:
        - Data type is always `int` (MPI_INT).
        - Operation is always MPI_SUM.
        - Process to hold final results is always process 0.
        - Number of processes is 2, 4, or 8.
        - Number of elements (`count`) is 1, 16, 256, 4096, 65536, 1048576,
          16777216, or 268435456.
        For other cases, your code is allowed to produce wrong results or even
        crash. It is totally fine.
    */
}