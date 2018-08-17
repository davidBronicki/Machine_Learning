#include "global_include.h"
#include "network.h"

#include <iostream>

using namespace std;

#ifdef DEBUG_MODE
#define print(input) cout << "network: " << input << endl;
#else
#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "network: " << input << std::endl
#endif
#endif

network::network(vector<int> layerSizes){
	workers = vector<layer>();
	for (int i = 1; i < layerSizes.size(); i++){
		workers.push_back(layer(layerSizes[i - 1], layerSizes[i]));
	}
	for (int i = 1; i < workers.size(); i++){
		workers[i].hookBack(workers[i - 1]);
	}
}

network::network(std::vector<layer> instantiationLayers){
	workers = instantiationLayers;
}

bool network::operator==(const network& other){
	bool output = true;
	output &= startNode == other.startNode;
	output &= endNode == other.endNode;
	// output &= workers == other.workers;
	return output;
}

void network::spliceIn(node& start, node& end){
	#ifdef DEBUG_MODE
	print("splicing in");
	#endif
	start.spliceInForward(*this);
	end.spliceInBack(*this);
	startNode = &start;
	endNode = &end;
}

void network::pushForward(){
	#ifdef DEBUG_MODE
	print("executing")
	#endif
	for (int i = 0; i < workers.size(); i++){
		workers[i].pushForward();
	}
	endNode->execute(*this);
}

void network::pullBack(float epsilon){
	#ifdef DEBUG_MODE
	print("back executing");
	#endif
	for (int i = workers.size() - 1; i >= 0; i--){
		workers[i].pullBack(epsilon);
	}
	startNode->backExecute(*this, epsilon);
}

int network::inputSize(){
	return workers[0].inputSize();
}

int network::outputSize(){
	return workers.back().outputSize();
}

void network::hookOutput(dmem<float> newOutput){
	#ifdef DEBUG_MODE
	print("hooking output");
	#endif
	workers.back().setOutput(newOutput);
}

void network::hookInput(dmem<float> newInput){
	#ifdef DEBUG_MODE
	print("hooking input");
	#endif
	workers[0].setInput(newInput);
}

void network::loadMemory(){
	for (int i = 0; i < workers.size(); i++){
		#ifdef DEBUG_MODE
		print("allocating layer");
		#endif
		workers[i].loadMemory();
	}
}

void network::loadWeights(vector<vector<float>> weights){
	if (weights.size() != workers.size())
		throw cuda_memory_error(6);
	for (int i = 0; i < weights.size(); i++){
		workers[i].loadWeights(weights[i]);
	}
}

void network::randomizeWeights(){
	for (layer worker : workers){
		worker.randomizeWeights();
	}
}

vector<vector<float>> getWeights(){

}
