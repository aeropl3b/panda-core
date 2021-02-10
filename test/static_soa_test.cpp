#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <numeric>

#include <panda/core/static_soa.h>

template < class T >
struct default_value
{
   using type = std::remove_reference_t<T>;
   static const type value;
};

template < class T >
typename default_value<T>::type const default_value<T>::value = default_value<T>::type();

PANDA_REGISTER_SOA_LAYOUT(test_layout,
   PANDA_LAYOUT_VAR(std::vector<double>, var1)
   PANDA_LAYOUT_VAR(std::vector<double>, var2)
);

TEST(static_soa, callbacks)
{
   test_layout tl;

   double init_value = 3.1415;
   tl.register_initializer<test_layout::_var1_v>(1, [init_value,&tl](std::vector<double>::iterator begin, std::vector<double>::iterator end)
         {
            printf("Init 1\n");
            auto& v1 = tl.var1();
            auto& v2 = tl.var2();
            auto offset = std::distance(v1.begin(), begin);
            std::copy(v2.begin() + offset, v2.end(), begin);
         });
   tl.register_initializer<test_layout::_var2_v>(0, [init_value](std::vector<double>::iterator begin, std::vector<double>::iterator end)
         {
            printf("Init 2\n");
            std::fill(begin, end, init_value);
         });

   auto& v1_1 = tl.var1();
   auto& v2_1 = tl.var2();
   auto& v1_2 = tl.get<test_layout::_var1_v>();
   auto& v2_2 = tl.get<test_layout::_var2_v>();

   ASSERT_EQ(&v1_1, &v1_2);
   ASSERT_EQ(&v2_1, &v2_2);

   tl.resize(10);

   ASSERT_EQ(v1_1.size(), 10);
   ASSERT_EQ(v2_1.size(), 10);

   for(auto& value: v1_1)
   {
      ASSERT_EQ(value, init_value);
      value += 1.;
   }

   tl.resize(20);

   for(int i = 0; i < 10; i++)
   {
      ASSERT_EQ(v1_1[i], init_value + 1.);
   }
   for(int i = 10; i < 20; i++)
   {
      ASSERT_EQ(v1_1[i], init_value);
   }

   using TestBucket = panda::core::static_soa<
      std::vector<char>,
      std::vector<double>,
      std::vector<int>,
      std::vector<char>,
      std::vector<long double>
   >;

#if 0
   TestBucket bucket;

   auto lambda = [](char& c, double& d, int& i, char& c2, long double& ld, int index) -> int
   {
      c = index;
      d = 1./(double)index;
      i = index + 1;
      c2 = c * 2;
      ld = 1. / d;
      return index;
   };

   bucket.resize(10);
   for(int i = 0; i < 10; i++)
   {
      bucket.apply_to_element(i, lambda, i);
   }

   std::vector<int> results(bucket.size());
   bucket.apply_per_element([&lambda](char& c, double& d, int& i, char& c2, long double& ld, int index, std::vector<int>& output)
         {
            output[index] = lambda(c,d,i,c2,ld,index);
         }, results);

   size_t i = 0;
   for(auto& r: results)
   {
      ASSERT_EQ(r,i++);
   }
#endif
}

TEST(static_soa, handles)
{
   using TestBucket = panda::core::static_soa<
      std::vector<char>,
      std::vector<double>,
      std::vector<int>,
      std::vector<char>,
      std::vector<long double>
   >;
   TestBucket bucket;

   TestBucket::handle h = bucket.get_handle<0>();
   auto mh = bucket.get_handle<0>();

   {
      //auto adata = h.data<char*>();
      auto bdata = mh.data();
      //ASSERT_EQ(adata, bdata);
   }

   bucket.resize(10);
   {
      auto adata = h.data<std::vector<char>>();
      auto bdata = mh.data();
      ASSERT_EQ(adata, bdata);
   }

}

