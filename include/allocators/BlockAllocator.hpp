#pragma once

#include <cstring>
#include <limits>
#include <cmath>

class LinkedVector {
  private:
    using alloc = std::allocator<std::size_t>;
    using alloc_traits = std::allocator_traits<alloc>;

    const std::size_t no_block = std::numeric_limits<std::size_t>::max();

    alloc allocator;

    std::size_t* vector;
    std::size_t capacity; //equal to size

    std::size_t first_available;
    std::size_t last_available;

    void p_default() {
        vector = nullptr;
        capacity = 0;
        first_available = no_block;
        last_available = no_block;
    }

    void p_copy(LinkedVector const& other){
        this->capacity = other.capacity;
        this->first_available = other.first_available;
        this->last_available = other.last_available;

        this->vector = alloc_traits::allocate(allocator, capacity);
        memcpy(this->vector, other.vector, this->capacity * sizeof(std::size_t));
    }

    void p_move(LinkedVector& other){
        this->vector = other.vector;
        this->capacity = other.capacity;
        this->first_available = other.first_available;
        this->last_available = other.last_available;
        
        other.p_default();
    }

    void p_delete(){
        alloc_traits::deallocate(allocator, vector, capacity);
    }

    void p_grow(std::size_t min_num_blocks){
        std::size_t* newvector = alloc_traits::allocate(allocator, min_num_blocks, vector);
        if(vector != newvector){
            memcpy(newvector, vector, this->capacity * sizeof(std::size_t));
            alloc_traits::deallocate(allocator, vector, capacity);
        }

        for(std::size_t i = this->capacity; i < min_num_blocks; ++i){
            newvector[i] = i + 1;
        }
        newvector[min_num_blocks - 1] = no_block;
       
        this->vector = newvector;
        this->capacity = min_num_blocks;
    }

  public:
    LinkedVector(){
        this->p_default();
    }

    ~LinkedVector(){
        this->p_delete();
    }

    LinkedVector(LinkedVector const& other){
        this->p_copy(other);
    }

    LinkedVector(LinkedVector&& other){
        this->p_move(other);
    }

    LinkedVector& operator=(LinkedVector const& other){
        if(this != &other){
            this->p_delete();
            this->p_copy(other);
        }
        return *this;
    }

    LinkedVector& operator=(LinkedVector&& other){
        if(this != &other){
            this->p_delete();
            this->p_move(other);
        }
        return *this;
    }


    void grow_and_add(std::size_t min_num_blocks){
        if(this->capacity < min_num_blocks){
            std::size_t backup = this->capacity;
            this->p_grow(min_num_blocks);

            if(first_available == no_block){
                first_available = backup;
            }

            if(last_available != no_block){
                vector[last_available] = backup;
            }
            last_available = capacity - 1;
        }
    }

    void add_available(std::size_t block){
        vector[block] = first_available;
        first_available = block;
    }

    std::size_t get_available(){
        std::size_t available = first_available;
        first_available = vector[available];
        vector[available] = no_block;
        if(first_available == no_block){
            last_available = no_block;
        }

        return available;
    }

    bool empty(){
        return first_available == no_block;
    }
};


