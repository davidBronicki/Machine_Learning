#pragma once

#include <vector>
#include <exception>

#include "hdmem.h"

class bad_splice : public std::exception{
	int errorType;
public:
	bad_splice();
	bad_splice(int inputErrorType);
	const char* what() const throw() override;
};

class layer{
	dmem<float> dataFrom;
	dmem<float> dataTo;
	dmem<float> weights;
	dmem<float> biases;
public:
	layer(int sizeFrom, int sizeTo);
	void hookBack(layer other);
	void hookFront(layer other);
	void setInput(dmem<float> back);
	void setOutput(dmem<float> front);
	void pushForward();
	void pullBack(float epsilon);
	int inputSize();
	int outputSize();
	void loadMemory();
	void loadWeights(std::vector<std::vector<float>> inputWeights);//not implemented
	void loadWeights(std::vector<float> inputWeights);
	void randomizeWeights();
	std::vector<float> getWeights();
};