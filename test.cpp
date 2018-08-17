#include "global_include.h"

#include <string>
#include <iostream>
#include <math.h>

#include "brain.h"

using namespace std;

#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "main: " << input << std::endl
#endif

float randomFloatThing(){
	return (float)rand() / RAND_MAX;
}

int test(){
	brain test({1, 6, 8, 6, 1});
	test.loadMemory();
	for (int i = 0; i < 1000; i++){
		float temp = randomFloatThing();
		test.pushThrough({temp});
		// test.pullThrough({sqrt(temp)}, .05);
	}
	for (int i = 0; i < 5; i++){
		float temp = randomFloatThing();
		print(temp);
		print(sqrt(temp));
		vector<float> newThing = test.pushThrough({temp});
		print(newThing[0]);
		print("");
	}
}

int main(){
	try{
		return test();
	}
	catch(exception& e){
		print(e.what());
	}
	return 1;
}
