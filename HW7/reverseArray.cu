//  reverseArray.cu
//  Chenfeng Hao
//  HW 7
//  Using a second array

#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;

#define ARRAY_SIZE 20
#define BLOCK_SIZE 4


__global__ void cu_reverseArray(int arr_in[], int arr_out[]) {
    // compute thread index
    // use it to retrieve block and thread IDs
    int x = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    arr_out[x] = arr_in[ARRAY_SIZE - x - 1];
}


int main(int argc, char *argv[]) {
    // declare arrays and initialize to 0
    int arr_in[ARRAY_SIZE];
    int arr_out[ARRAY_SIZE];

//    srand(0);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr_in[i] = i;
    }

    // print initial arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        cout << arr_in[i] << ' ';
    }
    cout << endl << endl;

    int *arr_in_d;
    int *arr_out_d;
    cudaError_t result;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // allocate space on the device
    result = cudaMalloc((void **) &arr_in_d, sizeof(int) * ARRAY_SIZE);
    result = cudaMalloc((void **) &arr_out_d, sizeof(int) * ARRAY_SIZE);
    if (result != cudaSuccess) {
        cerr << "cudaMalloc (thread) failed." << endl;
        exit(1);
    }

    // copy arrays from host to device
    result = cudaMemcpy(arr_in_d, arr_in, sizeof(int) * ARRAY_SIZE, cudaMemcpyHostToDevice);
    result = cudaMemcpy(arr_out_d, arr_out, sizeof(int) * ARRAY_SIZE, cudaMemcpyHostToDevice);
    if (result != cudaSuccess) {
        cerr << "cudaMemcpy host->dev failed." << endl;
        exit(1);
    }

    // set execution configuration
    dim3 dimblock(BLOCK_SIZE);
    dim3 dimgrid(ARRAY_SIZE / BLOCK_SIZE);

    // call the kernel function
    cudaEventRecord(start);
    cu_reverseArray <<<dimgrid, dimblock>>>(arr_in_d, arr_out_d);
    cudaEventRecord(stop);

    // transfer results back to host
    result = cudaMemcpy(arr_out, arr_out_d, sizeof(int) * ARRAY_SIZE, cudaMemcpyDeviceToHost);
    if (result != cudaSuccess) {
        cerr << "cudaMemcpy host <- dev (thread) failed." << endl;
        exit(1);
    }

    // release the memory on the GPU
    result = cudaFree(arr_out_d);
    result = cudaFree(arr_in_d);
    if (result != cudaSuccess) {
        cerr << "cudaFree (thread) failed." << endl;
        exit(1);
    }

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    // re-print arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        cout << arr_out[i] << ' ';
    }

    cout << endl << endl;

    cout << "Reversing an array of size " << ARRAY_SIZE << " took " << milliseconds * 1000 << " microseconds." << endl;

    return 0;
}