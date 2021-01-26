#pragma once

#include <xlib/core/matrix.h>

namespace xlib
{

template < class T, size_t N, size_t M = N >
class matrix
{
   xlib::matrix<T,M>& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }
   xlib::matrix<T,M>& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }
private:
   xlib::matrix<T,M> _data[N];
};

}