#include <cstddef>
#include <cstdlib>
#include <stdexcept>

template<typename T, std::size_t N>
class MemoryListPool
{
public:
    MemoryListPool() : free_(nullptr)
    {
        pool_ = static_cast<char *>(malloc(N * sizeof(T)));

        if (!pool_)
        {
            throw std::bad_alloc();
        }

        for (std::size_t i = 0; i < N; ++i)
        {
            MemoryNode* node = reinterpret_cast<MemoryNode *>(pool_ + i * sizeof(T));
            node->next = free_;
            free_ = node;
        }
    }

    ~MemoryListPool()
    {
        std::free(pool_);
    }

    T* alloc()
    {
        if (free_ == nullptr)
        {
            throw std::logic_error("Memory pool exhausted!");
        }

        MemoryNode* node = free_;
        free_ = free_->next;

        return new (node) T();
    }

    void dealloc(T* ptr)
    {
        if (ptr == nullptr)
        {
            return ;
        }

        MemoryNode* node = reinterpret_cast<MemoryNode *>(ptr);

        ptr->~T();

        node->next = free_;
        free_ = node;
    }

private:
    struct MemoryNode
    {
        MemoryNode* next;
    };

    MemoryNode* free_;
    char* pool_;
};

int main()
{
    MemoryListPool<int, 10> memoryListPool;

    int* ptrA = memoryListPool.alloc();
    int* ptrB = memoryListPool.alloc();

    memoryListPool.dealloc(ptrA);
    memoryListPool.dealloc(ptrB);
}