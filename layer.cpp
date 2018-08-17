#include "global_include.h"
#include "layer.h"
#include "cudaWrapperFunctions.h"

#include <iostream>

using namespace std;

#ifdef DEBUG_MODE
#define print(input) cout << "layer: " << input << endl;
#else
#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "layer: " << input << std::endl
#endif
#endif

float randomFloat(){
	return (float)rand() / RAND_MAX;
}

bad_splice::bad_splice(){
	errorType = 0;
}
bad_splice::bad_splice(int inputErrorType){
	errorType = inputErrorType;
}
const char* bad_splice::what() const throw(){
	switch(errorType){
		case 1:
			return "miss-match data length";
			break;
		default:
			return "bad network splicing";
			break;
	}
}

layer::layer(int sizeFrom, int sizeTo){
	dataFrom = dmem<float>(sizeFrom);
	dataTo = dmem<float>(sizeTo);
	weights = dmem<float>(sizeTo * sizeFrom);
	biases = dmem<float>(sizeTo);
}



void layer::hookBack(layer other){
	#ifdef DEBUG_MODE
	print("hooking to back layer");
	#endif
	dataFrom = other.dataTo;
}

void layer::hookFront(layer other){
	#ifdef DEBUG_MODE
	print("hooking to front layer");
	#endif
	dataTo = other.dataFrom;
}

void layer::pushForward(){
	runLinearKernel(dataFrom, dataTo, weights, biases);
}

void layer::pullBack(float epsilon){
	runLinearBackKernel(dataFrom, dataTo, weights, biases, epsilon);
}

int layer::inputSize(){
	return dataFrom.size();
}

int layer::outputSize(){
	return dataTo.size();
}

void layer::setOutput(dmem<float> front){
	#ifdef DEBUG_MODE
	print("setting front memory location");
	#endif
	dataTo = front;
}

void layer::setInput(dmem<float> back){
	#ifdef DEBUG_MODE
	print("setting back memory location");
	#endif
	dataFrom = back;
}

void layer::loadMemory(){
	dataFrom.loadMemory();
	dataTo.loadMemory();
	weights.loadMemory();
	biases.loadMemory();
}

void layer::loadWeights(vector<float> inputWeights){
	hdmem<float>::placeData(weights, inputWeights);
}

void layer::randomizeWeights(){
	vector<float> newWeights;
	for (int i = 0; i < weights.size(); i++){
		newWeights.push_back(randomFloat() * 2 - 1);
	}
	hdmem<float>::placeData(weights, newWeights);
}

vector<float> layer::getWeights(){
	return hdmem<float>::extractData(weights);
}
