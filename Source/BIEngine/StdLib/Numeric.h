#pragma once

#include "Functional.h"
#include "Iterator.h"

namespace BIEngine {

template <typename Itr, typename T, typename BinaryOp>
T Accumulate(Itr begin, Itr end, T initVal, BinaryOp op)
{
   for (; begin != end; ++begin) {
      initVal = op(initVal, *begin);
   }

   return initVal;
}

template <typename Itr, typename T>
T Accumulate(Itr begin, Itr end, T initVal)
{
   return Accumulate(begin, end, initVal, Plus<T>());
}

template <typename Itr1, typename Itr2, typename T, typename BinaryOp1, typename BinaryOp2>
T InnerProduct(Itr1 begin1, Itr1 end1, Itr2 begin2, T initVal, BinaryOp1 op1, BinaryOp2 op2)
{
   for (; begin1 != end1; ++begin1, ++begin2) {
      initVal = op1(initVal, op2(*begin1, *begin2));
   }

   return initVal;
}

template <typename Itr1, typename Itr2, typename T>
T InnerProduct(Itr1 begin1, Itr1 end1, Itr2 begin2, T initVal)
{
   return InnerProduct(begin1, end1, begin2, initVal, Plus<T>(), Multiplies<T>());
}

template <typename InputItr, typename OutputItr, typename BinaryOp>
OutputItr PartialSum(InputItr begin, InputItr end, OutputItr out, BinaryOp op)
{
   *out = *begin;

   OutputItr prev = out;

   ++out;
   ++begin;

   for (; begin != end; ++begin, ++out, ++prev) {
      *out = op(*prev, *begin);
   }

   return out;
}

template <typename InputItr, typename OutputItr>
OutputItr PartialSum(InputItr begin, InputItr end, OutputItr out)
{
   return PartialSum(begin, end, out, Plus<typename IteratorTraits<InputItr>::ValueType>());
}

template <typename InputItr, typename OutputItr, typename BinaryOp>
OutputItr AdjacentDifference(InputItr begin, InputItr end, OutputItr out, BinaryOp op)
{
   *out = *begin;

   OutputItr prev = begin;

   ++out;
   ++begin;

   for (; begin != end; ++begin, ++out, ++prev) {
      *out = op(*begin, *prev);
   }

   return out;
}

template <typename InputItr, typename OutputItr>
OutputItr AdjacentDifference(InputItr begin, InputItr end, OutputItr out)
{
   return AdjacentDifference(begin, end, out, Minus<typename IteratorTraits<InputItr>::ValueType>());
}

} // namespace BIEngine
