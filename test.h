#ifndef TEST_H
#define TEST_H

#include <vector>

#include <iostream>
#include <chrono>
#include "tile.h"

/* class Test
This class is only for testing and is never used in the real code.
*/

using namespace std;

class Test {
public:
	Test() {
		/*
		std::cout << "Starting test. " << std::endl;
		auto start = std::chrono::system_clock::now();
		*/
				
		Tile* tile = new Tile();
		std::vector<Tile*> test;
		for (int i = 0; i < 10000; i++) {
			test.push_back(tile);
		}

		std::cout << "Starting test. " << std::endl;
		auto start = std::chrono::system_clock::now();

		for (int i = 0; i < 10000; i++) {
			tile->reset();
		}

		delete tile;
		

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Test finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;

	}
private:
	
};

#endif