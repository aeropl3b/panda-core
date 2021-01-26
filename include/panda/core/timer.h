#pragma once

#include <chrono>

struct Timer
{
   using clock = std::chrono::high_resolution_clock;
   using tpoint = clock::time_point;

   void tic()
   {
      start = clock::now();
   }
   void toc()
   {
      end = clock::now();
   }

   template < class T >
   typename T::rep elapsed()
   {
      T elapsed = std::chrono::duration_cast<T>(end - start);
      return  elapsed.count();
   }
   tpoint start, end;
};