template<typename T>
class BlockAllocator
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    using Alloc_doublepointer = std::allocator<T*>;
    using Alloc_pointer = std::allocator<T>;

    using alloc_doublepointer_traits = std::allocator_traits<Alloc_doublepointer>;
    using alloc_pointer_traits = std::allocator_traits<Alloc_pointer>;

    Alloc_doublepointer alloc_doublepointer;
    Alloc_pointer alloc_pointer;

    T** blocklists;
    std::size_t blocklists_capacity;
    std::size_t blocklists_size;
    std::size_t total_blocks;

    const std::size_t first_list_capacity = 1; // This must not change (at the moment)
    const std::size_t factor_list_capacity = 2; // This must not change (at the moment)

    LinkedVector available_blocks;

    void p_default(){
        blocklists = nullptr;
        blocklists_capacity = 0;
        blocklists_size = 0;
        total_blocks = 0;
    }

    void p_move(BlockAllocator& other){
        blocklists = other.blocklists;
        blocklists_capacity = other.blocklists_capacity;
        blocklists_size = other.blocklists_size;
        total_blocks = other.total_blocks;

        available_blocks = std::move(other.available_blocks);
        other.p_default();
    }

    void p_delete(){
        std::size_t act = first_list_capacity;
        for(std::size_t i = 0; i < blocklists_size; ++i){
            T* actual_list = blocklists[i];
            alloc_pointer_traits::deallocate(alloc_pointer, actual_list, act);
            act *= 2;
        }
        alloc_doublepointer_traits::deallocate(
            alloc_doublepointer,
            blocklists,
            blocklists_capacity);

        this->p_default();
    }

    /**
     * @brief Reserves more memory. This updates blocklists, blocklists_capacity,
     * blocklists_size if necessary.
     */
    void p_grow_blocklists(){
        std::size_t blocklists_newcapacity;
        if(blocklists_capacity == 0) {
            blocklists_newcapacity = 1;
        }
        else {
            blocklists_newcapacity = 2 * blocklists_capacity;
        }

        T** blocklists_new = alloc_doublepointer_traits::allocate(
            alloc_doublepointer,
            blocklists_newcapacity,
            blocklists);
        
        if(blocklists != blocklists_new){
            for(std::size_t i = 0; i < blocklists_size; ++i){
                blocklists_new[i] = blocklists[i];
            }
            //memcpy(blocklists_new, blocklists, blocklists_size); this doesnt work (why?)
            alloc_doublepointer_traits::deallocate(alloc_doublepointer, blocklists, blocklists_size);
        }

        blocklists = blocklists_new;
        blocklists_capacity = blocklists_newcapacity;
    }

    inline std::size_t get_list_capacity(std::size_t index){
        return std::pow(2, index); //this must be improved...
    }

    void p_new_blocklist(){
        if(blocklists_size == blocklists_capacity) {
            p_grow_blocklists();
        }
        std::size_t actual_capacity = get_list_capacity(blocklists_size);
        blocklists[blocklists_size] = alloc_pointer_traits::allocate(
            alloc_pointer,
            actual_capacity);
        
        available_blocks.grow_and_add(total_blocks + actual_capacity);
        blocklists_size += 1;
        total_blocks += actual_capacity;
    }

    std::size_t p_get_available_block(){
        if(available_blocks.empty()) {
            this->p_new_blocklist();
        }

        std::size_t available_block = available_blocks.get_available();
        return available_block;
    }

    /**
     * @brief Search wich blocklist owns 'p'. If there is no list that owns 'p' results in
     * undefined behaviour, because one call to allocate precedes to one call to
     * deallocate.
     * @param p Pointer to search
     * @return std::size_t index of the list
     */
    std::size_t p_search_list(T* p){
        std::size_t i = 0;
        std::size_t act = first_list_capacity;
        while(!(&blocklists[i][0] <= p && p <= &blocklists[i][act - 1])){
            ++i;
            act *= 2;
        }
        return i;
    }

    std::size_t p_search_index(std::size_t list_index, T* p) {
        std::size_t diff = (std::size_t) (p - &blocklists[list_index][0]);
        return diff;
    }

    T* p_allocate() {
        std::size_t available_block = this->p_get_available_block();

        std::size_t blocklist_index = std::log2(available_block + 1); //this must be improved...
        std::size_t list_index = available_block - std::pow(2, blocklist_index) + 1; //this must be improved...

        return &blocklists[blocklist_index][list_index];
    }

    void p_deallocate(T* p){
        std::size_t blocklist_index = this->p_search_list(p);
        std::size_t list_index = this->p_search_index(blocklist_index, p);

        std::size_t available_block = std::pow(2, blocklist_index) + list_index - 1;
        available_blocks.add_available(available_block);
    }

  public:
    BlockAllocator() {
        this->p_default();
    }

    BlockAllocator(BlockAllocator const& other){
        this->p_default();
    }

    BlockAllocator(BlockAllocator&& other){
        this->p_move(other);
    }

    ~BlockAllocator(){
        this->p_delete();
    }

    BlockAllocator& operator=(BlockAllocator const& other){
        if(this != &other){
            this->p_delete();
            this->p_default();
        }
        return *this;
    }

    BlockAllocator& operator=(BlockAllocator&& other){
        if(this != &other){
            this->p_delete();
            this->p_move(other);
        }
        return *this;
    }

    /**
     * @brief Allocates n * sizeof(T) bytes of uninitialized storage
     * @param n number of blocks
     * @return T* Pointer to the first element
     */
    T* allocate(std::size_t n) {
        if(n == 1){
            return this->p_allocate();
        }
        else{
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
	}

    /**
     * @brief Deallocates the storage referenced by the pointer p, which must be a pointer
     * obtained by an earlier call to allocate().
     * The argument n must be equal to the first argument of the call to allocate() that
     * originally produced p; otherwise, the behavior is undefined.
     * @param p pointer obtained by allocate
     * @param n size given to allocate
     */
	void deallocate(T *p, std::size_t n) {
        if(n == 1){
            this->p_deallocate(p);
        }
        else{
            ::delete(p);
        }
	}
};
