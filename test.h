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
		std::cout << "Starting search. " << std::endl;
		auto start = std::chrono::system_clock::now();

		/*
		for (int i = 0; i < 10000; i++) {
			std::vector<bool> test;
			for (int j = 0; j < 8; j++) {
				test.push_back(true);
			}
		}
		*/

		/*
		Tile* tile = new Tile();
		for (int i = 0; i < 10000; i++) {
			std::vector<Tile*> test;
			for (int j = 0; j < 8; j++) {
				test.push_back(tile);
			}
		}
		delete tile;
		*/

		//bool kokot = true;
		bool* boolTest = new bool();
		for (int i = 0; i < 10000; i++) {
			std::vector<bool*> test;
			for (int j = 0; j < 8; j++) {
				test.push_back(boolTest);
			}
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Search finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;

	}
private:
	
};

#endif