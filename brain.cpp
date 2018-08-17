#include "global_include.h"
#include "brain.h"

#include <math.h>

using namespace std;

#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "brain: " << input << std::endl
#endif

namespace specifier{
	template <class T>
	void swap(T& a, T& b){
		T c = a;
		a = b;
		b = c;
	}
}

bad_matrix::bad_matrix() : bad_matrix(0){}

bad_matrix::bad_matrix(int inputThrowType){
	throwType = inputThrowType;
}

const char* bad_matrix::what() const throw(){
	switch(throwType){
		case 1:
			return "mesh loop detected";
		case 2:
			return "multiple inputs detected";
		case 3:
			return "multiple outputs detected";
		case 4:
			return "invalid values in mesh matrix";
		case 5:
			return "invalid matrix dimensions";
		default:
			return "bad mesh matrix";
	}
}

int triange(int n){
	return n * (n + 1) / 2;
}

int triangleIndex(int y){
	return (int)(sqrt(8 * y + 1) - 1) / 2;
}

void validateFootprint(vector<vector<char>> footprint){
	#ifdef DEBUG_MODE
	print("validating footprint");
	#endif
	for (vector<char> thing : footprint){
		if (thing.size() != footprint.size())
			throw bad_matrix(5);
		for (char i : thing){
			bool temp = false;
			temp |= i == -1;
			temp |= i == 0;
			temp |= i == 1;
			if (!temp) throw bad_matrix(4);
		}
	}
}

int linearIndex(int x, int y, int size){
	int index = 0;
	int j = 0;
	for (; j < y; j++){
		index += size - j - 1;
	}
	index += x - j - 1;
	return index;
}

vector<vector<char>> buildFootprint(vector<char> simpleFootprint){
	#ifdef DEBUG_MODE
	print("building vec<vec<char>> footprint from vec<char> footprint");
	#endif
	int n = triangleIndex(simpleFootprint.size());
	if (triange(n) != simpleFootprint.size())
		throw bad_matrix(5);
	int size = n + 1;
	vector<vector<char>> output;
	for (int i = 0; i < size; i++){
		vector<char> temp;
		for (int j = 0; j < size; j++){
			if (i == j) temp.push_back(0);
			else if(i < j){
				temp.push_back(-simpleFootprint[linearIndex(j, i, size)]);
			}
			else{
				temp.push_back(simpleFootprint[linearIndex(i, j, size)]);
			}
		}
		output.push_back(temp);
	}
	return output;
}

vector<vector<char>> buildFootprint(vector<int> simpleFootprint){
	vector<char> temp;
	for (int i : simpleFootprint){
		temp.push_back(i);
	}
	return buildFootprint(temp);
}

vector<vector<char>> buildFootprint(vector<vector<int>> intFootprint){
	vector<vector<char>> output;
	for (vector<int> thing : intFootprint){
		output.push_back(vector<char>());
		for (int i : thing){
			output.back().push_back(i);
		}
	}
	return output;
}

vector<network> buildNetworks(vector<vector<int>> layerSizes){
	vector<network> output;
	for (vector<int> layerData : layerSizes){
		output.push_back(network(layerData));
	}
	return output;
}

vector<node> buildNodes(vector<int> sizes){
	vector<node> output;
	output.push_back(node(sizes[0]));
	for (int i = 1; i < sizes.size(); i++){
		output.push_back(node());
	}
	// for (int size : sizes){
	// 	output.push_back(node(size));
	// }
	return output;
}

brain::brain(){
	
}

brain::brain(vector<int> initialLayers){
	nodes = vector<node>({node(initialLayers[0]), node()});
	networks = vector<network>({network(initialLayers)});
	networks[0].spliceIn(nodes[0], nodes[1]);
	setupParameters = {{0, -1}, {1, 0}};
}

brain::brain(vector<vector<char>> footprint,
	vector<int> nodeSizes,
	vector<vector<int>> layerSizes){
	
	setupParameters = footprint;
	validateFootprint(setupParameters);
	networks = buildNetworks(layerSizes);
	nodes = buildNodes(nodeSizes);
	fixFormat();
	spliceInNetworks();
}

brain::brain(vector<char> footprint,
	vector<int> nodeSizes,
	vector<vector<int>> layerSizes)
	: brain(buildFootprint(footprint), nodeSizes, layerSizes){}

// brain::brain(vector<char> footprint,
// 	vector<int> nodeSizes,
// 	vector<network> inputNetworks){

// 	setupParameters = buildFootprint(footprint);
// 	validateFootprint(setupParameters);
// 	networks = inputNetworks;
// 	nodes = buildNodes(nodeSizes);
// 	fixFormat();
// 	spliceInNetworks();
// }

