#include <iostream>
#include <vector>

#include "heaps/WilliamHeap.hpp"
#include "heaps/BinomialHeap.hpp"
#include "heaps/FibonacciHeap.hpp"

#include "trees/AVLTree.hpp"
#include "trees/RedBlackTree.hpp"

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
	std::cout << "Welcome!" << std::endl;

	BinomialHeap<int> heap = create_heap<BinomialHeap<int>>(5);
	print_heap(heap);

	AVLTree<int> tree;
	tree.insert(1);
	tree.insert(2);
	tree.insert(3);

	std::cout << "True : " << tree.contains(1) << std::endl;
	tree.erase(1);
	std::cout << "False: " << tree.contains(1) << std::endl;

	return 0;
}
