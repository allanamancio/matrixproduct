#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>


// Struct to matrix
typedef struct { 
	int row;
	int col;
	double **tab;
} matr;

// Global variables
matr matr_A, matr_Bt, matr_C;
double thread_space = 0;

int number_threads;

void *emalloc(size_t size) {
	void *memory = malloc(size);

	if (!memory) {
		fprintf(stderr, "ERROR: Failed to malloc.\n");
		exit(1);
	}

	return memory;
}

void print_matrix(matr *m) {
	for (int i = 0; i < m->row; i++) {
		for (int j = 0; j < m->col; j++) {
			printf("matr[%d][%d] = %lf\n", i, j, m->tab[i][j]);
		}
	}
}

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
	int row, col;
	double **matrix;

	// Number of rows and columns
	if (id == 'B') fscanf(path_matr, "%d%d", &col, &row);
	else fscanf(path_matr, "%d%d", &row, &col);

	// Creating matrix
	matrix = (double**)emalloc(row * sizeof(double*));
	for (int i = 0; i < row; i++) matrix[i] = (double*)emalloc(col * sizeof(double));

	// Filling matrix
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			matrix[i][j] = 0;
		}
	}

	double value;
	int i,j;
	while (fscanf(path_matr, "%d%d%lf", &i, &j, &value) != EOF) {
		if (id == 'B') matrix[j-1][i-1] = value;
		else matrix[i-1][j-1] = value;
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

void *parallel_product(void *position) {
	int my_position = *((int *) position);
	int begin, limit;
	while (thread_space == 0) continue; // Waits the command to begin

	int chao = matr_A.row / number_threads;
	int teto = (matr_A.row + number_threads - 1) / number_threads;

	int lim = matr_A.row - number_threads;

	if (my_position < lim) {
		begin = my_position * teto;
		limit = my_position * teto + teto;
	} else {
		begin = lim * teto + (my_position - lim) * chao;
		limit = lim * teto + (my_position - lim) * chao + chao;
	}
	
	

	for (int i = begin; i < limit; i++) {
		for (int j = 0; j < matr_Bt.row; j++) {
			matr_C.tab[i][j] = 0;
			for (int k = 0; k < matr_Bt.col; k++) {
				matr_C.tab[i][j] = matr_C.tab[i][j] + matr_A.tab[i][k]*matr_Bt.tab[j][k];
			}
		}
	}
	// printf("%f\n", matr_C.tab[0][0]);

}

int main(int argc, char **argv) {
	FILE *path_matr_A, *path_matr_B, *path_matr_C;
	char implementation;

	// Setup
	implementation = input_validation(argc, argv, &path_matr_A, &path_matr_B, &path_matr_C);
	file_to_matrix(path_matr_A, &matr_A, 'A');
	file_to_matrix(path_matr_B, &matr_Bt, 'B');
	
	matr_C.row = matr_A.row;
	matr_C.col = matr_Bt.row;
	matr_C.tab = (double**)emalloc(matr_C.row * sizeof(double*));
	for (int i = 0; i < matr_C.row; i++) matr_C.tab[i] = (double*)emalloc(matr_C.col * sizeof(double));

	// Matrix product
	if (implementation == 'p') {
		number_threads = sysconf(_SC_NPROCESSORS_ONLN);
		if (matr_A.row < number_threads) number_threads = matr_A.row; // Minimum between number of lines and threads
		pthread_t *id = emalloc(number_threads * sizeof(pthread_t));

		thread_space = matr_A.row/number_threads;
		for (int i = 0; i < number_threads; i++) {
			int* pos = emalloc(sizeof(int));
			*pos = i;
			if (pthread_create(&id[i], NULL, parallel_product, (void *) pos)) {
				fprintf(stderr, "ERROR: Thread not created.\n");
				exit(1);
			}
		}

		for (int i = 0; i < number_threads; i++) {
			if (pthread_join(id[i], NULL)) {
				fprintf(stderr, "ERROR: Thread not joined.\n");
				exit(1);
			}
		}
	}
	else { // implementation == 'o'
		#pragma omp parallel for
		for (int i=0; i<matr_A.row; i++) {
			for (int j=0; j<matr_Bt.row; j++) {
				for (int k=0; k<matr_A.col; k++) {
					matr_C.tab[i][j] += (matr_A.tab[i][k] * matr_Bt.tab[j][k]);
				}
			}
		}
	}
	
	// printf("q");
	// Result
	matrix_to_file(&matr_C, &path_matr_C);
	// printf("qa");
	// DEBUG (begin)
	printf("Matrix C\n");
	print_matrix(&matr_C);
	// DEBUG (end)

	/*Closing*/
	fclose(path_matr_A);
	fclose(path_matr_B);
	fclose(path_matr_C);

	return 1;
}
