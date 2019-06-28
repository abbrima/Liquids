#pragma once
#include <thrust/device_vector.h>
#include <thrust/sort.h>

struct Elem {
	unsigned int key;
	unsigned int val;

	bool operator== (const Elem& e) { return key == e.key; }
	bool operator!= (const Elem& e) { return !((*this) == e); }
	bool operator<  (const Elem& r) { return key < r.key; }
	bool operator<= (const Elem& r) { return key <= r.key; }
	bool operator>  (const Elem& r) { return key > r.key; }
	bool operator>= (const Elem& r) { return key >= r.key; }
};


void mCudaSort(unsigned int* ptr, unsigned int length) {
	thrust::device_ptr<unsigned int> device_ptr = thrust::device_pointer_cast<unsigned int>(ptr);
	thrust::sort(device_ptr, device_ptr + length);
}
void mCudaPairSort(unsigned int* keys,unsigned int length) {
	thrust::device_ptr<unsigned int> keys_ptr = thrust::device_pointer_cast<unsigned int>(keys);
	thrust::sort(keys_ptr, keys_ptr + length);
	
}