#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

const int arr[] = {1000, 10000, 13000};
const int N = std::size(arr);
const int cpuCores = 10;
const int coresArray[] = {cpuCores / 2, cpuCores, cpuCores * 2, cpuCores * 4, cpuCores * 8, cpuCores * 16};
const int MAX_SIZE = arr[N-1];

static int A[MAX_SIZE][MAX_SIZE];
static int B[MAX_SIZE][MAX_SIZE];
static int C[MAX_SIZE][MAX_SIZE];

void randomMatrix(int A[][MAX_SIZE], int B[][MAX_SIZE], int size, int seed) {
    std::srand(seed);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            A[i][j] = std::rand() % size;
            B[i][j] = std::rand() % size;
        }
    }
}

void printSystemInfo() {
    unsigned int numCPU = std::thread::hardware_concurrency();
    std::cout << "Number of CPU cores: " << numCPU << std::endl;
}

void matrixAdd(int A[][MAX_SIZE], int B[][MAX_SIZE], int C[][MAX_SIZE], int startRow, int endRow, int size) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < size; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}
int main() {
    int seed = static_cast<int>(std::time(nullptr));
    printSystemInfo();
    for (int i = 0; i < N; i++) {
        int size = arr[i];
        randomMatrix(A, B, size, seed);

        for (int numThreads : coresArray) {
            auto start = high_resolution_clock::now();
            std::thread myThreads[numThreads];
            int rowsPerThread = size / numThreads;
            int remainder = size % numThreads;

            for (int k = 0; k < numThreads; k++) {
                int startRow = k * rowsPerThread + std::min(k, remainder);
                int endRow = startRow + rowsPerThread + (k < remainder ? 1 : 0);
                myThreads[k] = std::thread(matrixAdd, std::cref(A), std::cref(B), std::ref(C), startRow, endRow, size);
            }

            for (int t = 0; t < numThreads; t++) {
                if (myThreads[t].joinable()) {
                    myThreads[t].join();
                }
            }

            auto end = high_resolution_clock::now();
            auto elapsed = duration_cast<nanoseconds>(end - start).count() * 1e-9;
            std::cout << "Array size: " << size << " Threads: " << numThreads << " Time: " << elapsed << " seconds" << std::endl;
        }
    }

    return 0;
}
