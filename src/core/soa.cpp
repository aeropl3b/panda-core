#include "xlib/core/soa.h"

namespace xlib
{


soa_bucket::soa_bucket(soa_bucket&& other)
{
   std::swap(other._data, this->_data);
}

soa_bucket::~soa_bucket()
{
   for(auto& e: _data)
   {
      delete e.first;
   }
}

}