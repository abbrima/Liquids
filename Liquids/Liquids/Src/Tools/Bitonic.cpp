#include <iostream>
#include <algorithm>
#include <stdlib.h>
#define uint unsigned int
struct UnsortedList {
	uint cIndex; uint pIndex;
};
void exchange(UnsortedList& lefter, UnsortedList& righter, bool SL) {
	if (SL) {
		if (lefter.cIndex > righter.cIndex)
		{
			UnsortedList temp = lefter;
			lefter = righter;
			righter = temp;
		}
	}
	else {
		if (lefter.cIndex < righter.cIndex)
		{
			UnsortedList temp = lefter;
			lefter = righter;
			righter = temp;
		}
	}
}
void bitonic1(UnsortedList* arr, const uint& subsize, const uint& cdist,const uint& N)
{
	for (uint i = 0; i < N / 2; i++) {
		uint lefter_index;
		if (i / cdist > 0)
			lefter_index = i * 2 - i % cdist;
		else
			lefter_index = i;

		exchange(arr[lefter_index], arr[lefter_index + cdist], (lefter_index / subsize) % 2 == 0);
	}
}
void bitonic2(UnsortedList* arr, const uint& cdist, const uint& N) {
	for (uint i = 0; i < N / 2; i++) {
		uint lefter_index;
		if (i / cdist > 0)
			lefter_index = i * 2 - i % cdist;
		else
			lefter_index = i;
		exchange(arr[lefter_index], arr[lefter_index + cdist], true);

	}
}
void bsort(UnsortedList* arr, const uint& N) {
	for (uint subsize = 2; subsize < N; subsize <<= 1)
		for (uint compare_distance = subsize >> 1; compare_distance > 0; compare_distance >>= 1)
		{
			bitonic1(arr,subsize,compare_distance,N);
		}

	for (uint compare_distance = N >> 1; compare_distance > 0; compare_distance >>= 1)
	{
		bitonic2(arr,compare_distance,N);
	}
}

int cmpr(const void* first, const void* second) {
	if (((UnsortedList*)first)->cIndex < ((UnsortedList*)second)->cIndex)
		return -1;
	else if (((UnsortedList*)first)->cIndex > ((UnsortedList*)second)->cIndex)
		return 1;
	else
		return 0;
}
bool compare(const UnsortedList& first, const UnsortedList& second)
{
	return first.cIndex < second.cIndex;
}
void hsort(UnsortedList* arr, const uint& n) {
	//qsort(arr, n, sizeof(UnsortedList), cmpr);
	std::sort(arr, arr + n, compare);
}

void bitonicMerge(UnsortedList* arr, uint low, uint cnt, bool up) {
	if (cnt > 1)
	{
		int k = cnt / 2;
		for (int i = low; i < k; i++) 
			exchange(arr[i], arr[i + k], up);
		bitonicMerge(arr, low, k, up);
		bitonicMerge(arr, low + k, k, up);
	}
}
void bitonicSort(UnsortedList* arr, int low, int cnt, bool up) {
	if (cnt > 1)
	{
		int k = cnt / 2;
		bitonicSort(arr, low, k, true);
		bitonicSort(arr, low + k, k, false);
		bitonicMerge(arr, low, cnt, up);
	}
}
void bbsort(UnsortedList* arr, const uint& N) {
	bitonicSort(arr, 0, N, true);
}