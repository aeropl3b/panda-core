

#define _XLIB_ASSERT(TYPE, ...) _XLIB_ASSERT_##TYPE ( __VA_ARGS__)

#define _XLIB_ASSERT_RANGE(L_BOUND, U_BOUND, X) assert((L_BOUND) < (X)); assert((U_BOUND) > (X))
