#include "gtest/gtest.h"

#include "heaps/FibonacciHeap.hpp"

TEST(FibonacciHeapTest, Push1) {
	FibonacciHeap<int> heap;
	heap.push(1);
	heap.push(8);
	heap.push(6);
	heap.push(5);
	heap.push(3);
	heap.push(7);
	heap.push(4);

	EXPECT_EQ(heap.pop(), 8);
	EXPECT_EQ(heap.pop(), 7);
	EXPECT_EQ(heap.pop(), 6);
	EXPECT_EQ(heap.pop(), 5);
	EXPECT_EQ(heap.pop(), 4);
	EXPECT_EQ(heap.pop(), 3);
	EXPECT_EQ(heap.pop(), 1);
}

TEST(FibonacciHeapTest, Push2) {
	FibonacciHeap<int> heap;
	heap.push(10);
	heap.push(1);
	heap.push(8);
	heap.push(2);
	heap.push(7);
	heap.push(11);
	heap.push(10);

	EXPECT_EQ(heap.pop(), 11);
	EXPECT_EQ(heap.pop(), 10);
	EXPECT_EQ(heap.pop(), 10);
	EXPECT_EQ(heap.pop(), 8);
	EXPECT_EQ(heap.pop(), 7);
	EXPECT_EQ(heap.pop(), 2);
	EXPECT_EQ(heap.pop(), 1);
}

TEST(FibonacciHeapTest, Emplace1) {
	FibonacciHeap<int, std::less<int>> heap;
	heap.emplace(1);
	heap.emplace(2);
	heap.emplace(3);
	heap.emplace(4);
	heap.emplace(5);
	heap.emplace(6);
	heap.emplace(7);

	EXPECT_EQ(heap.pop(), 1);
	EXPECT_EQ(heap.pop(), 2);
	EXPECT_EQ(heap.pop(), 3);
	EXPECT_EQ(heap.pop(), 4);
	EXPECT_EQ(heap.pop(), 5);
	EXPECT_EQ(heap.pop(), 6);
	EXPECT_EQ(heap.pop(), 7);
}

#include <random>
TEST(FibonacciHeapTest, RandomTest1) {
	FibonacciHeap<int> heap;
	std::default_random_engine random;
	size_t size = 1000000;

	for (std::size_t i = 0; i < size; ++i) {
		heap.push(i);
	}

	EXPECT_EQ(heap.size(), size);

	int actual = heap.pop();
	for (std::size_t i = 0; i < size - 1; ++i) {
		EXPECT_LE(heap.top(), actual);
		actual = heap.pop();
	}

	EXPECT_TRUE(heap.empty());
}

TEST(FibonacciHeapTest, NestedHeap) {
	FibonacciHeap<int> h0;
	FibonacciHeap<int> h1;
	h1.push(1);
	FibonacciHeap<int> h2;
	h2.push(2);
	h2.push(3);

	FibonacciHeap<FibonacciHeap<int>, std::function<bool(const FibonacciHeap<int>&, const FibonacciHeap<int>&)>> heap(
		[](const FibonacciHeap<int>& left, const FibonacciHeap<int>& right) {
		return left.size() < right.size();
		}
	);

	heap.push(h0);
	heap.push(h1);
	heap.push(h2);

	EXPECT_EQ(heap.pop().size(), 0);
	EXPECT_EQ(heap.pop().size(), 1);
	EXPECT_EQ(heap.pop().size(), 2);
}

TEST(FibonacciHeapTest, RaisesExceptionWhenEmpty) {
	FibonacciHeap<int> heap;

	EXPECT_THROW(heap.pop(), std::domain_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
