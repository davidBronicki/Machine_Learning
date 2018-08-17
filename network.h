#pragma once

#include <vector>

#include "node.h"
#include "layer.h"

class network{
	friend class node;
	node* startNode;
	node* endNode;
	std::vector<layer> workers;
	void hookOutput(dmem<float> newOutput);
	void hookInput(dmem<float> newInput);
public:
	network(std::vector<int> layerSizes);
	network(std::vector<int> layerSizes,
		std::vector<std::vector<std::vector<float>>> weights);
	network(std::vector<layer> instantiationLayers);
	bool operator==(const network& other);
	void spliceIn(node& start, node& end);
	void spliceOut();//not implemented
	void pushForward();
	void pullBack(float epsilon);
	int inputSize();
	int outputSize();
	void loadMemory();
	void loadWeights(std::vector<std::vector<float>> weights);
	void randomizeWeights();
	std::vector<std::vector<float>> getWeights();//not implemented
};
