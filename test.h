#ifndef TEST_H
#define TEST_H

//#include <ctime>
#include <chrono>

/* class Test
This class is only for testing and is never used in the real code.
*/

using namespace std;

class Test {
public:
	Test() {
		//cout << time(NULL) << endl;
		auto start = std::chrono::system_clock::now();
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = end - start;
		std::cout << "Time to fill and iterate a vector of " << " ints : " << diff.count() << " s\n";
	}
private:
	
};

#endif