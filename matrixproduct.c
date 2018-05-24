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

/// --- >>>>> TEMPORARY FUNCTION FOR DEBUGS!!! <<<<< --- (begin)
void print_matrix(double **matr, int rows, int columns) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			printf("matr[%d][%d] = %lf\n", i, j, matr[i][j]);
		}
	}
}
/// --- >>>>> TEMPORARY FUNCTION FOR DEBUGS!!! <<<<< --- (end)

char input_validation(int argc, char **argv, FILE **path_matr_A, FILE **path_matr_B, FILE **path_matr_C) {
	/*Valids the input data and return the implementation chosen*/

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
	*path_matr_A = fopen(argv[2], "r");
	if ((*path_matr_A) == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix A.\n");
		exit(1);
	}

	// Thrid one
	*path_matr_B = fopen(argv[3], "r");
	if ((*path_matr_B) == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix B.\n");
		exit(1);
	}

	// Fourth one
	*path_matr_C = fopen(argv[4], "w");
	if ((*path_matr_C) == NULL) {
		fprintf(stderr, "ERROR: Invalid file to matrix C.\n");
		exit(1);
	}

	return implementation;
}

int file_to_matrix(FILE *path_matr, double **matr, char id) {
	/*Reads a file and get the matrix from it and return the number of rows or colmuns of it*/
	/*In case if it is matrix B, the function will build its transpose matrix*/

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
	if (id == 'B') columns = atoi(token); // Transpose
	else rows = atoi(token);
	token = strtok(NULL, space);
	if (id == 'B') rows = atoi(token); // Transpose
	else columns = atoi(token);

	// Creating matrix
	matr = (double**)emalloc(rows*sizeof(double*));
	for (int i = 0; i < rows; i++) matr[i] = (double*)emalloc(columns*sizeof(double));

	// Filling matrix
	int i_aux = rows + 1, j_aux = columns + 1; // In case, there is not position non-zero in matrix
	double value;

	// First position non-zero
	if (fgets(buffer, 128, path_matr) != NULL) {
		token = strtok(buffer, space);
		if (id == 'B') j_aux = atoi(token) - 1; // Transpose
		else i_aux = atoi(token) - 1;
		token = strtok(NULL, space);
		if (id == 'B') i_aux = atoi(token) - 1; // Transpose
		else j_aux = atoi(token) - 1;
		token = strtok(NULL, space);
		value = atof(token);
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (i != i_aux || j != j_aux) matr[i][j] = 0; // Not matrix position
			else {
				matr[i][j] = value;

				// Next position non-zero
				if (fgets(buffer, 128, path_matr) != NULL) {
					token = strtok(buffer, space);
					if (id == 'B') j_aux = atoi(token) - 1; // Transpose
					else i_aux = atoi(token) - 1;
					token = strtok(NULL, space);
					if (id == 'B') i_aux = atoi(token) - 1; // Transpose
					else j_aux = atoi(token) - 1;
					token = strtok(NULL, space);
					value = atof(token);
				}
			}
		}
	}

	// DEBUG (begin)
	printf("Matrix %c\n", id);
	print_matrix(matr, rows, columns);
	printf("\n");
	// DEBUG (end)

	return rows; // if (id == 'B') {rows = columns}
}

void matrix_to_file(double **matr, int rows, int columns, FILE **path_matr) {
	/*Reads a matrix and print it in a specif format in a file*/
	fprintf(*path_matr, "%d %d\n", rows, columns); // First line

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (matr[i][j] != 0) {
				fprintf(*path_matr, "%i %i %lf\n", i, j, matr[i][j]);
			}
		}
	}
}

int main(int argc, char **argv) {
	FILE *path_matr_A, *path_matr_B, *path_matr_C;
	double **matr_A, **matr_Bt, **matr_C;
	int rowsC, columnsC;
	char implementation;

	// Setup
	implementation = input_validation(argc, argv, &path_matr_A, &path_matr_B, &path_matr_C);
	rowsC = file_to_matrix(path_matr_A, matr_A, 'A');
	columnsC = file_to_matrix(path_matr_B, matr_Bt, 'B');

	// Matrix product
	if (implementation == 'p') {

	}
	else { // implementation == 'o'

	}

	// DEBUG (begin)
	matr_C = (double**)emalloc(rowsC*sizeof(double*));
	for (int i = 0; i < rowsC; i++) matr_C[i] = (double*)emalloc(columnsC*sizeof(double));
	for (int i = 0; i < rowsC; i++) {
		for (int j = 0; j < columnsC; j++) {
			if ((i + j)%3 == 2) matr_C[i][j] = ((i+j)*3)/2;
		}
	}
	printf("Matrix C\n");
	print_matrix(matr_C, rowsC, columnsC);
	// DEBUG (end)

	// Result
	matrix_to_file(matr_C, rowsC, columnsC, &path_matr_C);

	/*Closing*/
	fclose(path_matr_A);
	fclose(path_matr_B);
	fclose(path_matr_C);

	return 1;
}