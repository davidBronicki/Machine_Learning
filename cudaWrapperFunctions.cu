#include "global_include.h"
#include "cudaWrapperFunctions.h"
#include "cuda_runtime.h"
#include <iostream>
#include <math.h>

using namespace std;

#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "cuda functions: " << input << std::endl
#endif

bool allocateMemory(void** pointer, int size){
	cudaError_t cudaStatus = cudaMalloc(pointer, size);
	if (cudaStatus != cudaSuccess)
	{
		print("allocate memory: " << cudaGetErrorString(cudaStatus));
		return false;
	}
	return true;
}
bool freeMemory(void* pointer){
	cudaError_t cudaStatus = cudaFree(pointer);
	if (cudaStatus != cudaSuccess){
		print("free memory: " << cudaGetErrorString(cudaStatus));
		return false;
	}
	return true;
}

bool updateCudaDevice(void* pointerHost, void* pointerDevice, int size){
	cudaError_t cudaStatus =
		// cudaMemcpy(pointerHost, pointerDevice, size, cudaMemcpyDeviceToHost);
		cudaMemcpy(pointerDevice, pointerHost, size, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
	{
		print("update device: " << cudaGetErrorString(cudaStatus));
		return false;
	}
	return true;
}
bool updateThisHost(void* pointerHost, void* pointerDevice, int size){
	
	cudaError_t cudaStatus =
		cudaMemcpy(pointerHost, pointerDevice, size, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
	{
		print("update host: " << cudaGetErrorString(cudaStatus));
		return false;
	}
	return true;
}


#define syncThreads __syncthreads()
#define syncDevice cudaDeviceSynchronize()
#define THREAD_COUNT 128
#define LINEAR_GRID_SIZE(count) ((count - 1) / THREAD_COUNT + 1)

__device__ float squash(float input){
	return 2 / (1 + exp(-input)) - 1;
}

__device__ float invSquash(float input){
	return 2 / (1 - input * input);
}

__global__ void linearKernel(float* inputData, int inputLength,
	float* outputData, int outputLength, float* weights,
	float* biases){
	
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if (index < outputLength){
		outputData[index] = biases[index];
		for (int i = 0; i < inputLength; i++){
			outputData[index] += inputData[i] * weights[i * outputLength + index];
		}
		outputData[index] = squash(outputData[index]);
	}
}

__global__ void linearBackKernel(float* inputData, int inputLength,
	float* outputData, int outputLength, float* weights,
	float* biases, float* inputBuffer, float epsilon){

	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if (index < inputLength){
		inputBuffer[index] = 0;
		for (int i = 0; i < outputLength; i++){
			inputBuffer[index] += outputData[i] * weights[index * outputLength + i] *
				invSquash(inputData[index]);
		}
	}
	syncThreads;
	if (index < outputLength){
		biases[index] += epsilon * outputData[index];
	}
	if (index < inputLength){
		for (int i = 0; i < outputLength; i++){
			weights[index * outputLength + i] += epsilon * outputData[i] * inputData[index];
		}
	}
	syncThreads;
	if (index < inputLength){
		for (int i = 0; i < outputLength; i++){
			inputData[index] = inputBuffer[index];
		}
	}
}

void runLinearKernel(dmem<float> from, dmem<float> to,
	dmem<float> weights, dmem<float> biases){

	dim3 grid(THREAD_COUNT);
	dim3 block(LINEAR_GRID_SIZE(from.size()));
	linearKernel<<<grid, block>>>(
		from, from.size(),
		to, to.size(),
		weights, biases);
	syncDevice;
}

void runLinearBackKernel(dmem<float> from, dmem<float> to,
	dmem<float> weights, dmem<float> biases, float epsilon){

	dmem<float> buffer(from.size());
	buffer.loadMemory();
	dim3 grid(THREAD_COUNT);
	dim3 block(LINEAR_GRID_SIZE(max(to.size(), from.size())));
	linearBackKernel<<<grid, block>>>(
		from, from.size(),
		to, to.size(),
		weights, biases,
		buffer, epsilon);
	syncDevice;
}
