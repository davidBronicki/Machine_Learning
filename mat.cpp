#include "mat.h"

#include <string>
#include <iostream>
#include <math.h>

using namespace std;

#define print(input) cout << input << endl

int getDim(int size){
	return (int)(sqrt(8*size + 1) - 1) / 2;
}

bool isTriangle(int size, int dim){
	return size == dim * (dim + 1) / 2;
}

mat::mat(){

}

mat::mat(vector<char> inputMat){
	dimension = getDim(inputMat.size()) + 1;
	if (!isTriangle(inputMat.size(), dimension - 1))
		throw bad_mesh_matrix(0);
	dataSize = inputMat.size();
	matrix = vector<vector<char>>();
	for (int i = 0; i < dimension; i++){
		matrix.push_back(vector<char>());
		for (int j = 0; j < dimension; j++){
			matrix[i].push_back(0);
		}
	}
	int index = 0;
	for (int i = 0; i < dimension; i++){
		for (int j = i + 1; j < dimension; j++){
			matrix[i][j] = -inputMat[index];
			matrix[j][i] = inputMat[index];
			index++;
		}
	}
	checkMeshValues();
	reduce();
}

mat::mat(vector<vector<char>> inputMat){
	dimension = inputMat.size();
	for (int i = 0; i < dimension; i++){
		if (dimension != inputMat[i].size())
			throw bad_mesh_matrix(0);
	}
	dataSize = (dimension - 1) * (dimension - 2) / 2;
	matrix = inputMat;
	checkMeshValues();
	reduce();
}

mat::mat(vector<int> inputMat){
	vector<char> newMat = vector<char>();
	for (int i = 0; i < inputMat.size(); i++){
		newMat.push_back(inputMat[i]);
	}
	*this = mat(newMat);
}

mat::mat(vector<vector<int>> inputMat){
	vector<vector<char>> newMat = vector<vector<char>>();
	for (int i = 0; i < inputMat.size(); i++){
		vector<char> temp = vector<char>();
		for (int j = 0; j < inputMat[i].size(); j++){
			temp.push_back(inputMat[i][j]);
		}
		newMat.push_back(temp);
	}
	*this = mat(newMat);
}

void mat::checkMeshValues(){
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
			char temp = matrix[i][j];
			if (temp != 0 && temp != 1 && temp != -1)
				throw bad_mesh_matrix(4);
		}
	}
}

void mat::swapSpots(int row1, int column1, int row2, int column2){
	char temp = matrix[column1][row1];
	matrix[column1][row1] = matrix[column2][row2];
	matrix[column2][row2] = temp;
}

void mat::swapRows(int row1, int row2){
	for (int i = 0; i < dimension; i++){
		swapSpots(row1, i, row2, i);
	}
	for (int i = 0; i < dimension; i++){
		swapSpots(i, row1, i, row2);
	}
}

void mat::reduce(int recursionDepth){
	if (recursionDepth > dataSize)
		throw bad_mesh_matrix(1);
	for (int i = 1; i < dimension; i++){
		for (int j = 0; j < i; j++){
			if (matrix[i][j] == -1){
				swapRows(i - 1, i);
				reduce(recursionDepth + 1);
				break;
			}
		}
	}
	if (matrix[0][1] == 0)
		throw bad_mesh_matrix(2);
	if (matrix[dimension - 1][dimension - 2] == 0)
		throw bad_mesh_matrix(3);
}

void mat::reduce(){
	reduce(0);
}

string mat::toString(){
	string output = "";
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
			if (matrix[j][i] != -1) output += " ";
			output += to_string(matrix[j][i]);
			if (j != dimension - 1) output += ",";
		}
		if (i != dimension) output += "\n";
	}
	return output;
}