TEST(static_soa, main)
{
#if 0
   panda::core::static_soa<char*, std::vector<double>, int*, char*, long double*> bucket;

   size_t size = 0;
   ASSERT_EQ(bucket.size(), 0);
   bucket.resize(size = 10);
   ASSERT_EQ(bucket.size(), size);
   {
      auto& dvec = bucket.get<0>();
      std::iota(std::begin(dvec), std::end(dvec), 0);
      ASSERT_EQ(std::distance(std::begin(dvec), std::end(dvec)), size);

      auto& ivec = bucket.get<1>();
      std::iota(std::begin(ivec), std::end(ivec), 0);
      ASSERT_EQ(std::distance(std::begin(ivec), std::end(ivec)), size);

      auto& ldvec = bucket.get<3>();
      std::iota(std::begin(ldvec), std::end(ldvec), 0);
      ASSERT_EQ(std::distance(std::begin(ldvec), std::end(ldvec)), size);

      for(int i = 0; i < size; i++)
      {
         ASSERT_EQ(ivec[i], i);
         ASSERT_EQ(dvec[i], i);
         ASSERT_EQ(ldvec[i], i);
      }
   }
   bucket.reorder(std::vector<int>{9,8,76,5,4,3,2,1,0});
   bucket.reorder(std::vector<size_t>{9,8,76,5,4,3,2,1,0});
   bucket.reorder(std::vector<long>{9,8,76,5,4,3,2,1,0});
   bucket.resize(size = 20);
   ASSERT_EQ(bucket.size(), size);
   {
      auto& dvec = bucket.get<0>();
      ASSERT_EQ(std::distance(std::begin(dvec), std::end(dvec)), size);

      auto ivec = bucket.get<1>();
      ASSERT_EQ(std::distance(std::begin(ivec), std::end(ivec)), size);

      auto ldvec = bucket.get<3>();
      ASSERT_EQ(std::distance(std::begin(ldvec), std::end(ldvec)), size);

      for(int i = 9; i >= 0; i--)
      {
         ASSERT_EQ(dvec[i], i) << "size: " << size << " i: " << i;
         ASSERT_EQ(ivec[i], i);
         ASSERT_EQ(ldvec[i], i);
      }

      for(int i = 10; i < size; i++)
      {
         ASSERT_EQ(ivec[i], default_value<decltype(ivec[0])>::value);
         ASSERT_EQ(dvec[i], default_value<decltype(dvec[0])>::value);
         ASSERT_EQ(ldvec[i], default_value<decltype(ldvec[0])>::value);
      }
   }
   bucket.resize(size = 4);
   ASSERT_EQ(bucket.size(), size);
   {
      auto& dvec = bucket.get<0>();
      ASSERT_EQ(std::distance(std::begin(dvec), std::end(dvec)), size);

      auto ivec = bucket.get<1>();
      ASSERT_EQ(std::distance(std::begin(ivec), std::end(ivec)), size);

      auto ldvec = bucket.get<3>();
      ASSERT_EQ(std::distance(std::begin(ldvec), std::end(ldvec)), size);

      for(int i = 0; i < size; i++)
      {
         ASSERT_EQ(dvec[i], i) << "size: " << size << " i: " << i;
         ASSERT_EQ(ivec[i], i);
         ASSERT_EQ(ldvec[i], i);
      }
   }
   bucket.resize(size = 0);
   bucket.resize(size = 100);
   bucket.resize(size = 10);
   ASSERT_EQ(bucket.size(), size);
   {
      auto& dvec = bucket.get<0>();
      ASSERT_EQ(std::distance(std::begin(dvec), std::end(dvec)), size);

      auto ivec = bucket.get<1>();
      ASSERT_EQ(std::distance(std::begin(ivec), std::end(ivec)), size);

      auto ldvec = bucket.get<3>();
      ASSERT_EQ(std::distance(std::begin(ldvec), std::end(ldvec)), size);

      for(int i = 0; i < size; i++)
      {
         ASSERT_EQ(ivec[i], default_value<decltype(ivec[0])>::value);
         ASSERT_EQ(dvec[i], default_value<decltype(dvec[0])>::value);
         ASSERT_EQ(ldvec[i], default_value<decltype(ldvec[0])>::value);
      }
   }
#endif
}
