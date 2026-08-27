#include "PointerManager.h"
#include <iostream>
#include "../Tree/Tree.h"
#include "../Symbol/Symbol.h"
#include "../debug/debug.h"

template <class T>
void PointerManager<T>::add(T ptr) {
    this->ptrs.push_back(ptr);
}

template <class T>
void PointerManager<T>::free() {
    int count = 0;
    for (auto ptr : this->ptrs) {
        if (ptr != nullptr) {
            delete ptr;
            ++ count;
            ptr = nullptr;
        }
    }
    INFO("[free] " << count << " pointer(s)");
}

template <class T>
PointerManager<T>::~PointerManager() {
    this->free();
}

#ifdef DEBUG
template <class T>
void PointerManager<T>::display() {
    for (auto ptr : this->ptrs) {
        std::cout << ptr << std::endl;
    }
}
#endif

template class PointerManager<Tree*>;
//template class PointerManager<VarSymbol*>;
//template class PointerManager<FuncSymbol*>;
