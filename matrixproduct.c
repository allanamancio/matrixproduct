#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Struct to matrix
typedef struct { 
	int row;
	int col;
	double **tab;
} matr;

void *emalloc(size_t size) {
	void *memory = malloc(size);

	if (!memory) {
		fprintf(stderr, "ERROR: Failed to malloc.\n");
		exit(1);
	}

	return memory;
}

/// --- >>>>> TEMPORARY FUNCTION FOR DEBUGS!!! <<<<< --- (begin)
void print_matrix(matr *m) {
	for (int i = 0; i < m->row; i++) {
		for (int j = 0; j < m->col; j++) {
			printf("matr[%d][%d] = %lf\n", i, j, m->tab[i][j]);
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

void file_to_matrix(FILE *path_matr, matr *m, char id) {
	/*Reads a file and get the matrix from it and return the number of rows or colmuns of it*/
	/*In case if it is matrix B, the function will build its transpose matrix*/

	// Variables
	int row;
	int col;
	double **matrix;

	const char space[2] = " ";
	char *token = emalloc(64 * sizeof(char));
	char *buffer = emalloc(128 * sizeof(char));

	// Number of rows and columns
	if (fgets(buffer, 128, path_matr) == NULL) {
		fprintf(stderr, "ERROR: Empty file.\n");
		exit(1);
	}

	token = strtok(buffer, space);
	if (id == 'B') col = atoi(token); // Transpose
	else row = atoi(token);
	token = strtok(NULL, space);
	if (id == 'B') row = atoi(token); // Transpose
	else col = atoi(token);

	// Creating matrix
	matrix = (double**)emalloc(row * sizeof(double*));
	for (int i = 0; i < row; i++) matrix[i] = (double*)emalloc(col * sizeof(double));

	// Filling matrix
	int i_aux = row + 1, j_aux = col + 1; // In case, there is not position non-zero in matrix
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

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (i != i_aux || j != j_aux) matrix[i][j] = 0; // Not matrix position
			else {
				matrix[i][j] = value;

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

	m->row = row;
	m->col = col;
	m->tab = matrix;

	// DEBUG (begin)
	printf("Matrix %c\n", id);
	print_matrix(m);
	printf("\n");
	// DEBUG (end)

}

void matrix_to_file(matr *m, FILE **path_matr) {
	/*Reads a matrix and print it in a specif format in a file*/
	fprintf(*path_matr, "%d %d\n", m->row, m->col); // First line

	for (int i = 0; i < m->row; i++) {
		for (int j = 0; j < m->col; j++) {
			if (m->tab[i][j] != 0) {
				fprintf(*path_matr, "%i %i %lf\n", i+1, j+1, m->tab[i][j]);
			}
		}
	}
}

void *parallel_product(void *arg) {
	printf("EXECUTEI QUALQUER PORRA\n"); // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
}

int main(int argc, char **argv) {
	FILE *path_matr_A, *path_matr_B, *path_matr_C;
	matr matr_A, matr_Bt, matr_C;
	char implementation;

	// Setup
	implementation = input_validation(argc, argv, &path_matr_A, &path_matr_B, &path_matr_C);
	file_to_matrix(path_matr_A, &matr_A, 'A');
	file_to_matrix(path_matr_B, &matr_Bt, 'B');

	// Matrix product
	if (implementation == 'p') {
		int number_threads = sysconf(_SC_NPROCESSORS_ONLN);
		pthread_t *id = emalloc(number_threads * sizeof(pthread_t));

		for (int i = 0; i < number_threads; i++) {
			if (pthread_create(&id[i], NULL, parallel_product, NULL)) {
				fprintf(stderr, "ERROR: Thread not created.\n");
				exit(1);
			}
			else printf("CREATED, YEAH %d\n", id[i]); // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
		}

		for (int i = 0; i < number_threads; i++) {
			if (pthread_join(id[i], NULL)) {
				fprintf(stderr, "ERROR: Thread not joined.\n");
				exit(1);
			}
			else printf("JOINED, YEAH %d\n", id[i]); // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
		}

	}
	else { // implementation == 'o'

	}

	// DEBUG (begin)
	matr_C.row = matr_A.row;
	matr_C.col = matr_Bt.row;
	matr_C.tab = (double**)emalloc(matr_C.row * sizeof(double*));

	for (int i = 0; i < matr_C.row; i++) matr_C.tab[i] = (double*)emalloc(matr_C.col * sizeof(double));
	for (int i = 0; i < matr_C.row; i++) {
		for (int j = 0; j < matr_C.col; j++) {
			if ((i + j)%3 == 2) matr_C.tab[i][j] = ((i+j)*3)/2;
		}
	}
	printf("Matrix C\n");
	print_matrix(&matr_C);
	// DEBUG (end)

	// Result
	matrix_to_file(&matr_C, &path_matr_C);

	/*Closing*/
	fclose(path_matr_A);
	fclose(path_matr_B);
	fclose(path_matr_C);

	return 1;
}