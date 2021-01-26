#pragma once

#include <xlib/core/vector.h>

namespace xlib
{

template < class T, size_t N, size_t M = N >
class matrix
{
   xlib::vec<T,M>& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }
   xlib::vec<T,M>& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }
private:
   xlib::vec<T,M> _data[N];
};

}