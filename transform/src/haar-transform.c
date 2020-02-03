// Copyright (c) 2020 Ovidiu Gheorghies
// Licensed under MIT License.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

const int ERROR_CODE_CLI_ARGUMENTS = 100;
const int ERROR_CODE_ALLOCATION    = 101;
const int ERROR_CODE_REALLOCATION  = 102;
const int ERROR_CODE_WRONG_SIZE    = 103;

const int MIN_BUFFER_SIZE_BYTES = 512 * sizeof(double);

struct data_t {
    double* buffer;
    int length;     // count of useful double values in buffer
    int allocated;  // count of allocated double values in buffer
};

int  result_processor_called = 0;
void result_processor_stdout(double result); 
void result_processor_stdout_end();

int input_initialize   (struct data_t* input);
int input_read         (struct data_t* input);
int input_validate     (struct data_t* input);
int input_generate_haar(struct data_t* input, void (*result_processor)(double));

int parse_arguments(int argc, const char** argv);

int error_code = 0;
int error_register(int error_code, const char* format, ...);
int error_exit();

int main(int argc, const char** argv) 
{
    parse_arguments(argc, argv) || error_exit();

    struct data_t input;

    input_initialize(&input) || error_exit();
    input_read      (&input) || error_exit();
    input_validate  (&input) || error_exit();

    input_generate_haar(&input, result_processor_stdout) || error_exit();

    result_processor_stdout_end();
}

int input_initialize(struct data_t* input)
{
    input->buffer = (double*) malloc(MIN_BUFFER_SIZE_BYTES);
    input->length = 0;
    input->allocated = MIN_BUFFER_SIZE_BYTES / sizeof(double);

    if (!input->buffer) {
        return error_register(ERROR_CODE_ALLOCATION, "Allocation error, bytes %d.", MIN_BUFFER_SIZE_BYTES);
    }

    return 1;
}

int input_read(struct data_t* input)
{
    double value;
    while(scanf("%lf", &value) == 1) {
        if (input->length == input->allocated) {
            int allocated_new = 2 * input->allocated;

            input->buffer = realloc(input->buffer, allocated_new * sizeof(double));
            input->allocated = allocated_new;

            if (!input->buffer) {
                return error_register(ERROR_CODE_ALLOCATION, "Reallocation error, bytes %d.", allocated_new * sizeof(double));
            }
        }

        input->buffer[input->length] = value;
        input->length ++;
    }

    return 1;
}

int input_validate(struct data_t* input) 
{
    int n = input->length;

    if (n == 0) {
        return error_register(ERROR_CODE_WRONG_SIZE, "Input size not a power of 2, actual value %d.", input->length);
    }

    // Check if the length of the input is a power of 2 by iteratively looking at the least significant bit and right-shifting until the end.
    while (n) {
        if (n == 1) {
            return 1;
        }
        if (n & 1) {
            return error_register(ERROR_CODE_WRONG_SIZE, "Input size not a power of 2, actual value %d.", input->length);
        }
        n >>= 1;
    }

    return 1;
}

void result_processor_stdout(double result) 
{
    fprintf(stdout, "%s%lf", result_processor_called ? " ": "", result);
    result_processor_called = 1;
}

void result_processor_stdout_end() {
    fprintf(stdout, "\n");
}


int input_generate_haar(struct data_t* input, void (*result_processor)(double))
{
    struct data_t aux;
    aux.length = aux.allocated = input->length;
    aux.buffer = malloc(input->length * sizeof(double));

    if (!aux.buffer) {
        return error_register(ERROR_CODE_ALLOCATION, "Allocation error (buffer), bytes %d.", aux.allocated * sizeof(double));
    }

    struct data_t* a = input;
    struct data_t* b = &aux;

    double sqrt_2 = sqrt(2);
    int div = 1; // or 2, after the first iteration.

    // With the exception of the first iteration, the size of the computed array b is half that of the source array a.
    // See https://cs.nyu.edu/media/publications/zhu_yunyue.pdf page 59.
    // a0 a1 a2 a3 a4 a5 a6 a7
    // \   /\    /\    /\    /
    //   b0   b1    b2    b3

    while (b->length >= 2) {
        for(int i = 0; i < b->length / 2; i++) {
            b->buffer[i] = ( a->buffer[2 * i] + a->buffer[2 * i + 1] ) / sqrt_2;
        }

        for(int i = b->length / 2; i < b->length; i++) {
            int i_offset = i - b->length / 2;

            b->buffer[i] = ( a->buffer[2 * i_offset] - a->buffer[2 * i_offset + 1] ) / sqrt_2;

            result_processor(b->buffer[i]);
        }

        div = 2; // after the first iteration, halve the buffer on the next level
        a->length = b->length / div;

        struct data_t* swp = b;
        b = a;
        a = swp;
    }

    result_processor(a->buffer[0]);

    return 1;
}

int parse_arguments(int argc, const char** argv)
{
    // TODO Add --help, number formatting 
    return 1;
}

int error_register(int error_code, const char* format, ...) 
{
    error_code = error_code;
    fprintf(stderr, "HARR-ERR-%03d: ", error_code);

    va_list argp;
    va_start(argp, format);
    vfprintf(stderr, format, argp);
    va_end(argp);

    fprintf(stderr, "\n");

    return 0;
}

int error_exit()
{
    exit(error_code);
}
