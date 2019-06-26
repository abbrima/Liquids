#pragma once
#include <thread>
#include <chrono>
#include <iostream>

struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = end - start;

		std::cout << "Timer took " << duration.count()*1000 << "ms " << std::endl;
	}
};
