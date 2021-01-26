#include <xlib/core/assert.h>

namespace xlib
{

template < class T, size_t N >
class vec
{
public:
   T& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }
   T& operator[](size_t i) { _XLIB_ASSERT(RANGE, 0, N, i); return _data[i]; }

   template < class U, class = std::enable_if_t<std::is_convertible<T,U>::value> >
   xlib::promote_fp_t<T> dot(const vec<U,N>& lhs)
   {

   }

   template < class U, class = std::enable_if_t<std::is_convertible<T,U>::value && N == 3> >
   vec<xlib::promote_fp_t<T>,N> cross(const vec<U,N>& lhs)
   {

   }

private:
   T _data[N];
};

}