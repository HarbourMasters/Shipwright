/*
 *  IXWebSocketSendData.h
 *
 *  WebSocket (Binary/Text) send data buffer
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <iterator>

namespace ix
{
    /*
    * IXWebSocketSendData implements a wrapper for std::string, std:vector<char/uint8_t> and char*.
    * It removes the necessarity to copy the data or string into a std::string 
    */
    class IXWebSocketSendData {
    public:

        template<typename T>
        struct IXWebSocketSendData_const_iterator
            //: public std::iterator<std::forward_iterator_tag, T>
        {
            typedef IXWebSocketSendData_const_iterator<T> const_iterator;

            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type*;
            using reference = const value_type&;

            pointer _ptr;
        public:
            IXWebSocketSendData_const_iterator() : _ptr(nullptr) {}
            IXWebSocketSendData_const_iterator(pointer ptr) : _ptr(ptr) {}
            ~IXWebSocketSendData_const_iterator() {}

            const_iterator  operator++(int) { return const_iterator(_ptr++); }
            const_iterator& operator++() { ++_ptr; return *this; }
            reference       operator* () const { return *_ptr; }
            pointer         operator->() const { return _ptr; }
            const_iterator  operator+ (const difference_type offset) const { return const_iterator(_ptr + offset); }
            const_iterator  operator- (const difference_type offset) const { return const_iterator(_ptr - offset); }
            difference_type operator- (const const_iterator& rhs) const { return _ptr - rhs._ptr; }
            bool            operator==(const const_iterator& rhs) const { return _ptr == rhs._ptr; }
            bool            operator!=(const const_iterator& rhs) const { return _ptr != rhs._ptr; }
            const_iterator& operator+=(const difference_type offset) { _ptr += offset; return *this; }
            const_iterator& operator-=(const difference_type offset) { _ptr -= offset; return *this; }
        };

        using const_iterator = IXWebSocketSendData_const_iterator<char>;

        /* The assigned std::string must be kept alive for the lifetime of the input buffer */
        IXWebSocketSendData(const std::string& str)
            : _data(str.data())
            , _size(str.size())
        {
        }

        /* The assigned std::vector must be kept alive for the lifetime of the input buffer */
        IXWebSocketSendData(const std::vector<char>& v)
            : _data(v.data())
            , _size(v.size())
        {
        }

        /* The assigned std::vector must be kept alive for the lifetime of the input buffer */
        IXWebSocketSendData(const std::vector<uint8_t>& v)
            : _data(reinterpret_cast<const char*>(v.data()))
            , _size(v.size())
        {
        }

        /* The assigned memory must be kept alive for the lifetime of the input buffer */
        IXWebSocketSendData(const char* data, size_t size)
            : _data(data)
            , _size(data == nullptr ? 0 : size)
        {
        }

        bool empty() const
        {
            return _data == nullptr || _size == 0;
        }

        const char* c_str() const
        {
            return _data;
        }

        const char* data() const
        {
            return _data;
        }

        size_t size() const
        {
            return _size;
        }

        inline const_iterator begin() const
        {
            return const_iterator(const_cast<char*>(_data));
        }

        inline const_iterator end() const
        {
            return const_iterator(const_cast<char*>(_data) + _size);
        }

        inline const_iterator cbegin() const
        {
            return begin();
        }

        inline const_iterator cend() const
        {
            return end();
        }

    private:
        const char* _data;
        const size_t _size;
    };

}