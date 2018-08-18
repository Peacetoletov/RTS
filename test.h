#ifndef TEST_H
#define TEST_H

#include <vector>

#include <iostream>

/* class Test
This class is only for testing and is never used in the real code.
*/

using namespace std;

class Test {
public:
	Test() {
		std::vector<int> test1;
		test1.push_back(5);
		test1.push_back(6);
		test1.push_back(8);
		std::vector<int> test2 = test1;

		std::cout << &test1 << "   " << &test2 << std::endl;


	}
private:
	
};

#endif