#include <stdio.h>
#include <string.h>     // memcpy strcmp
#include <stdlib.h>     // abs
#include <sys/time.h>   // for gettimeofday()
#include <omp.h>        // openmp


int number_of_threads = 1;


// Convert BGR image to RGB, pixel type from int to float and rescale to 0.0-1.0
void process(int height, int width, unsigned char* data_in, float* data_out) {
    int j;
    omp_set_dynamic(0); 
    omp_set_num_threads(number_of_threads);
    #pragma omp parallel for private(j)     // To avoid race condition among the threads
    for (int i = 0; i < height; i++)
    for (j = 0; j < width; j++) {
        data_out[i*width*3+j*3+0] = (float) data_in[i*width*3+j*3+2] * (1 / 255.0);
        data_out[i*width*3+j*3+1] = (float) data_in[i*width*3+j*3+1] * (1 / 255.0);
        data_out[i*width*3+j*3+2] = (float) data_in[i*width*3+j*3+0] * (1 / 255.0);
    }
}


int test(int height, int width, unsigned char *data_in, float *data_out) {
    for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++) {
        if (abs(data_out[i*width*3+j*3+0] - data_in[i*width*3+j*3+0]) > 1e-2 ||
            abs(data_out[i*width*3+j*3+1] - data_in[i*width*3+j*3+1]) > 1e-2 || 
            abs(data_out[i*width*3+j*3+2] - data_in[i*width*3+j*3+2]) > 1e-2) 
        {
            printf("Pixel at [%d, %d] are not equal (%.3f, %d).\n", i, j, data_out[i*width*3+j*3+0], data_in[i*width*3+j*3+0]);
            return -1;
        }
    }
        
    printf("TEST SUCCESS!\n\n");
    return 0;
}



// Convert BRG image to RGB and pixel type from int to float
int main(int argc, char **argv)
{
    if (argc != 3 && argc !=4) {
        printf("Usage 1: program_name <height> <width> <number_of_threads>\n");
        return -1;
    }
    
    if (argc==4)
        number_of_threads = atoi(argv[3]);

    int height = atoi(argv[1]);
    int width = atoi(argv[2]);
    unsigned char *data_in = new unsigned char[height*width*3];
    float *data_out = new float[height*width*3];

    // Process
    struct timeval start, end;
    gettimeofday(&start, NULL);

    process(height, width, data_in, data_out);

    gettimeofday(&end, NULL);
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("\n%s %s %s %s %s \nProcessing time %ld seconds and %ld micros\n", 
           argv[0], argv[1], argv[2], argv[3], argv[4], seconds, micros);


    // Test
    printf("Pixel in %hhu %hhu %hhu ...\n", data_in[0], data_in[1], data_in[2]);
    printf("Scaled %.3f %.3f %.3f ...\n", data_in[0] / 255.0, data_in[1] / 255.0, data_in[2] / 255.0);
    printf("Pixel out %.3f %.3f %.3f ...\n", data_out[0], data_out[1], data_out[2]);

    if (argc == 7) 
        return test(height, width, data_in, data_out);
    else
        printf("NO TEST!\n");


    // Free dynamic allocation
    delete [] data_in; 
    delete [] data_out;   

    return 0;
}