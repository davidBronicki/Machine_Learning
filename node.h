#pragma once

#include <vector>

#include "hdmem.h"

class network;

class node{
	friend class network;
	hdmem<float> data;
	std::vector<network*> forwardEdges;
	std::vector<network*> backwardEdges;
	std::vector<bool> backReached;
	std::vector<bool> forwardReached;
	void execute();
	void backExecute(float epsilon);
	void execute(network& source);
	void backExecute(network& source, float epsilon);
	void spliceInForward(network& forward);
	void spliceOutForward(network& forward);//not implemented
	void spliceInBack(network& back);
	void spliceOutBack(network& back);//not implemented
public:
	node();
	node(int size);
	void start(std::vector<float> startData);
	void backStart(std::vector<float> backData, float epsilon);
	void loadMemory();
	void resetReachedBools();
	std::vector<float> getData();
	int size();
};
