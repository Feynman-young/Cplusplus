#include <cstddef>
#include <iostream>
#include <stdexcept>

template <typename T, std::size_t N> class MemoryListPool
{
  public:
    MemoryListPool()
    {
        pool_ = (MemNode *)std::malloc(N * sizeof(MemNode));

        free_ = pool_;

        for (int i = 0; i < N - 1; ++i)
        {
            (free_ + i)->next_ = free_ + i + 1;
        }

        (free_ + N - 1)->next_ = nullptr;
    }

    ~MemoryListPool()
    {
        std::free(pool_);
    }

    T *alloc()
    {
        if (free_ == nullptr)
        {
            throw std::logic_error("Memory pool exhausted!");
        }

        T *ret = (T *)free_;
        free_ = free_->next_;

        return ret;
    }

    void deAlloc(T *ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        ptr->~T();

        MemNode *node = (MemNode *)ptr;

        node->next_ = free_;
        free_ = node;
    }

  private:
    union MemNode {
        MemNode *next_;
        char data_[sizeof(T)];
    };

    MemNode *free_;
    MemNode *pool_;
};

enum
{
    ALIGN_ = 8
};
enum
{
    MAX_BYTES_ = 128
};
enum
{
    N_FREE_LIST_ = 16
};

template <bool threads, int inst> class DefaultAllocTemplate
{
    union Obj {
        Obj *freeListLink_;
        char clientData_[1];
    };

  private:
    static char *startFree_;
    static char *endFree_;
    static size_t heapSize_;
    static Obj *volatile freeList_[N_FREE_LIST_];

    static size_t getFreeListIndex(size_t bytes)
    {
        return (bytes + ALIGN_ - 1) / ALIGN_;
    }

    static size_t getRoundUp(size_t bytes)
    {
        return (bytes + (size_t)ALIGN_ - 1) & (~((size_t)ALIGN_ - 1));
    }

    static void *refill(size_t n);

    static char *chunkAlloc(size_t size, int &nObjs);

  public:
    static void *alloc(size_t n);

    static void deAlloc(void *ptr, size_t n);
};

template <bool threads, int inst> char *DefaultAllocTemplate<threads, inst>::startFree_ = 0;

template <bool threads, int inst> char *DefaultAllocTemplate<threads, inst>::endFree_ = 0;

template <bool threads, int inst> size_t DefaultAllocTemplate<threads, inst>::heapSize_ = 0;

template <bool threads, int inst>
typename DefaultAllocTemplate<threads, inst>::Obj
    *volatile DefaultAllocTemplate<threads, inst>::freeList_[N_FREE_LIST_] = {0};

template <bool threads, int inst> void *DefaultAllocTemplate<threads, inst>::alloc(size_t n)
{
    void *ret;

    if (n > MAX_BYTES_)
    {
        ret = std::malloc(n);
    }
    else
    {
        Obj *volatile *pObjPtr = freeList_ + getFreeListIndex(n);
        Obj *ptr = *pObjPtr;

        if (ptr == nullptr)
        {
            ret = refill(getRoundUp(n));
        }
        else
        {
            *pObjPtr = ptr->freeListLink_;
            ret = ptr;
        }
    }

    return ret;
}

template <bool threads, int inst> void DefaultAllocTemplate<threads, inst>::deAlloc(void *ptr, size_t n)
{
    if (n > MAX_BYTES_)
    {
        free(ptr);
    }
    else
    {
        Obj *q = (Obj *)ptr;
        Obj *volatile *pObjPtr = freeList_ + getFreeListIndex(n);
        q->freeListLink_ = *pObjPtr;
        *pObjPtr = q;
    }
}

/**
Refill the free list with new blocks of memory. Allocate 20 objects at once,
return the header and put the remaining objects to the free list.
*/
template <bool threads, int inst> void *DefaultAllocTemplate<threads, inst>::refill(size_t n)
{
    int nObjs = 20;
    char *chunk = chunkAlloc(n, nObjs);

    if (nObjs == 1)
    {
        return chunk;
    }

    Obj *ret = (Obj *)chunk;
    Obj *volatile *pObjPtr = freeList_ + getFreeListIndex(n);
    *pObjPtr = (Obj *)(chunk + n);
    Obj *curr = *pObjPtr;
    Obj *next = nullptr;

    for (int i = 2; i < nObjs; ++i)
    {
        next = (Obj *)(chunk + n * i);
        curr->freeListLink_ = next;
        curr = next;
    }

    curr->freeListLink_ = nullptr;
    return ret;
}

/**
Request memory from the system in bulk.
If the remaining memory size >= the total requested memory size or > at least
one object size, use the current start, move the start pointer forward and
return allocated memory. Otherwise, move the left memory to the free list and
request new memory from the system.
*/
template <bool threads, int inst> char *DefaultAllocTemplate<threads, inst>::chunkAlloc(size_t size, int &nObjs)
{
    char *result = nullptr;

    size_t totalBytes = size * nObjs;
    size_t leftBytes = endFree_ - startFree_;

    if (leftBytes >= totalBytes)
    {
        result = startFree_;
        startFree_ += totalBytes;
        return result;
    }
    else if (leftBytes > size)
    {
        nObjs = (int)(leftBytes / size);
        result = startFree_;
        startFree_ += size * nObjs;
        return result;
    }
    else
    {
        size_t newBytes = 2 * totalBytes + getRoundUp(heapSize_ >> 4);

        if (leftBytes > 0)
        {
            // Put the start memory to the free list.
            Obj *volatile *pObjPtr = freeList_ + getFreeListIndex(leftBytes);
            ((Obj *)startFree_)->freeListLink_ = *pObjPtr;
            *pObjPtr = (Obj *)startFree_;
        }

        startFree_ = (char *)std::malloc(newBytes);
        if (startFree_ == 0)
        {
            // If allocation is fail, try from the free list.
            for (size_t i = size; i < (size_t)MAX_BYTES_; i += (size_t)ALIGN_)
            {
                Obj *volatile *pObjPtr = freeList_ + getFreeListIndex(i);
                Obj *ptr = *pObjPtr;
                if (ptr != NULL)
                {
                    // Push the start memory from the free list.
                    startFree_ = (char *)ptr;
                    *pObjPtr = ptr->freeListLink_;
                    endFree_ = startFree_ + i;
                    return chunkAlloc(size, nObjs);
                }
            }
            endFree_ = nullptr;
            startFree_ = (char *)std::malloc(newBytes);
        }
        heapSize_ += newBytes;
        endFree_ = startFree_ + newBytes;
        return chunkAlloc(size, nObjs);
    }
}

typedef DefaultAllocTemplate<0, 0> DefaultAlloc;

int main()
{
    MemoryListPool<int, 10> memoryListPool;

    int *ptrA = memoryListPool.alloc();
    int *ptrB = memoryListPool.alloc();

    memoryListPool.deAlloc(ptrA);
    memoryListPool.deAlloc(ptrB);

    int *ptrC = (int *)DefaultAlloc::alloc(sizeof(int));
    DefaultAlloc::deAlloc(ptrC, sizeof(int));
}