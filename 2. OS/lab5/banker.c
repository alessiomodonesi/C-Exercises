/*
 * Banker's algorithm
 * adapted from https://www.geeksforgeeks.org/bankers-algorithm-in-operating-system-2/
 */

#include <stdio.h>


#define N 3 /* number of processes */
#define M 3 /* number of resources */


int main()  
{
    /*******************
     * DATA STRUCTURES *
     *******************/

    /* Allocation matrix */
    int alloc[N][M] = {{0, 1, 0},  /* P0 */
                       {1, 1, 0},  /* P1 */
                       {0, 0, 1}}; /* P2 */
  
    /* Max matrix */
    int max[N][M] = {{1, 1, 0},  /* P0 */
                     {1, 1, 1},  /* P1 */
                     {0, 1, 1}}; /* P2 */
  
    /* Available resources */
    int avail[M] = {0, 0, 0};

    /* Finish vector */
    int finish[N];
    for (int i= 0; i < N; i++)
    {  
        finish[i] = 0;
    }

    /* Needs matrix */
    int need[N][M];  
    for (int i = 0; i < N; i++)
    {  
        for (int j = 0; j < M; j++)
            need[i][j] = max[i][j] - alloc[i][j];
    }

    /* Auxiliary variables to keep track of finishing progress */
    int ans[N], ind = 0; 


    /*************
     * ALGORITHM *
     *************/

    int prev_ind;
    do
    {
        prev_ind = ind;

        /* Scan all processes and remaining needs */
        for (int i = 0; i < N; i++)
        {  
            if (finish[i] == 0)
            {  /* Check only unfinished processes */
                int waiting = 0;
                for (int j = 0; j < M; j++)
                {  
                    if (need[i][j] > avail[j])
                    { /* Process 'i' is waiting for more resources */
                        waiting = 1;
                        break;
                    }  
                }
                if (waiting == 0)  
                { /* Process 'i' has enough resources to finish */
                    for (int j = 0; j < M; j++) 
                        avail[j] += alloc[i][j]; /* release allocated resources */
                    finish[i] = 1;
                    ans[ind++] = i; /* keep track of the finishing order */
                }  
            }  
        }  
    }
    while (ind > prev_ind); /* continue as long as more processes finish */


    /*****************
     * PRINT RESULTS *
     *****************/

    for (int i = 0; i < N; i++)
    {
        if (finish[i] == 0)
        { /* At least one process cannot finish */
            printf("UNSAFE system\n");
            return 0; /* exit program */
        }  
    }

    /* All processes can finish */
    printf("SAFE sequence\n");
    for (int i = 0; i < N-1; i++)
        printf(" P%d ->", ans[i]);
    printf(" P%d\n", ans[N-1]);

    return 0; /* exit program */
}
