#include "global_include.h"
#include "network.h"

#include <string>
#include <iostream>

using namespace std;

#ifdef DEBUG_MODE
#define print(input) cout << "node: " << input << endl;
#else
#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "node: " << input << std::endl
#endif
#endif

node::node(){
	data = hdmem<float>();
}

node::node(int size){
	data = hdmem<float>(size);
}

void node::execute(){
	#ifdef DEBUG_MODE
	print("executing");
	#endif
	for (int i = 0; i < forwardEdges.size(); i++){
		forwardEdges[i]->pushForward();
	}
}

void node::execute(network& source){
	int location = 0;
	for (; location < backwardEdges.size(); location++){
		if (source == *(backwardEdges[location])) break;
	}
	backReached[location] = true;
	bool allReached = true;
	for (int i = 0; i < backReached.size(); i++){
		allReached &= backReached[i];
	}
	if (allReached) execute();
}

void node::backExecute(float epsilon){
	#ifdef DEBUG_MODE
	print("back executing");
	#endif
	for (int i = 0; i < backwardEdges.size(); i++){
		backwardEdges[i]->pullBack(epsilon);
	}
}

void node::backExecute(network& source, float epsilon){
	int location = 0;
	for (; location < forwardEdges.size(); location++){
		if (source == *(forwardEdges[location])) break;
	}
	forwardReached[location] = true;
	bool allReached = true;
	for (int i = 0; i < forwardReached.size(); i++){
		allReached &= forwardReached[i];
	}
	if (allReached) backExecute(epsilon);
}

void node::resetReachedBools(){
	for (int i = 0; i < forwardReached.size(); i++){
		forwardReached[i] = false;
	}
	for (int i = 0; i < backReached.size(); i++){
		backReached[i] = false;
	}
}

void node::start(vector<float> startData){
	data.setAll(startData);
	execute();
}

void node::backStart(vector<float> actualData, float epsilon){
	vector<float> currentData = data.getAll();
	if (currentData.size() != actualData.size()){
		print(currentData.size() << ", " << actualData.size());
		throw cuda_memory_error(6);
	}
	for (int i = 0; i < currentData.size(); i++){
		currentData[i] = actualData[i] - currentData[i];
	}
	data.setAll(currentData);
	backExecute(epsilon);
}

void node::spliceInBack(network& back){
	#ifdef DEBUG_MODE
	print("splicing in back network");
	#endif
	data += back.outputSize();
	for (int i = 0; i < forwardEdges.size(); i++){
		forwardEdges[i]->hookInput(data.getSlot(-1));
	}
	backwardEdges.push_back(&back);
	backReached.push_back(false);
	back.hookOutput(data.getSlot(data.slotCount() - 1));
}

void node::spliceInForward(network& forward){
	#ifdef DEBUG_MODE
	print("splicing in forward network");
	#endif
	if (data.size() != forward.inputSize()){
		throw bad_splice(1);
	}
	forward.hookInput(data.getSlot(-1));
	forwardEdges.push_back(&forward);
	forwardReached.push_back(false);
}

vector<float> node::getData(){
	return data.getAll();
}

void node::loadMemory(){
	data.loadMemory();
}

int node::size(){
	return data.size();
}