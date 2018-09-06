#include "gtest/gtest.h"
#include "HeapTest.hpp"

#include "heaps/WilliamHeap.hpp"

TEST(WilliamHeapTest, Construct) {
	construct_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, ConstructCopy) {
	construct_copy_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, ConstructMove) {
	construct_move_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, Push1) {
	push1_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, Push2) {
	push2_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, Emplace1) {
	emplace1_reverse_test<WilliamHeap<int, std::less<int>>>();
}

TEST(WilliamHeapTest, BigPush1) {
	big_push1_test<WilliamHeap<int>>();
}

TEST(WilliamHeapTest, BigPush2) {
	big_push2_test<WilliamHeap<int>>();
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
	raises_exception_when_empty_test<WilliamHeap<int>>();
}
