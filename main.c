#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include "matrix_library.h"

//defines
#define TRUE 1
#define FALSE 0
#define bool int
#define MAX_COLS_COUNT 100
#define MAX_ROWS_COUNT 100
//maximum number of threads is max number of rows * cols
#define MAX_THREAD_NUMBER 10000

int A[MAX_ROWS_COUNT][MAX_COLS_COUNT];
int B[MAX_ROWS_COUNT][MAX_COLS_COUNT];
int C[MAX_ROWS_COUNT][MAX_COLS_COUNT];
int A_cols  = -1, B_cols = -1 , A_rows = -1 , B_rows = -1 , C_cols = -1 , C_rows = -1;
char* fileA;
char* fileB;
char* fileC;
enum error_codes error;
pthread_t threads[MAX_THREAD_NUMBER];

int main(int argc , char** argv)
{
    fileA = malloc(100);
    fileB = malloc(100);
    fileC = malloc(100);
    if(argc < 5)  //if no files defined
    {
        fileA = "a.txt";
        fileB = "b.txt";
        fileC = "c.out";
    }
    else
    {
        strcpy(fileA , argv[2]);
        strcpy(fileB , argv[3]);
        strcpy(fileC , argv[4]);
    }
    if(read_matrix_A()==FALSE)
    {
        error = FAILED_READ_MATRIX_A;
        print_error(error);
        exit(1);
    }
    else if(read_matrix_B()==FALSE)
    {
        error = FAILED_READ_MATRIX_B;
        print_error(error);
        exit(1);
    }
    else if(A_cols!= B_rows)
    {
        error = INVALID_ARGUMENTS;
        print_error(error);
        exit(1);
    }
    else
    {
        if(argv[1][0]=='0'){
            printf("using thread for each row method\n");
            second_method();
        }
        else {
            printf("Using thread for each element method\n");
            first_method();
        }
        return 0;
    }
    return 1;
}

//calculates C by having a thread calculate each element
void first_method()
{
    //each thread calculates an element , needs C_rows * C_cols threads
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    int i;
    int number_of_threads = C_cols * C_rows;
    printf("number of threads used %d\n" , number_of_threads);
    for(i = 0 ; i < number_of_threads ; i ++)
    {
        thread_element_params *thread_data = malloc(sizeof(thread_element_params));
        thread_data->row = i/C_cols;
        thread_data->column = i%C_cols;
        int state = pthread_create(&threads[i] , NULL , calculate_element , (void*) thread_data);
        if(state)
        {
            error = FAILED_TO_CREATE_THREAD;
            print_error(error);
            exit(-1);
        }
    }
    for(i = 0 ; i < number_of_threads ; i++)
    {
        pthread_join(threads[i],NULL);
    }
    print_result();
    gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    return;
}

//the starting function for thread per element method
void* calculate_element(void* threadarg)
{
    int value = 0;
    int i;
    thread_element_params *my_data;
    my_data = (thread_element_params *)threadarg;
    int row = my_data->row;
    int column = my_data->column;
    for(i = 0 ; i < A_cols ; i ++)
    {
        value += (A[row][i] * B[i][column]);
    }
    C[row][column] = value;
    pthread_exit(NULL);
}


//calculates C by having a thread calculate each row
void second_method()
{
    //each thread calculates a row , number of threads = C_rows
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    int number_of_threads = C_rows;
    int i;
    printf("number of threads used %d\n" , number_of_threads);
    for(i = 0 ; i < number_of_threads ; i ++)
    {
        int state;
        int thread_data = i;
        state = pthread_create(&threads[i] , NULL , calculate_row , (void*)thread_data);
        if(state)
        {
            error = FAILED_TO_CREATE_THREAD;
            print_error(error);
            exit(-1);
        }
    }
    for(i = 0 ; i < number_of_threads ; i++)
    {
        pthread_join(threads[i],NULL);
    }
    print_result();
    gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    return;
}


