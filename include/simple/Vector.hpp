#pragma once

#include <memory>

template<typename T, class Allocator = std::allocator<T>>
class Vector {
private:
	Allocator alloc;

	T* vector;
	std::size_t capacity;
	std::size_t num_elems;

	void p_grow() {
		T* new_vector;
		std::size_t new_capacity;
		if (this->capacity == 0) {
			new_capacity = 1;
		}
		else {
			new_capacity = this->capacity * 2;
		}

		new_vector = this->alloc.allocate(new_capacity, this->vector);
		if (new_vector != this->vector) {
			for (std::size_t i = 0; i < this->num_elems; ++i)
				new_vector[i] = std::move(this->vector[i]);

			this->alloc.deallocate(this->vector, this->capacity);
			this->capacity = new_capacity;
			this->vector = new_vector;
		}
	}

public:

	Vector() {
		this->alloc = Allocator();
		this->vector = nullptr;
		this->num_elems = 0;
		this->capacity = 0;
	}

	template <class... Args>
	void emplace(Args&&... args) {
		if (num_elems == capacity) p_grow();
		this->alloc.construct(&this->vector[num_elems], args...);
		++num_elems;
	}
};