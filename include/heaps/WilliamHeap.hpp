#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>

/**
 * @brief WilliamHeap or BinaryHeap <br>
 * Implements a heap using a vector <br>
 * Element priority can be defined using the Comparator <br>
 * Element allocations are made using the Allocator <br>
 * This implementation is famous due to the Heapsort algorithm, whose base is based on
 * the WilliamHeap
 * @sa https://en.wikipedia.org/wiki/Binary_heap
 * @tparam T Elements' type this heap will store
 * @tparam Comparator Comparator between keys
 * @tparam Allocator Keys' allocator
 */
template<typename T, class Comparator = std::greater<T>, class Allocator = std::allocator<T>>
class WilliamHeap {
protected:
	/**
	 * @brief Uses the first swap found by the ADL algorithm. <br>
	 * This should find the user-defined swap before the generic std::swap, that makes
	 * three moves
	 * @tparam U Type
	 * @param left Left object
	 * @param right Right object
	 */
	template<typename U>
	void p_adl_swap(U& left, U& right) {
		using std::swap;
		swap(left, right); // ADL swap
	}

	using alloc_key_traits = std::allocator_traits<Allocator>;

	Comparator comparator;
	Allocator alloc_key;

	T* vector;
	std::size_t capacity;
	std::size_t num_elems;
	
	void p_default() {
		this->vector = nullptr;
		this->capacity = 0;
		this->num_elems = 0;
	}

	void p_new(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_default();
		this->comparator = c;
		this->alloc_key = alloc_key;
	}

	void p_delete() {
		for (std::size_t i = 0; i < this->num_elems; ++i) {
			alloc_key_traits::destroy(alloc_key, &vector[i]);
		}
		alloc_key_traits::deallocate(alloc_key, this->vector, this->capacity);
		
		this->p_default();
	}

	void p_copy(WilliamHeap const& other) {
		this->comparator = other.comparator;
		this->capacity = other.capacity;
		this->num_elems = other.num_elems;

		this->vector = alloc_key_traits::allocate(alloc_key, this->capacity, this->vector);
		for (std::size_t i = 0; i < this->num_elems; ++i) {
			alloc_key_traits::construct(alloc_key, &this->vector[i], other.vector[i]);
		}
	}

	void p_move(WilliamHeap& other) {
		this->comparator = other.comparator;
		this->capacity = other.capacity;
		this->num_elems = other.num_elems;
		this->vector = other.vector;

		other.p_default();
	}

	void p_swap(WilliamHeap& other) {
		p_adl_swap(this->comparator, other.comparator);
		p_adl_swap(this->vector, other.vector);
		p_adl_swap(this->capacity, other.capacity);
		p_adl_swap(this->num_elems, other.num_elems);
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
		
		new_vector = alloc_key_traits::allocate(alloc_key, new_capacity, this->vector);
		if (new_vector != this->vector) {
			for (std::size_t i = 0; i < this->num_elems; ++i)
				alloc_key_traits::construct(alloc_key, &new_vector[i], std::move(this->vector[i]));

			alloc_key_traits::deallocate(alloc_key, this->vector, this->capacity);
			this->capacity = new_capacity;
			this->vector = new_vector;
		}
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
		if (greater_child + 1 < num_elems && comparator(vector[greater_child + 1], vector[greater_child])) {
			//we must choose the greatest of the childs
			greater_child += 1;
		}

		return greater_child;
	}

	void p_float(std::size_t k) {
		// We must float 'node k' until his father has more priority
		while (k != 0 && this->comparator(this->vector[k], this->vector[p_father(k)])) {
			p_adl_swap(vector[p_father(k)], vector[k]);
			k = p_father(k);
		}
	}

	void p_sink(std::size_t j) {
		std::size_t greater_child = p_greater_child(j);

		//We must sink 'node j' until his childs have less priority
		bool j_must_sink = greater_child < this->num_elems && comparator(vector[greater_child], vector[j]);
		while (j_must_sink) {
			p_adl_swap(vector[j], vector[greater_child]);
			j = greater_child;

			greater_child = p_greater_child(j);
			j_must_sink = greater_child < this->num_elems && comparator(vector[greater_child], vector[j]);
		}
	}

	template <class... Args>
	void p_emplace(Args&&... args) {
		if (num_elems == capacity) p_grow();
		alloc_key_traits::construct(alloc_key, &this->vector[num_elems], std::forward<Args>(args)...);
		p_float(num_elems);
		++num_elems;
	}

