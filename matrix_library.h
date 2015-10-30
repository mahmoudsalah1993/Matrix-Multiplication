#ifndef MATRIX_LIBRARY

#define MATRIX_LIBRARY
#define TRUE 1
#define FALSE 0
#define bool int

//enumeration containing different error types
enum error_codes{
    FAILED_READ_MATRIX_A,
    FAILED_READ_MATRIX_B,
    FAILED_TO_CREATE_THREAD,
    INVALID_ARGUMENTS,
    FILE_A_NOT_FOUND,
    FILE_B_NOT_FOUND,
};

//struct holding data for thread which handles each element
typedef struct{
    int row;
    int column;
} thread_element_params;

void first_method();
void* calculate_element(void* threadarg);
void second_method();
void* calculate_row(void* threadarg);
void print_result();
bool read_matrix_A();
bool read_matrix_B();
void print_error(enum error_codes error);
#endif
