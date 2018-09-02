#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>

template<typename T, class Comparator = std::greater<T>, class Allocator = std::allocator<T>>
class WilliamHeap {
protected:
	Comparator greater;
	Allocator alloc;

	T* vector;
	std::size_t capacity;
	std::size_t num_elems;
	
	void p_default() {
		this->vector = nullptr;
		this->capacity = 0;
		this->num_elems = 0;
	}

	void p_new(Comparator c = Comparator(), Allocator alloc = Allocator()) {
		this->p_default();
		this->greater = c;
		this->alloc = alloc;
	}

	void p_delete() {
		for (std::size_t i = 0; i < this->num_elems; ++i) {
			this->alloc.destroy(&vector[i]);
		}
		this->alloc.deallocate(this->vector, this->capacity);
		
		this->p_default();
	}

	void p_copy(WilliamHeap const& other) {
		this->greater = other.greater;
		this->capacity = other.capacity;
		this->num_elems = other.num_elems;

		this->vector = this->alloc.allocate(this->capacity, this->vector);
		for (std::size_t i = 0; i < this->num_elems; ++i) {
			this->alloc.construct(&this->vector[i], other.vector[i]);
		}
	}

	void p_move(WilliamHeap& other) {
		this->greater = other.greater;
		this->capacity = other.capacity;
		this->num_elems = other.num_elems;
		this->vector = other.vector;

		other.p_default();
	}

	void p_swap(WilliamHeap& other) {
		std::swap(this->greater, other.greater);
		std::swap(this->vector, other.vector);
		std::swap(this->capacity, other.capacity);
		std::swap(this->num_elems, other.num_elems);
	}

	void p_grow() {
		T* new_vector;
		std::size_t new_capacity;
		if (this->capacity == 0) {
			new_capacity = 10;
		}
		else {
			new_capacity = this->capacity * 2;
		}
		
		new_vector = this->alloc.allocate(new_capacity, this->vector);
		if (new_vector != this->vector) {
			for (std::size_t i = 0; i < this->num_elems; ++i)
				this->alloc.construct(&new_vector[i], std::move(this->vector[i]));

			this->alloc.deallocate(this->vector, this->capacity);
			this->capacity = new_capacity;
			this->vector = new_vector;
		}
	}

	template<typename U>
	void p_adl_swap(U& left, U& right) {
		using std::swap;
		swap(left, right); // ADL swap
	}

	inline std::size_t p_father(std::size_t k) const {
		return k % 2 == 1 ?
			k / 2 : 
			k / 2 - 1;
	}

	inline std::size_t p_left_child(std::size_t k) const {
		return k * 2 + 1;
	}

	std::size_t p_greater_child(std::size_t j) const {
		std::size_t greater_child = p_left_child(j); //left_child of 'j'
		if (greater_child + 1 < num_elems && greater(vector[greater_child + 1], vector[greater_child])) {
			//we must choose the greatest of the childs
			greater_child += 1;
		}

		return greater_child;
	}

	void p_float(std::size_t k) {
		// We must float 'node k' until his father has more priority
		while (k != 0 && this->greater(this->vector[k], this->vector[p_father(k)])) {
			p_adl_swap(vector[p_father(k)], vector[k]);
			k = p_father(k);
		}
	}

	void p_sink(std::size_t j) {
		std::size_t greater_child = p_greater_child(j);

		//We must sink 'node j' until his childs have less priority
		bool j_must_sink = greater_child < this->num_elems && greater(vector[greater_child], vector[j]);
		while (j_must_sink) {
			p_adl_swap(vector[j], vector[greater_child]);
			j = greater_child;

			greater_child = p_greater_child(j);
			j_must_sink = greater_child < this->num_elems && greater(vector[greater_child], vector[j]);
		}
	}

	template <class... Args>
	void p_emplace(Args&&... args) {
		if (num_elems == capacity) p_grow();
		this->alloc.construct(&this->vector[num_elems], std::forward<Args>(args)...);
		p_float(num_elems);
		++num_elems;
	}

	T p_pop() {
		if (!empty()) {
			T key = std::move(vector[0]);
			vector[0] = std::move(vector[--num_elems]);
			p_sink(0);
			return key;
		}
		else throw std::domain_error("Empty heap");
	}

public:

	//Default constructor
	//Time complexity: O(1)
	WilliamHeap(Comparator c = Comparator(), Allocator alloc = Allocator()) {
		this->p_new(c, alloc);
	}

	//Copy constructor
	//Time complexity: O(other.size())
	WilliamHeap(WilliamHeap const& other) {
		this->p_copy(other);
	}

	//Move constructor
	//Time complexity: O(1)
	WilliamHeap(WilliamHeap&& other) noexcept {
		this->p_move(other);
	}

	//Destructor
	//Time complexity: O(this->size())
	~WilliamHeap() {
		this->p_delete();
	}

	//Copy assignment operator
	//Time complexity: O(other.size() + this->size())
	// - Because delete operator costs O(n)
	WilliamHeap& operator=(WilliamHeap const& other) {
		if (this != &other) {
			this->p_delete();
			this->p_copy(other);
		}
		return *this;
	}

	//Move assignment operator
	//Time complexity: O(this->size())
	// - Because delete operator costs O(n)
	WilliamHeap& operator=(WilliamHeap&& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_move(other);
		}
		return *this;
	}

	//Swap the two heaps
	//Time complexity: O(1)
	void swap(WilliamHeap& other) {
		this->p_swap(other);
	}

	//Push 'elem' to the heap, copying it
	//Time complexity: O(log(this->size) + O(elem copy))
	void push(T const& elem) {
		this->p_emplace(elem);
	}

	//Push 'elem' to the heap, moving it
	//Time complexity: O(log(this->size) + O(elem move))
	void push(T&& elem) {
		this->p_emplace(std::move(elem));
	}

	//Push 'elem' to the heap, creating it
	//Time complexity: O(log(this->size) + O(elem creation))
	template <class... Args>
	void emplace(Args&&... args) {
		this->p_emplace(args...);
	}

	//Pop element from the heap
	//Time complexity: O(log(this->size))
	T pop() {
		return p_pop();
	}

	//Consult top element of the heap
	//Time complexity: O(1)
	T const& top() const {
		return vector[0];
	}

	//If heap empty.
	//Time complexity: O(1)
	bool empty() const {
		return this->num_elems == 0;
	}

	//Number of elements.
	//Time complexity: O(1)
	std::size_t size() const {
		return this->num_elems;
	}
};

// Time complexity: O(1)
// ADL finds this swap
template<typename T, class Comparator, class Allocator>
void swap(WilliamHeap<T, Comparator, Allocator>& lhs, WilliamHeap<T, Comparator, Allocator>& rhs) {
	lhs.swap(rhs);
}
