#ifndef MATRIX_LIBRARY

#define MATRIX_LIBRARY
#define TRUE 1
#define FALSE 0
#define bool int

enum error_codes{
    FAILED_READ_MATRIX_A,
    FAILED_READ_MATRIX_B,
    FAILED_TO_CREATE_THREAD,
};

typedef struct{
    int row;
    int column;
} thread_element_params;

void print_result(int method_id);
void first_method();
void* calculate_element(void* threadarg);
void second_method();
bool read_matrix_A();
bool read_matrix_B();
void print_error(enum error_codes error);
#endif
