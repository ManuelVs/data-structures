#include <iostream>
#include <vector>

#include "heaps/WilliamHeap.hpp"
#include "heaps/BinomialHeap.hpp"
#include "heaps/FibonacciHeap.hpp"

template<typename T>
T create_heap(std::size_t size){
	T heap;
	for(std::size_t i = 0; i < size; ++i){
		heap.push(i);
	}

	return heap;
}

template<typename T>
void print_heap(T& heap){
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
	std::cout << "Welcome!" << '\n';
	return 0;
}
