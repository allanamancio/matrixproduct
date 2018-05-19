#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *emalloc(size_t size) {
	void *memory = malloc(size);

	if (!memory) {
		fprintf(stderr, "ERROR: Failed to malloc.\n");
		exit(1);
	}

	return memory;
}

char input_validation(int argc, char **argv, FILE *path_matr_A, FILE *path_matr_B, FILE *path_matr_C) {
	char implementation;

	// Checking quantity of arguments
	if (argc != 5) {
		fprintf(stderr, "ERROR: Invalid number of arguments.\n");
		exit(1);
	}

	// Checking values of the arguments
	// First one
	if (strcmp(argv[1], "o") == 0) implementation = 'o';
	else if (strcmp(argv[1], "p") == 0) implementation = 'p';
	else {
		fprintf(stderr, "ERROR: Invalid argument for implementation.\n");
		exit(1);
	}

	// Second one
	path_matr_A = fopen(argv[2], "r");
	if (path_matr_A == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix A.\n");
		exit(1);
	}

	// Thrid one
	path_matr_B = fopen(argv[3], "r");
	if (path_matr_B == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix B.\n");
		exit(1);
	}

	// Fourth one
	path_matr_C = fopen(argv[4], "w");
	if (path_matr_C == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix C.\n");
		exit(1);
	}
}

void file_to_matrix(FILE *path_matr, double **matr) {
	/*Reads a file and get the matrix from it*/

	// Variables
	int rows;
	int columns;
	const char space[2] = " ";
	char *token = emalloc(64*sizeof(char));
	char *buffer = emalloc(128*sizeof(char));

	// Number of rows and columns
	if (fgets(buffer, 128, path_matr) == NULL) {
		fprintf(stderr, "ERROR: Empty file.\n");
		exit(1);
	}

	token = strtok(buffer, space);
	rows = atoi(token);
	token = strtok(buffer, space);
	columns = atoi(token);
	
	// Creating matrix
	matr = (double**)emalloc(rows*sizeof(double*));
	for (int i = 0; i < rows; i++) matr[i] = (double*)emalloc(columns*sizeof(double));

	// Filling matrix
	int i_aux, j_aux, value;
	i_aux = 0;
	j_aux = 0;

	// First position non-zero
	if (fgets(buffer, 128, path_matr) != NULL) {
		token = strtok(buffer, space);
		i_aux = atoi(token);
		token = strtok(buffer, space);
		j_aux = atoi(token);
		token = strtok(buffer, space);
		value = atoi(token);
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (i_aux != i || j_aux != j) matr[i][j] = 0;
			else {
				matr[i][j] = value;

				// Next position non-zero
				if (fgets(buffer, 128, path_matr) != NULL) {
					token = strtok(buffer, space);
					i_aux = atoi(token);
					token = strtok(buffer, space);
					j_aux = atoi(token);
					token = strtok(buffer, space);
					value = atoi(token);
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	FILE *path_matr_A, *path_matr_B, *path_matr_C;
	double **matr_A, **matr_B, **matr_C;
	char implementation;

	/*Setup*/
	implementation = input_validation(argc, argv, path_matr_A, path_matr_B, path_matr_C);
	file_to_matrix(path_matr_A, matr_A);
	// file_to_matrix(path_matr_B, matr_B);

	/*Closing*/

	// fclose(path_matr_A);
	// fclose(path_matr_B);
	// fclose(path_matr_C);

	return 1;
}