void brain::spliceInNetworks(){
	#ifdef DEBUG_MODE
	print("splicing in networks");
	#endif
	for (int i = 1; i < nodes.size(); i++){
		for (int j = 0; j < i; j++){
			if (setupParameters[i][j] == 1){
				networks[networkIndex(i, j)].spliceIn(
					nodes[j], nodes[i]);
			}
		}
	}
}

int brain::networkIndex(int indexA, int indexB){
	#ifdef DEBUG_MODE
	print("getting network index for " << indexA << " and " << indexB);
	#endif
	int index = 0;
	for (int j = 0; j < indexB; j++){
		for (int i = j + 1; i < nodes.size(); i++){
			if (setupParameters[i][j] != 0){
				index++;
			}
		}
	}
	for (int i = indexB + 1; i < indexA; i++){
		if (setupParameters[i][indexB] != 0){
			index++;
		}
	}
	return index;
}

void brain::swap(int indexA, int indexB){
	#ifdef DEBUG_MODE
	print("swapping " << indexA << " and " << indexB);
	#endif
	specifier::swap(nodes[indexA], nodes[indexB]);
	for (int i = 0; i < nodes.size(); i++){
		specifier::swap(networks[networkIndex(i, indexA)],
			networks[networkIndex(i, indexB)]);
		specifier::swap(setupParameters[i][indexA],
			setupParameters[i][indexB]);
	}
	for (int i = 0; i < nodes.size(); i++){
		specifier::swap(networks[networkIndex(indexA, i)],
			networks[networkIndex(indexB, i)]);
		specifier::swap(setupParameters[indexA][i],
			setupParameters[indexB][i]);
	}
}

void brain::fixFormat(int recursionDepth){
	#ifdef DEBUG_MODE
	print("entering format fix iteration " << recursionDepth);
	#endif
	if (recursionDepth > triange(nodes.size() - 1)) throw bad_matrix(1);
	for (int i = 1; i < nodes.size(); i++){
		for (int j = 0; j < i; j++){
			if (setupParameters[i][j] == -1){
				swap(i - 1, i);
				return fixFormat(recursionDepth + 1);
			}
		}
	}
	if (setupParameters[0][1] == 0) throw bad_matrix(2);
	if (setupParameters.back()[nodes.size() - 2] == 0) throw bad_matrix(3);
}

void brain::fixFormat(){
	fixFormat(0);
};

void brain::loadMemory(){
	#ifdef DEBUG_MODE
	print("allocating device memory");
	#endif
	for (network thing : networks){
		#ifdef DEBUG_MODE
		print("allocating network");
		#endif
		thing.loadMemory();
	}
}

vector<float> brain::pushThrough(vector<float> input){
	#ifdef DEBUG_MODE
	print("beginning forward propagation");
	#endif
	for (node item : nodes){
		item.resetReachedBools();
	}
	nodes[0].start(input);
	return nodes.back().getData();
}

void brain::pullThrough(vector<float> actualOutput, float epsilon){
	#ifdef DEBUG_MODE
	print("beginning back propagation");
	#endif
	nodes.back().backStart(actualOutput, epsilon);
}

void brain::loadWeights(vector<vector<vector<float>>> weights){
	#ifdef DEBUG_MODE
	print("loading weights");
	#endif
	if (weights.size() != networks.size())
		throw cuda_memory_error(6);
	for (int i = 0; i < weights.size(); i++){
		networks[i].loadWeights(weights[i]);
	}
}

void brain::randomizeWeights(){
	#ifndef LOW_PROFILE_PRINTING
	print("randomizing weights");
	#endif
	for (network net : networks){
		net.randomizeWeights();
	}
}

string padToLength(string input, int length){
	string output = input;
	while (output.size() < length){
		output.insert(0, " ");
	}
	return output;
}

string brain::footprintString(){
	int maxSize = 0;
	for (node thing : nodes){
		maxSize = max(maxSize, thing.size());
	}
	int length = max((int)log10(maxSize - 1) + 1, 2);
	string output;
	for (node thing : nodes){
		string temp = to_string(thing.size());
		output += padToLength(temp, length) + ",";
	}
	output.pop_back();
	output += "\n";
	for (int j = 0; j < setupParameters.size(); j++){
		for (int i = 0; i < setupParameters.size(); i++){
			string temp = to_string(setupParameters[i][j]);
			output += padToLength(temp, length) + ",";
		}
		output.pop_back();
		output += "\n";
	}
	output.pop_back();
	return output;
}
