#include "gtest/gtest.h"

#include "heaps/WilliamHeap.hpp"

TEST(WilliamHeapTest, Push1) {
	WilliamHeap<int> heap;
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

TEST(WilliamHeapTest, Push2) {
	WilliamHeap<int> heap;
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

TEST(WilliamHeapTest, Emplace1) {
	WilliamHeap<int, std::less<int>> heap;
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
TEST(WilliamHeapTest, RandomTest1) {
	WilliamHeap<int> heap;
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

TEST(WilliamHeapTest, NestedHeap) {
	WilliamHeap<int> h0;
	WilliamHeap<int> h1;
	h1.push(1);
	WilliamHeap<int> h2;
	h2.push(2);
	h2.push(3);

	WilliamHeap<WilliamHeap<int>, std::function<bool(const WilliamHeap<int>&, const WilliamHeap<int>&)>> heap(
		[](const WilliamHeap<int>& left, const WilliamHeap<int>& right) {
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

TEST(WilliamHeapTest, RaisesExceptionWhenEmpty) {
	WilliamHeap<int> heap;

	EXPECT_THROW(heap.pop(), std::domain_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}