//the starting function for thread per row method
void* calculate_row(void* threadarg)
{
    int i , j;
    int row = threadarg;
    for(i = 0 ; i < C_cols ; i ++)
    {
        int value = 0;
        for(j = 0 ; j < C_rows ; j++)
        {
            value += (A[row][j] * B[j][i]);
        }
        C[row][i] = value;
    }
    pthread_exit(NULL);
}

//prints the matrix C to the file of C
void print_result()
{
    int i,j;
    freopen(fileC , "w" , stdout);
    printf("%d ___ %d\n" , C_rows , C_cols);
    for(i = 0 ; i  < C_rows ; i++)
    {
        for(j = 0 ; j < C_cols ; j ++)
        {
            printf("%d" , C[i][j]);
            if(j<C_cols-1)printf("\t");
        }
        printf("\n");
    }
    freopen("/dev/tty" , "a" , stdout);
}

//reads the A matrix from file of A
bool read_matrix_A()
{
    char* input_line = malloc(200);
    char* temp = malloc(200);
    if(!freopen(fileA , "r" , stdin))
    {
        error = FILE_A_NOT_FOUND;
        print_error(error);
        exit(1);
    }
    else
    {
        fgets(input_line , 200 , stdin);
        temp = strtok(input_line , "=\t ");
        while(temp!= NULL)
        {
            if(atoi(temp)!=0 || temp[0]=='0')
            {
                if(A_rows==-1)
                {
                    A_rows = atoi(temp);
                    C_rows = atoi(temp);
                }
                else A_cols = atoi(temp);
            }
            temp = strtok(NULL , "=\t ");
        }
        int i , j;
        for(i = 0 ; i < A_rows ; i++)
        {
            fgets(input_line , 200 , stdin);
            temp = strtok(input_line , " \t");
            for(j = 0 ; j  < A_cols ; j++)
            {
                if(atoi(temp)!=0 || temp[0]=='0')
                {
                    A[i][j] = atoi(temp);
                }
                else return FALSE;
                if(temp!=NULL)temp = strtok(NULL, "\t ");
            }
        }
        return TRUE;
    }
}

//reads the A matrix from file of A
bool read_matrix_B()
{
    char* input_line = malloc(200);
    char* temp = malloc(200);
    if(!freopen(fileB , "r" , stdin))
    {
        error = FILE_B_NOT_FOUND;
        print_error(error);
        exit(1);
    }
    else
    {
        fgets(input_line , 200 , stdin);
        temp = strtok(input_line , "=\t ");
        while(temp!= NULL)
        {
            if(atoi(temp)!=0 || temp[0]=='0')
            {
                if(B_rows==-1)
                    B_rows = atoi(temp);
                else
                {
                    B_cols = atoi(temp);
                    C_cols = atoi(temp);
                }
            }
            temp = strtok(NULL , "=\t ");
        }
        int i , j;
        for(i = 0 ; i < B_rows ; i++)
        {
            fgets(input_line , 200 , stdin);
            temp = strtok(input_line , " \t");
            for(j = 0 ; j  < B_cols ; j++)
            {
                if(atoi(temp)!=0 || temp[0]=='0')
                {
                    B[i][j] = atoi(temp);
                }
                else return FALSE;
                if(temp!=NULL)temp = strtok(NULL, "\t ");
            }
        }
        return TRUE;
    }
}

//prints appropriate message for each error
void print_error(enum error_codes error)
{
    switch(error)
    {
    case FAILED_READ_MATRIX_A:
            printf("failed to read matrix A\n");
        break;
    case FAILED_READ_MATRIX_B:
        printf("failed to read matrix B\n");
        break;
    case FAILED_TO_CREATE_THREAD:
        printf("failed to create thread\n");
        break;
    case INVALID_ARGUMENTS:
        printf("Can't multiply these matrices: columns of A must Equal rows of B\n");
        break;
    case FILE_A_NOT_FOUND:
        printf("The file for matrix A was not found\n");
        break;
    case FILE_B_NOT_FOUND:
        printf("The file for matrix B was not found\n");
        break;
    }
}
