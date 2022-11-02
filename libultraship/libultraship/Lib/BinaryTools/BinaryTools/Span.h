#pragma once

//Simple wrapper around a contiguous area of memory
template<class T>
struct Span
{
public:
    Span(T* ptr, size_t size) : ptr_(ptr), size_(size) {}

    T* Data() { return ptr_; }
    size_t Size() { return size_; }

    //Returns pointer to start of contiguous memory area
    T* begin() { return ptr_; }
    //Returns pointer to start of contiguous memory area
    const T* begin() const { return ptr_; }
    //Returns pointer to end of contiguous memory area (the end of the last memory). 
    //To get the last member, use back() instead.
    T* end() { return ptr_ + size_; }
    //Returns pointer to end of contiguous memory area (the end of the last memory). 
    //To get the last member, use back() instead.
    const T* end() const { return ptr_ + size_; }
    //Returns reference to first member of the span
    T& front() { return *ptr_; }
    //Returns reference to first member of the span
    const T& front() const { return *ptr_; }
    //Returns reference to last member of the span
    T& back() { return *(ptr_ + size_); }
    //Returns reference to last member of the span
    const T& back() const { return *(ptr_ + size_); }

    //Todo: Add optional bounds checking for debug builds
    //Returns reference to element at provided index. Does no bounds checking
    T& operator[](size_t index) { return ptr_[index]; }
    //Returns reference to element at provided index. Does no bounds checking
    const T& operator[](size_t index) const { return ptr_[index]; }

private:
    T* ptr_ = nullptr;
    size_t size_ = 0;
};