	T p_pop() {
		T key = std::move(vector[0]);
		vector[0] = std::move(vector[--num_elems]);
		p_sink(0);
		return key;
	}

public:

	/**
	 * @brief Construct a new William Heap object
	 * 
	 * @param c Comparator to be used
	 * @param alloc Allocator to be used
	 */
	WilliamHeap(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_new(c, alloc);
	}

	/**
	 * @brief Construct a new William Heap object by copy
	 * Time complexity: O(other.size())
	 * @param other The other heap to copy
	 */
	WilliamHeap(WilliamHeap const& other) {
		this->p_copy(other);
	}

	/**
	 * @brief Construct a new William Heap object by move
	 * Time complexity: O(1)
	 * @param other The other heap to move
	 */
	WilliamHeap(WilliamHeap&& other) noexcept {
		this->p_move(other);
	}

	/**
	 * @brief Destroy the William Heap object
	 * Time complexity: O(this->size())
	 */
	~WilliamHeap() {
		this->p_delete();
	}

	/**
	 * @brief Copy assignment operator
	 * Time complexity: O(other.size() + this->size())
	 *  - Because delete operator costs O(n)
	 * @param other The other heap to copy
	 * @return WilliamHeap& Reference to *this
	 */
	WilliamHeap& operator=(WilliamHeap const& other) {
		if (this != &other) {
			this->p_delete();
			this->p_copy(other);
		}
		return *this;
	}

	/**
	 * @brief Move assignment operator
	 * Time complexity: O(this->size())
	 *  - Because delete operator costs O(n)
	 * @param other The other heap to move
	 * @return WilliamHeap& Reference to *this
	 */
	WilliamHeap& operator=(WilliamHeap&& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_move(other);
		}
		return *this;
	}

	/**
	 * @brief Swap the two heaps
	 * Time complexity: O(1)
	 * @param other The other heap to swap with
	 */
	void swap(WilliamHeap& other) {
		this->p_swap(other);
	}

	/**
	 * @brief Push 'elem' to the heap, copying it
	 * Time complexity: O(log(this->size) + O(elem copy))
	 * @param elem The elem to be pushed
	 */
	void push(T const& elem) {
		this->p_emplace(elem);
	}

	/**
	 * @brief Push 'elem' to the heap, moving it
	 * Time complexity: O(log(this->size) + O(elem move))
	 * @param elem lvalue reference to the elem to be pushed
	 */
	void push(T&& elem) {
		this->p_emplace(std::move(elem));
	}

	/**
	 * @brief Construct element directly on the heap
	 * Time complexity: O(log(this->size) + O(elem creation))
	 * @tparam Args Template parameter list, type parameters of the object constructor
	 * @param args Parameters list of the object constructor
	 */
	template <class... Args>
	void emplace(Args&&... args) {
		this->p_emplace(std::forward<Args>(args)...);
	}

	/**
	 * @brief Pop element from the heap
	 * Time complexity: O(log(this->size))
	 * @return T Element popped
	 */
	T pop() {
		if (empty()) throw std::domain_error("Empty heap");
		return p_pop();
	}

	/**
	 * @brief Consult top element of the heap
	 * Time complexity: O(1)
	 * @return T const& Reference to the top element
	 */
	T const& top() const {
		if (empty()) throw std::domain_error("Empty heap");
		return vector[0];
	}

	/**
	 * @brief If heap empty
	 * Time complexity: O(1)
	 * @return If heap empty
	 */
	bool empty() const {
		return this->num_elems == 0;
	}

	/**
	 * @brief Number of elements
	 * Time complexity: O(1)
	 * @return std::size_t Number of elements
	 */
	std::size_t size() const {
		return this->num_elems;
	}
};

/**
 * @brief Swaps the two heaps
 * Time complexity: O(1)
 * This implementation is intended to be found by the ADL algorithm
 * @tparam T Same as class documentation
 * @tparam Comparator Same as class documentation
 * @tparam Allocator Same as class documentation
 * @param lhs Left heap
 * @param rhs Right heap
 */
template<typename T, class Comparator, class Allocator>
void swap(WilliamHeap<T, Comparator, Allocator>& lhs, WilliamHeap<T, Comparator, Allocator>& rhs) {
	lhs.swap(rhs);
}
