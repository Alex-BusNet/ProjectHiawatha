#ifndef HEAP_H
#define HEAP_H


template <class T> class Heap
{

public:
    Heap(int maxHeapSize);

    void SortUp();
    void SortDown();
    void Add(T item);
    void UpdateItem(T item);
    void Swap();

    T RemoveFirst();

    int Count();
    bool Contains();

private:
    T items[];
    int currentItemCount;

};

#endif // HEAP_H
