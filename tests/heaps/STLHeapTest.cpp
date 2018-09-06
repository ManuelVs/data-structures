#include "gtest/gtest.h"
#include "HeapTest.hpp"

#include <queue>

TEST(STLHeapTest, Construct) {
	construct_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, ConstructCopy) {
	construct_copy_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, ConstructMove) {
	construct_move_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, Push1) {
	push1_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, Push2) {
	push2_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, Emplace1) {
	emplace1_reverse_test<std::priority_queue<int, std::vector<int>, std::greater<int>>>();
}

TEST(STLHeapTest, BigPush1) {
	big_push1_test<std::priority_queue<int>>();
}

TEST(STLHeapTest, BigPush2) {
	big_push2_test<std::priority_queue<int>>();
}

#include <functional>
TEST(STLHeapTest, NestedHeap) {
	std::priority_queue<int> h0;
	std::priority_queue<int> h1;
	h1.push(1);
	std::priority_queue<int> h2;
	h2.push(2);
	h2.push(3);

	std::priority_queue<
		std::priority_queue<int>,
		std::vector<std::priority_queue<int>>,
		std::function<bool(const std::priority_queue<int>&, const std::priority_queue<int>&)>>
		heap(
		[](const std::priority_queue<int>& left, const std::priority_queue<int>& right) {
			return left.size() > right.size();
		}
	);
	
	heap.push(h0);
	heap.push(h1);
	heap.push(h2);

	EXPECT_EQ(heap.top().size(), 0); heap.pop();
	EXPECT_EQ(heap.top().size(), 1); heap.pop();
	EXPECT_EQ(heap.top().size(), 2); heap.pop();
}
