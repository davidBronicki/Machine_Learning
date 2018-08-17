#include "network.h"
#include "mat.h"

class bad_matrix : public std::exception{
	int throwType;
public:
	bad_matrix();
	bad_matrix(int inputThrowType);
	const char* what() const throw() override;
};

class brain{
	std::vector<network> networks;
	std::vector<node> nodes;
	std::vector<std::vector<char>> setupParameters;
	int networkIndex(int indexA, int indexB);
	void swap(int indexA, int indexB);
	void fixFormat();
	void fixFormat(int recursionDepth);
	void spliceInNetworks();
public:
	brain();
	brain(std::vector<int> initialLayers);
	brain(std::vector<std::vector<char>> footprint,
		std::vector<int> nodeSizes,
		std::vector<std::vector<int>> layerSizes);
	brain(std::vector<char> footprint,
		std::vector<int> nodeSizes,
		std::vector<std::vector<int>> layerSizes);
	// brain(std::vector<char> footprint,
	// 	std::vector<int> nodeSizes,
	// 	std::vector<network> inputNetworks);
	brain(const brain& other);
	void spliceIn(brain& other, int startNodeIndex, int endNodeIndex);//not implemented
	void loadMemory();
	void unloadMemory();//not implemented
	std::vector<float> pushThrough(std::vector<float> input);
	void pullThrough(std::vector<float> actualOutput, float epsilon);
	void loadWeights(std::vector<std::vector<std::vector<float>>> weights);
	void randomizeWeights();
	std::string footprintString();
};
