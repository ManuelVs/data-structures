#include <iostream>

#include "heaps/WilliamHeap.hpp"
#include "heaps/BinomialHeap.hpp"

template<typename T>
void print_heap(T heap){
	if(!heap.empty()){
		std::cout << heap.pop();
		while(!heap.empty()){
			std::cout << ' ' << heap.pop();
		}
		std::cout << std::endl;
	}
}

/**
 * @brief Main function
 * 
 * @param argc number of arguments
 * @param argv array of arguments
 * @return int result code
 */
int main(int argc, const char *argv[]) {
	std::cout << "hello Visual Studio Code! :)" << '\n';

	WilliamHeap<int> a;
	a.push(1);
	a.push(2);
	a.push(3);

	print_heap(a);

	return 0;
}
