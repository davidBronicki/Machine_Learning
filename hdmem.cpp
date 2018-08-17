#include "hdmem.h"
#include "cudaWrapperFunctions.h"

#include <vector>
#include <iostream>
#include <string>

#ifdef LOW_PROFILE_PRINTING
#define print(input) std::cout << input << std::endl
#else
#define print(input) std::cout << "hdmem: " << input << std::endl
#endif

template <class T>
dmem<T>::dmem(){
	memory = new std::vector<dmem::mem>();
	referenceCount = new int(1);
	memLoaded = new bool(false);
	slot = -1;
}

template <class T>
dmem<T>::dmem(int inputSize) : dmem(){
	memory->push_back({0, inputSize});
}

template <class T>
dmem<T>::dmem(std::vector<int> sizes) : dmem(){
	for (int i = 0; i < sizes.size(); i++){
		memory->push_back({0, sizes[i]});
	}
}

template <class T>
dmem<T>::dmem(const dmem<T>& other){
	memory = other.memory;
	referenceCount = other.referenceCount;
	memLoaded = other.memLoaded;
	slot = other.slot;
	(*referenceCount)++;
}

template <class T>
T* dmem<T>::getGrandPointer(){
	return ((*memory)[0].pointer);
}

template <class T>
dmem<T>::~dmem(){
	(*referenceCount)--;
	if (*referenceCount == 0){
		if (*memLoaded){
			freeMemory((void*)getGrandPointer());
		}
		delete referenceCount;
		delete memLoaded;
		delete memory;
	}
}

template <class T>
dmem<T>& dmem<T>::operator=(dmem<T> other){
	(*this).~dmem<T>();
	memory = other.memory;
	referenceCount = other.referenceCount;
	memLoaded = other.memLoaded;
	slot = other.slot;
	(*referenceCount)++;
	return *this;
}

template <class T>
dmem<T>& dmem<T>::operator+=(const int& other){
	if (*memLoaded){
		std::vector<T> temp = hdmem<T>::extractData(*this);
		for (int i = 0; i < other; i++){
			temp.push_back(T());
		}
		unloadMemory();
		memory->push_back({0, other});
		loadMemory();
		hdmem<T>::placeData(*this, temp);
	}
	else{
		memory->push_back({0, other});
	}
	return *this;
}

template <class T>
dmem<T> dmem<T>::getSlot(int slotToGet){
	if ((slotToGet < 0 || slotToGet >= memory->size()) != (slotToGet == -1)){
		throw cuda_memory_error(4);
	}
	dmem<T> temp = dmem<T>(*this);
	temp.slot = slotToGet;
	return temp;
}

template <class T>
void dmem<T>::loadMemory(){
	if (!*memLoaded){
		void* temp = (void*)getGrandPointer();
		bool worked =
			allocateMemory(&temp, getGrandSize() * sizeof(T));
		if (!worked) throw cuda_memory_error(1);
		T* newPointer = (T*)temp;
		int position = 0;
		for (int i = 0; i < memory->size(); i++){
			(*memory)[i].pointer = newPointer + position;
			position += (*memory)[i].size;
		}
		*memLoaded = true;
	}
}

template <class T>
void dmem<T>::unloadMemory(){
	if (*memLoaded){
		freeMemory((void*)getGrandPointer());
		*memLoaded = false;
	}
}

template <class T>
dmem<T>::operator T*(){
	if (slot == -1) return getGrandPointer();
	else{
		return (*memory)[slot].pointer;
	}
}

template <class T>
int dmem<T>::getGrandSize(){
	int output = 0;
	for (int i = 0; i < memory->size(); i++){
		output += (*memory)[i].size;
	}
	return output;
}

template <class T>
int dmem<T>::size(){
	if (slot == -1){
		return getGrandSize();
	}
	else{
		return (*memory)[slot].size;
	}
}

template <class T>
int dmem<T>::deviceMemoryAllocated(){
	if (*memLoaded) return getGrandSize() * sizeof(T);
	else return 0;
}

template <class T>
int dmem<T>::getRefCount(){
	return *referenceCount;
}

template <class T>
bool dmem<T>::isLoaded(){
	return *memLoaded;
}

template <class T>
int dmem<T>::slotCount(){
	return memory->size();
}





template <class T>
hdmem<T>::hdmem(){
	deviceMemory = dmem<T>();
	hostMemory = std::vector<T>();
}

template <class T>
hdmem<T>::hdmem(int numberOfElements){
	deviceMemory = dmem<T>(numberOfElements);
	hostMemory = std::vector<T>(numberOfElements, T());
}

