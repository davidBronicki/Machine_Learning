#pragma once

#include <vector>
#include <exception>
#include <iostream>
#include <memory>

class cuda_memory_error : public std::exception{
	int violationType;
public:
	cuda_memory_error();
	cuda_memory_error(int inputViolationType);
	const char* what() const throw() override;
};

template <class T>
class hdmem;

template <class T>
class dmem{
	//short for device memory. Handles the device pointer.
	friend class hdmem<T>;
	struct mem{
		T* pointer;
		int size;
	};
	std::vector<mem>* memory;
	int* referenceCount;
	bool* memLoaded;
	int slot;
	T* getGrandPointer();
	// void setToNewPointer(T* newPointer);
public:
	dmem();
	dmem(int inputSize);
	dmem(std::vector<int> sizes);
	dmem(const dmem<T>& other);
	~dmem();
	dmem<T>& operator=(dmem<T> other);
	dmem<T>& operator+=(const int& other);
	dmem<T> getSlot(int slotToGet);
	void loadMemory();
	void unloadMemory();
	operator T* ();
	int getGrandSize();
	int size();
	int deviceMemoryAllocated();
	int getRefCount();
	bool isLoaded();
	int slotCount();
};

template <class T>
class hdmem
{
	//short for host device memory. Handles the
	//exchange of host memory and device memory.
	dmem<T> deviceMemory;
	std::vector<T> hostMemory;
	hdmem(dmem<T> rootMemory);
public:
	hdmem();
	hdmem(int numberOfElements);
	hdmem(std::vector<int> sizes);
	hdmem(T instantiationData, int blank);
	hdmem(std::vector<T> instantiationData);
	hdmem(std::vector<std::vector<T>> instantiationData);
	hdmem<T>& operator+=(const int& other);
	hdmem<T>& loadMemory();
	hdmem<T>& unloadMemory();
	hdmem<T>& updateHost();
	hdmem<T>& updateDevice();
	dmem<T> getSlot(int slotNumber);
	T get(int index);
	T get(int slot, int index);
	std::vector<T> getAll();
	hdmem<T>& setAll(std::vector<T> refValue);
	operator T* ();
	static std::vector<T> extractData(dmem<T> devData);
	static void placeData(dmem<T> devData, std::vector<T> placementData);
	int size();
	int deviceMemoryAllocated();
	bool isLoaded();
	int slotCount();
};

#include "hdmem.cpp"
