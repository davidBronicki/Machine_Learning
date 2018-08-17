#include "hdmem.h"

bool allocateMemory(void** pointer, int size);
bool freeMemory(void* pointer);
bool updateCudaDevice(void* pointerHost, void* pointerDevice, int size);
bool updateThisHost(void* pointerHost, void* pointerDevice, int size);

void runLinearKernel(dmem<float> from, dmem<float> to,
	dmem<float> weights, dmem<float> biases);

void runLinearBackKernel(dmem<float> from, dmem<float> to,
	dmem<float> weights, dmem<float> biases, float epsilon);
