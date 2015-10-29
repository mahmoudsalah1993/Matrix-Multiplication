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
#define MAX_THREAD_NUMBER 1000

int A[100][100];
int B[100][100];
int C[100][100];
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
    if(argc < 4){ //no files defined
        fileA = "a.txt";
        fileB = "b.txt";
        fileC = "c.out";
    }
    else{
        strcpy(fileA , argv[1]);
        strcpy(fileB , argv[2]);
        strcpy(fileC , argv[3]);
    }
    if(read_matrix_A()==FALSE){
        error = FAILED_READ_MATRIX_A;
        print_error(error);
        exit(1);
    }
    else if(read_matrix_B()==FALSE){
        error = FAILED_READ_MATRIX_B;
        print_error(error);
        exit(1);
    }
    else{
        printf("Using first method\n");
        first_method();
        printf("*************************\nUsing second method\n");
        second_method();
        return 0;
    }
    return 1;
}

void first_method(){
    //each thread calculates an element , needs C_rows * C_cols threads
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    int i;
    int number_of_threads = C_cols * C_rows;
    printf("number of threads used %d\n" , number_of_threads);
    for(i = 0 ; i < number_of_threads ; i ++){
        thread_element_params *thread_data = malloc(sizeof(thread_element_params));
        thread_data->row = i/C_cols;
        thread_data->column = i%C_cols;
        int state = pthread_create(&threads[i] , NULL , calculate_element , (void*) thread_data);
        if(state){
            error = FAILED_TO_CREATE_THREAD;
            print_error(error);
            exit(-1);
        }
    }
    for(i = 0 ; i < number_of_threads ; i++){
        pthread_join(threads[i],NULL);
    }
    print_result(0);
    gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    return;
}

void* calculate_element(void* threadarg){
    int value = 0;
    int i;
    thread_element_params *my_data;
    my_data = (thread_element_params *)threadarg;
    int row = my_data->row;
    int column = my_data->column;
    for(i = 0 ; i < A_cols ; i ++){
        value += (A[row][i] * B[i][column]);
    }
    C[row][column] = value;
    pthread_exit(NULL);
}

void second_method(){
    //each thread calculates a row , number of threads = C_rows
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    int number_of_threads = C_cols;
    int i;
    printf("number of threads used %d\n" , number_of_threads);
    for(i = 0 ; i < number_of_threads ; i ++){

    }
    for(i = 0 ; i < number_of_threads ; i++){
        pthread_join(threads[i],NULL);
    }
    print_result(1);
    gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    return;
}

void* calculate_row(){

    pthread_exit(NULL);
}

void print_result(int method_id){
    int i,j;
    if(method_id==0){
        freopen(fileC , "w" , stdout);
    }
    else {
        freopen(fileC , "a" , stdout);
        printf("____________________________________\n");
    }
    printf("%d ___ %d\n" , C_rows , C_cols);
    for(i = 0 ;i  < C_rows ; i++){
        for(j = 0 ; j < C_cols ; j ++){
            printf("%d" , C[i][j]);
            if(j<C_cols-1)printf("\t");
        }
        printf("\n");
    }
    freopen("/dev/tty" , "a" , stdout);
}

bool read_matrix_A(){
    char* input_line = malloc(200);
    char* temp = malloc(200);
    freopen(fileA , "r" , stdin);
    fgets(input_line , 200 , stdin);
    temp = strtok(input_line , "=\t ");
    while(temp!= NULL){
        if(atoi(temp)!=0 || temp[0]=='0'){
            if(A_rows==-1){
                A_rows = atoi(temp);
                C_rows = atoi(temp);
            }
            else A_cols = atoi(temp);
        }
        temp = strtok(NULL , "=\t ");
    }
    int i , j;
    for(i = 0 ; i < A_rows ; i++){
        fgets(input_line , 200 , stdin);
        temp = strtok(input_line , " \t");
        for(j = 0 ; j  < A_cols ; j++){
            if(atoi(temp)!=0 || temp[0]=='0'){
                A[i][j] = atoi(temp);
            }
            else return FALSE;
            if(temp!=NULL)temp = strtok(NULL, "\t ");
        }
    }
    return TRUE;
}

bool read_matrix_B(){
    char* input_line = malloc(200);
    char* temp = malloc(200);
    freopen(fileB , "r" , stdin);
    fgets(input_line , 200 , stdin);
    temp = strtok(input_line , "=\t ");
    while(temp!= NULL){
        if(atoi(temp)!=0 || temp[0]=='0'){
            if(B_rows==-1)
                B_rows = atoi(temp);
            else{
                B_cols = atoi(temp);
                C_cols = atoi(temp);
            }
        }
        temp = strtok(NULL , "=\t ");
    }
    int i , j;
    for(i = 0 ; i < B_rows ; i++){
        fgets(input_line , 200 , stdin);
        temp = strtok(input_line , " \t");
        for(j = 0 ; j  < B_cols ; j++){
            if(atoi(temp)!=0 || temp[0]=='0'){
                B[i][j] = atoi(temp);
            }
            else return FALSE;
            if(temp!=NULL)temp = strtok(NULL, "\t ");
        }
    }
    return TRUE;
}

void print_error(enum error_codes error){
    switch(error){
        case FAILED_READ_MATRIX_A:
            printf("failed to read matrix A\n");
            break;
        case FAILED_READ_MATRIX_B:
            printf("failed to read matrix B\n");
            break;
        case FAILED_TO_CREATE_THREAD:
            printf("failed to create thread\n");
            break;
    }
}
