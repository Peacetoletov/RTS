#ifndef TEST_H
#define TEST_H

//#include <ctime>
#include <chrono>
#include <iostream>

#include <list>
#include <vector>
#include <algorithm>    // std::sort

/* class Test
This class is only for testing and is never used in the real code.
*/

using namespace std;

class Test {
public:
	Test() {
		//cout << time(NULL) << endl;
		auto start = std::chrono::system_clock::now();

		/*
		std::list<int> test;
		std::list<int>::iterator iter;
		test.push_back(5);
		test.push_back(5);
		test.push_back(5);
		for (int i = 0; i < 8000; i++) {
			iter = test.begin();
			test.insert(iter, i);
			--iter;
		}
		*/
		
		/*
		std::vector<int> test;
		std::vector<int>::iterator iter;
		test.push_back(1);
		test.push_back(2);
		iter = test.begin();
		test.push_back(3);
		for (int i = 0; i < 8000; i++) {
			for (int j = 0; j < 8000; j++) {
				if (j == i / 2) {
					break;
				}
			}
			iter = test.begin();
			//test.push_back(i);
			test.insert((test.begin() + i / 2), i);
		}
		*/

		/*
		std::vector<int> test;
		std::vector<int>::iterator iter;
		test.push_back(1);
		test.push_back(2);
		test.push_back(3);
		for (int i = 0; i < 3000; i++) {
			int value = i % 2 == 0 ? i : -i;
			for (int j = 0; j < test.size(); j++) {
				 //od menšího k vìtšímu
				if (value > test[j] && j != test.size() - 1) {
					continue;
				}
				else {
					iter = test.begin() + j;
					test.insert(iter, i);
					break;
				}
			}
		}
		*/

		std::vector<int> test;
		for (int i = 0; i < 100; i++) {
			test.push_back(i);
		}
		
		/*
		//1) Change one value, sort (N log (N))
		for (int i = 0; i < 10000; i++) {
			test[50] = 7;
			std::sort(test.begin(), test.end());
		}
		*/
		
		
		//2) Erase one element, insert it back
		std::vector<int>::iterator iter;
		for (int i = 0; i < 10000; i++) {
			//Simulate O(n)
			for (int j = 0; j < 51; j++) {
				if (j == 50) {
					test.erase(test.begin() + 50);
				}
			}

			//Simulate O(log n)
			for (int j = 0; j < 11; j++) {
				if (j == 10) {
					iter = test.begin() + 50;
					test.insert(iter, 7);
				}
			}
		}
		
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;
	}
private:
	
};

#endif