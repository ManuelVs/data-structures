#include "gtest/gtest.h"

template<class T>
void construct_test(){
    T heap;
}

template<class T>
void construct_copy_test(){
    T heap1;
    heap1.push(1);
    heap1.push(2);
    heap1.push(3);
    heap1.push(4);
    heap1.pop();

    T heap2(heap1);

    EXPECT_EQ(heap1.size(), heap2.size());
}

template<class T>
void construct_move_test(){
    T heap1;
    heap1.push(1);
    heap1.push(2);
    heap1.push(3);
    heap1.push(4);
    heap1.pop();

    T heap2(std::move(heap1));

    EXPECT_EQ(heap1.size(), 0);
    EXPECT_EQ(heap2.size(), 3);
}

template<class T>
void push1_test(){
    T heap;
	heap.push(1);
	heap.push(8);
	heap.push(6);
	heap.push(5);
	heap.push(3);
	heap.push(7);
	heap.push(4);

	EXPECT_EQ(heap.top(), 8); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();
	EXPECT_EQ(heap.top(), 6); heap.pop();
	EXPECT_EQ(heap.top(), 5); heap.pop();
	EXPECT_EQ(heap.top(), 4); heap.pop();
	EXPECT_EQ(heap.top(), 3); heap.pop();
	EXPECT_EQ(heap.top(), 1); heap.pop();
}

template<class T>
void push2_test(){
    T heap;
	heap.push(10);
	heap.push(1);
	heap.push(8);
	heap.push(2);
	heap.push(7);
	heap.push(11);
	heap.push(10);

	EXPECT_EQ(heap.top(), 11); heap.pop();
	EXPECT_EQ(heap.top(), 10); heap.pop();
	EXPECT_EQ(heap.top(), 10); heap.pop();
	EXPECT_EQ(heap.top(), 8); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();
	EXPECT_EQ(heap.top(), 2); heap.pop();
	EXPECT_EQ(heap.top(), 1); heap.pop();
}

template<class T>
void emplace1_reverse_test(){
    T heap;
    heap.emplace(1);
    heap.emplace(6);
    heap.emplace(3);
	heap.emplace(2);
    heap.emplace(7);
	heap.emplace(4);
	heap.emplace(5);

	EXPECT_EQ(heap.top(), 1); heap.pop();
	EXPECT_EQ(heap.top(), 2); heap.pop();
	EXPECT_EQ(heap.top(), 3); heap.pop();
	EXPECT_EQ(heap.top(), 4); heap.pop();
	EXPECT_EQ(heap.top(), 5); heap.pop();
	EXPECT_EQ(heap.top(), 6); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();

    EXPECT_EQ(heap.size(), 0);
}

#include <random>
template<class T>
void big_push1_test(){
    T heap;
	std::default_random_engine random(1227992885);
	size_t size = 1000000;

	for (std::size_t i = 0; i < size; ++i) {
		heap.push(random());
	}

	EXPECT_EQ(heap.size(), size);

	int actual = heap.top(); heap.pop();
	for (std::size_t i = 0; i < size - 1; ++i) {
		EXPECT_LE(heap.top(), actual);
		actual = heap.top(); heap.pop();
	}

	EXPECT_TRUE(heap.empty());
}

template<class T>
void big_push2_test(){
    T heap;
	std::default_random_engine random(1227992885);
	size_t size = 1000000;
    size_t num_pops = 10;

	for (std::size_t i = 0; i < size; ++i) {
		heap.push(random());
	}

	EXPECT_EQ(heap.size(), size);

    while(!heap.empty()){
        heap.push(random());
        int actual = heap.top(); heap.pop();
        size_t i = 0;
        while(!heap.empty() && i < num_pops){
			EXPECT_LE(heap.top(), actual);
			actual = heap.top(); heap.pop();
            ++i;
        }
    }

	EXPECT_TRUE(heap.empty()); // ...
}

template<class T>
void raises_exception_when_empty_test(){
    T heap;

	EXPECT_THROW(heap.pop(), std::domain_error);
}

template<class T>
void merge_test(){
	T heap1;
	T heap2;

	heap1.push(1);
	heap1.push(2);
	heap1.push(3);

	heap2.push(4);
	heap2.push(5);
	heap2.push(6);

	EXPECT_EQ(heap1.size(), 3);
	EXPECT_EQ(heap2.size(), 3);

	heap1.merge(heap2);

	EXPECT_EQ(heap1.size(), 6); // Heap1 has acquired all elements
	EXPECT_EQ(heap2.size(), 0); // Heap2 is now empty
	EXPECT_TRUE(heap2.empty());

	EXPECT_EQ(heap1.top(), 6); heap1.pop(); // Check that heap1 is well constructed

	heap2.push(1); // Check that heap2 is usable
	EXPECT_EQ(heap2.top(), 1); heap2.pop();
	EXPECT_TRUE(heap2.empty());
}

template<class T>
void big_merge_test(){
	T heap1, heap2;
	std::default_random_engine random(1227992885);
	size_t size = 100000;

	for (std::size_t i = 0; i < size; ++i) {
		heap1.push(random());
		heap2.push(random());
	}

	heap1.merge(heap2);
	EXPECT_EQ(heap1.size(), 2 * size);
	EXPECT_EQ(heap2.size(), 0);

	int actual = heap1.top(); heap1.pop();
	for (std::size_t i = 0; i < 2 * size - 1; ++i) {
		EXPECT_LE(heap1.top(), actual);
		actual = heap1.top(); heap1.pop();
	}

	EXPECT_TRUE(heap1.empty());
}
