#include <boost/preprocessor.hpp>

#define _PANDA_LAYOUT_GET_NAME(_TYPE, _NAME) _NAME
#define _PANDA_LAYOUT_GET_TYPE(_TYPE, _NAME) _TYPE
#define _PANDA_LAYOUT_TYPE(r,_,_INDEX, _NODE) (_PANDA_LAYOUT_GET_TYPE _NODE)
#define _PANDA_LAYOUT_INTEGRAL_CONST_I(r,_PREFIX,_INDEX,_NODE) using BOOST_PP_CAT(_PREFIX, _PANDA_LAYOUT_GET_NAME _NODE) = std::integral_constant<std::size_t, _INDEX>;
#define _PANDA_LAYOUT_CONSTEXPR_INDEX_I(r,_PREFIX,_INDEX,_NODE) static constexpr std::size_t BOOST_PP_SEQ_CAT((_PREFIX)(_PANDA_LAYOUT_GET_NAME _NODE)(_v)) = _INDEX;
#define _PANDA_LAYOUT_GETTER_I(r,_,_INDEX,_NODE) typename std::tuple_element<BOOST_PP_SUB(_INDEX,2),soa_t>::type& _PANDA_LAYOUT_GET_NAME _NODE () { return this->get<_INDEX>(); }

#define _PANDA_EVAL_MACRO(_MACRO, _ARGS) _MACRO _ARGS
#define _PANDA_REM(...) __VA_ARGS__

#define PANDA_REGISTER_SOA_LAYOUT(_NAME, LAYOUT_SEQ) \
class _NAME : \
   public ::panda::core::static_soa<_PANDA_EVAL_MACRO(_PANDA_REM,BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(_PANDA_LAYOUT_TYPE,_,LAYOUT_SEQ)))>\
{\
   using soa_t = ::panda::core::static_soa<_PANDA_EVAL_MACRO(_PANDA_REM,BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(_PANDA_LAYOUT_TYPE,_,LAYOUT_SEQ)))>;\
public:\
   BOOST_PP_SEQ_FOR_EACH_I(_PANDA_LAYOUT_INTEGRAL_CONST_I,_,LAYOUT_SEQ)\
   BOOST_PP_SEQ_FOR_EACH_I(_PANDA_LAYOUT_CONSTEXPR_INDEX_I,_,LAYOUT_SEQ)\
   BOOST_PP_SEQ_FOR_EACH_I(_PANDA_LAYOUT_GETTER_I,_,LAYOUT_SEQ)\
}

#define PANDA_LAYOUT_VAR(_TYPE, _NAME) ((_TYPE, _NAME))

