#include <stdlib.h>

#define ROWS 1024
#define COLS 1024

int main(void) {

	int data[ROWS][COLS];

	int i, j;

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
			data[i][j] = rand();

	return 0;
}
