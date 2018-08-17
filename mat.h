#include <vector>
#include <exception>
#include <string>

class bad_mesh_matrix : public std::exception{
	int throwType;
public:
	bad_mesh_matrix(int inputThrowType){
		throwType = inputThrowType;
	}
	const char* what() const throw() override{
		switch(throwType){
			case 0:
				return "invalid matrix dimensions";
			case 1:
				return "mesh loop detected";
			case 2:
				return "multiple inputs detected";
			case 3:
				return "multiple outputs detected";
			case 4:
				return "invalid values in mesh matrix";
			default:
				return "bad mesh matrix";
		}
	}
};

class mat{
	std::vector<std::vector<char>> matrix;
	int dimension;
	int dataSize;
	void swapSpots(int row1, int column1, int row2, int column2);
	void swapRows(int row1, int row2);
	void reduce(int recursionDepth);
	void reduce();
	void checkMeshValues();
public:
	mat();
	mat(std::vector<char> inputMat);
	mat(std::vector<std::vector<char>> inputMat);
	mat(std::vector<int> inputMat);
	mat(std::vector<std::vector<int>> inputMat);
	std::string toString();
};