template <class T>
hdmem<T>::hdmem(T instantiationData, int blank)
{
	deviceMemory = dmem<T>(1);
	hostMemory = std::vector<T>({instantiationData});
}

template <class T>
hdmem<T>::hdmem(std::vector<T> instantiationData)
{
	deviceMemory = dmem<T>(1);
	hostMemory = instantiationData;
}

template <class T>
hdmem<T>::hdmem(std::vector<int> sizes){
	deviceMemory = dmem<T>(sizes);
	hostMemory = std::vector<T>(deviceMemory.getGrandSize(), T());
}

template <class T>
hdmem<T>::hdmem(std::vector<std::vector<T>> instantiationData){
	std::vector<int> sizes = std::vector<int>();
	hostMemory = std::vector<T>();
	for (int i = 0; i < instantiationData.size(); i++){
		sizes.push_back(instantiationData[i].size());
		for (int j = 0; j < instantiationData[i].size(); j++){
			hostMemory.push_back(instantiationData[i][j]);
		}
	}
	deviceMemory = dmem<T>(sizes);
}

template <class T>
hdmem<T>::hdmem(dmem<T> rootMemory){
	deviceMemory = rootMemory;
	hostMemory = std::vector<T>(rootMemory.getGrandSize());
}

template <class T>
hdmem<T>& hdmem<T>::operator+=(const int& other){
	deviceMemory += other;
	for (int i = 0; i < other; i++){
		hostMemory.push_back(T());
	}
	return *this;
}

template <class T>
hdmem<T>& hdmem<T>::loadMemory()
{
	deviceMemory.loadMemory();
	return *this;
}

template <class T>
hdmem<T>& hdmem<T>::unloadMemory()
{
	deviceMemory.unloadMemory();
	return *this;
}

template <class T>
hdmem<T>& hdmem<T>::updateDevice()
{
	bool worked = updateCudaDevice((void*)&(hostMemory[0]),
		(void*)(deviceMemory.getGrandPointer()), size() * sizeof(T));
	if (!worked){
		throw cuda_memory_error(3);
	}
	return *this;
}

template <class T>
hdmem<T>& hdmem<T>::updateHost()
{
	bool worked = updateThisHost((void*)&(hostMemory[0]),
		(void*)(deviceMemory.getGrandPointer()), size() * sizeof(T));
	if (!worked){
		throw cuda_memory_error(2);
	}
	return *this;
}

template <class T>
dmem<T> hdmem<T>::getSlot(int slotNumber){
	return deviceMemory.getSlot(slotNumber);
}

template <class T>
T hdmem<T>::get(int index)
{
	if (index < 0 || index >= size())
		throw cuda_memory_error(5);
	return hostMemory[index];
}

template <class T>
T hdmem<T>::get(int slot, int index){
	if (slot < 0 || slot >= deviceMemory.memory->size())
		throw cuda_memory_error(4);
	if (index < 0 || index >= (*(deviceMemory.memory))[slot].size)
		throw cuda_memory_error(5);
	int trueIndex = 0;
	for (int i = 0; i < slot; i++){
		trueIndex += (*(deviceMemory.memory))[i].size;
	}
	return hostMemory[trueIndex];
}

template <class T>
std::vector<T> hdmem<T>::getAll()
{
	updateHost();
	return hostMemory;
}

template <class T>
hdmem<T>& hdmem<T>::setAll(std::vector<T> refValue)
{
	if (refValue.size() != hostMemory.size())
		throw cuda_memory_error(0);
	hostMemory = refValue;
	if (deviceMemory.isLoaded()) return updateDevice();
	return *this;
}

template <class T>
hdmem<T>::operator T* ()
{
	return deviceMemory.getGrandPointer();
}

template <class T>
std::vector<T> hdmem<T>::extractData(dmem<T> devData)
{
	hdmem<T> helper(devData);
	helper.updateHost();
	return helper.getAll();
}

template <class T>
void hdmem<T>::placeData(dmem<T> devData, std::vector<T> placementData)
{
	hdmem<T> helper(devData);
	helper.setAll(placementData);
	helper.updateDevice();
}

template <class T>
int hdmem<T>::size()
{
	return hostMemory.size();
}

template <class T>
int hdmem<T>::deviceMemoryAllocated()
{
	if (*(deviceMemory.memLoaded)) return size() * sizeof(T);
	else return 0;
}

template <class T>
bool hdmem<T>::isLoaded(){
	return *(deviceMemory.memLoaded);
}

template <class T>
int hdmem<T>::slotCount(){
	return deviceMemory.slotCount();
}

#undef print