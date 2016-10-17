#pragma once

#include "my_swap.hpp"
#include "my_assert.hpp"
#include <stdlib.h>
#include <new>

template <typename T>
struct Vector {
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

    T *_data;
    size_t _size;
    size_t _capacity;

    size_t size() const {
        return _size;
    }

    size_t capacity() const {
        return _capacity;
    }

    bool empty() const {
        return size() == 0;
    }

          T* data        (        )       { return _data; }
    const T* data        (        ) const { return _data; }
          T& at          (size_t i)       { my_assert(i < size()); return data()[i]; }
    const T& at          (size_t i) const { my_assert(i < size()); return data()[i]; }
          T& operator [] (size_t i)       { return at(i); }
    const T& operator [] (size_t i) const { return at(i); }
          T& front       (        )       { return at(0); }
    const T& front       (        ) const { return at(0); }
          T& back        (        )       { return at(size() - 1); }
    const T& back        (        ) const { return at(size() - 1); }
          T* begin       (        )       { return data(); }
    const T* begin       (        ) const { return data(); }
          T* end         (        )       { return data() + size(); }
    const T* end         (        ) const { return data() + size(); }
    const T* cbegin      (        ) const { return data(); }
    const T* cend        (        ) const { return data() + size(); }

    Vector(): _data(NULL), _size(0), _capacity(0){}

    Vector(const T *values, size_t new_size, size_t new_capacity):
        _size(new_size),
        _capacity(new_capacity)
    {
        my_assert(size() <= capacity());
        _data = (T*)malloc(sizeof(T)*capacity());

        for (size_t i = 0; i < size(); i++){
            new(&at(i))T(values[i]);
        }
    }

    Vector(const T *values, size_t new_size):
        Vector(values, new_size, new_size){}

    Vector(size_t new_size, const T &value = T()):
        _size(new_size),
        _capacity(new_size)
    {
        _data = (T*)malloc(sizeof(T)*capacity());

        for (size_t i = 0; i < size(); i++){
            new(&at(i))T(value);
        }
    }

    Vector(const Vector &other):
        Vector(other.data(), other.size(), other.size()){}

    Vector& operator = (const Vector &other){
        Vector tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(Vector &other){
        my_swap(_data, other._data);
        my_swap(_size, other._size);
        my_swap(_capacity, other._capacity);
    }

    void reserve(size_t required_capacity){
        if (capacity() >= required_capacity) return;
        Vector temp(data(), size(), required_capacity);
        swap(temp);
    }

    void resize(size_t new_size, const T &value = T()){
        reserve(new_size);
        while (size() < new_size) push(value);
    }

    void push(const T &value){
        if (size() >= capacity()){
            reserve(size()*5/2 + 1);
        }
        my_assert(size() < capacity());
        T *ptr = data() + _size;
        new(ptr)T(value);
        _size++;
    }

    T pop(){
        my_assert(!empty());
        T value = back();
        back().~T();
        _size--;
        return value;
    }

    void clear(){
        while (!empty()) pop();
    }

    ~Vector(){
        clear();
        free(_data);
        _data = NULL;
    }
};
