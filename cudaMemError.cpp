#include "hdmem.h"

cuda_memory_error::cuda_memory_error() : cuda_memory_error(0){}
cuda_memory_error::cuda_memory_error(int inputViolationType){
	violationType = inputViolationType;
}
const char* cuda_memory_error::what() const throw(){
	switch(violationType){
		case 1:
			return "unable to allocate device memory";
		case 2:
			return "unable to update host memory";
		case 3:
			return "unable to update device memory";
		case 4:
			return "slot index out of range";
		case 5:
			return "array index out of range";
		case 6:
			return "array size miss match";
		case 7:
			return "attempted device access when not loaded";
		default:
			return "host-device memory error";
	}
}