#pragma once


namespace xlib
{

template < class T >
struct promote_fp;

#define PROMOTE_FP_DEF(TYPE, PTYPE)\
template <>\
struct promote_fp<TYPE>\
{\
   using type = PTYPE;\
}

PROMOTE_FP_DEF(double,double);
PROMOTE_FP_DEF(float,float);
PROMOTE_FP_DEF(int8_t,float);
PROMOTE_FP_DEF(int16_t,float);
PROMOTE_FP_DEF(int32_t,float);
PROMOTE_FP_DEF(int64_t,double);

}
