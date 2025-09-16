#pragma once

#include <utility>
#include <random>

#include "Iterator.h"
#include "Functional.h"

namespace BIEngine {

// Nonmodifying algorithms

template <typename Itr, typename T>
typename IteratorTraits<Itr>::DifferenceType Count(Itr begin, Itr end, const T& val)
{
   typename IteratorTraits<Itr>::DifferenceType cnt = 0;
   for (; begin != end; ++begin) {
      if (*begin == val) {
         ++cnt;
      }
   }

   return cnt;
}

template <typename Itr, typename UnaryPred>
typename IteratorTraits<Itr>::DifferenceType CountIf(Itr begin, Itr end, UnaryPred p)
{
   typename IteratorTraits<Itr>::DifferenceType cnt = 0;
   for (; begin != end; ++begin) {
      if (p(*begin)) {
         ++cnt;
      }
   }

   return cnt;
}

template <typename Itr, typename CompFunc>
Itr MinElement(Itr begin, Itr end, CompFunc op)
{
   if (begin == end) {
      return end;
   }

   Itr min = begin;
   while (++begin != end) {
      if (op(*begin, *min)) {
         min = begin;
      }
   }

   return min;
}

template <typename Itr>
Itr MinElement(Itr begin, Itr end)
{
   using ValueType = typename IteratorTraits<Itr>::ValueType;
   return MinElement(begin, end, Less<ValueType>());
}

template <typename Itr, typename CompFunc>
Itr MaxElement(Itr begin, Itr end, CompFunc op)
{
   if (begin == end) {
      return end;
   }

   Itr max = begin;
   while (++begin != end) {
      if (op(*begin, *max)) {
         max = begin;
      }
   }

   return max;
}

template <typename Itr>
Itr MaxElement(Itr begin, Itr end)
{
   using ValueType = typename IteratorTraits<Itr>::ValueType;
   return MaxElement(begin, end, Greater<ValueType>());
}

template <typename Itr, typename CompOp>
std::pair<Itr, Itr> MinMaxElement(Itr begin, Itr end, CompOp op)
{
   Itr min = begin;
   Itr max = begin;

   if (begin == end || ++begin == end) {
      return std::make_pair(min, max);
   }

   if (op(*begin, *min)) {
      min = begin;
   } else {
      max = begin;
   }

   while (++begin != end) {
      Itr temp = begin;

      if (++begin == end) {
         if (op(*temp, *min)) {
            min = temp;
         }
         if (!op(*temp, *max)) {
            max = temp;
         }

         break;
      }

      if (op(*temp, *begin)) {
         if (op(*temp, *min)) {
            min = temp;
         }
         if (!op(*begin, *max)) {
            max = begin;
         }
      } else {
         if (op(*begin, *min)) {
            min = begin;
         }
         if (!op(*temp, *max)) {
            max = temp;
         }
      }
   }

   return std::make_pair(min, max);
}

template <typename Itr>
std::pair<Itr, Itr> MinMaxElement(Itr begin, Itr end)
{
   using ValueType = typename IteratorTraits<Itr>::ValueType;
   return MinMaxElement(begin, end, Less<ValueType>());
}

template <typename Itr, typename T>
Itr Find(Itr begin, Itr end, const T& value)
{
   for (; begin != end; ++begin) {
      if (*begin == value) {
         return begin;
      }
   }

   return end;
}

template <typename Itr, typename UnaryPred>
Itr FindIf(Itr begin, Itr end, UnaryPred op)
{
   for (; begin != end; ++begin) {
      if (op(*begin)) {
         return begin;
      }
   }

   return end;
}

template <typename Itr, typename UnaryPred>
Itr FindIfNot(Itr begin, Itr end, UnaryPred op)
{
   for (; begin != end; ++begin) {
      if (!op(*begin)) {
         return begin;
      }
   }

   return end;
}

template <typename Itr, typename UnaryPred>
bool AllOf(Itr begin, Itr end, UnaryPred op)
{
   for (; begin != end; ++begin) {
      if (!op(*begin)) {
         return false;
      }
   }

   return true;
}

template <typename Itr, typename UnaryPred>
bool AnyOf(Itr begin, Itr end, UnaryPred op)
{
   for (; begin != end; ++begin) {
      if (op(*begin)) {
         return true;
      }
   }

   return false;
}

template <typename Itr, typename UnaryPred>
bool NoneOf(Itr begin, Itr end, UnaryPred op)
{
   for (; begin != end; ++begin) {
      if (op(*begin)) {
         return false;
      }
   }

   return true;
}

// Modifying algorithms

template <typename InputItr, typename OutputItr>
OutputItr Copy(InputItr begin, InputItr end, OutputItr output)
{
   for (; begin != end; ++begin, ++output) {
      *output = *begin;
   }

   return output;
}

template <typename InputItr, typename OutputItr, typename UnaryOp>
OutputItr CopyIf(InputItr begin, InputItr end, OutputItr output, UnaryOp op)
{
   for (; begin != end; ++begin) {
      if (op(*begin)) {
         *output = *begin;
         ++output;
      }
   }

   return output;
}

template <typename InputItr, typename Size, typename OutputItr>
OutputItr CopyN(InputItr begin, Size n, OutputItr output)
{
   while (n--) {
      *output = *begin;
      ++output;
      ++begin;
   }

   return output;
}

template <typename InputItr, typename OutputItr>
OutputItr Move(InputItr begin, InputItr end, OutputItr output)
{
   for (; begin != end; ++begin, ++output) {
      *output = std::move(*begin);
   }

   return output;
}

template <typename InputItr, typename OutputItr, typename UnaryOp>
OutputItr Transform(InputItr begin, InputItr end, OutputItr output, UnaryOp op)
{
   for (; begin != end; ++begin, ++output) {
      *output = op(*begin);
   }

   return output;
}

template <typename InputItr, typename T>
void Fill(InputItr begin, InputItr end, const T& val)
{
   for (; begin != end; ++begin) {
      *begin = val;
   }
}

template <typename InputItr, typename Size, typename T>
void FillN(InputItr begin, Size n, const T& val)
{
   while (n--) {
      *begin = val;
      ++begin;
   }
}

template <typename InputItr, typename Func>
void Generate(InputItr begin, InputItr end, Func op)
{
   for (; begin != end; ++begin) {
      *begin = op();
   }
}

template <typename InputItr, typename Size, typename Func>
void GenerateN(InputItr begin, Size n, Func op)
{
   while (n--) {
      *begin = op();
      ++begin;
   }
}

template <typename InputItr, typename T>
void Iota(InputItr begin, InputItr end, T val)
{
   for (; begin != end; ++begin, ++val) {
      *begin = val;
   }
}

template <typename InputItr, typename T>
void Replace(InputItr begin, InputItr end, const T& oldVal, const T& newVal)
{
   for (; begin != end; ++begin) {
      if (*begin == oldVal) {
         *begin = newVal;
      }
   }
}

template <typename InputItr, typename UnaryPred, typename T>
void ReplaceIf(InputItr begin, InputItr end, UnaryPred op, const T& newVal)
{
   for (; begin != end; ++begin) {
      if (op(*begin)) {
         *begin = newVal;
      }
   }
}

// Removing algorithms

template <typename Itr, typename T>
Itr Remove(Itr begin, Itr end, const T& val)
{
   begin = Find(begin, end, val);
   if (begin == end) {
      return end;
   }

   for (Itr i = begin; i != end; ++i) {
      if (!(*i == val)) {
         *begin = std::move(*i);
         ++begin;
      }
   }

   return begin;
}

template <typename Itr, typename UnaryPred>
Itr RemoveIf(Itr begin, Itr end, UnaryPred op)
{
   begin = FindIf(begin, end, op);
   if (begin == end) {
      return end;
   }

   for (Itr i = begin; i != end; ++i) {
      if (!op(*i)) {
         *begin = std::move(*i);
         ++begin;
      }
   }

   return begin;
}

template <typename InputItr, typename OutputItr, typename T>
OutputItr RemoveCopy(InputItr begin, InputItr end, OutputItr out, const T& val)
{
   for (; begin != end; ++begin, ++out) {
      if (*begin == val) {
         break;
      }

      *out = *begin;
   }

   for (InputItr i = begin; ++i != end;) {
      if (!(*i == val)) {
         *out = std::move(*i);
         ++out;
      }
   }

   return out;
}

template <typename InputItr, typename OutputItr, typename UnaryPred>
OutputItr RemoveCopyIf(InputItr begin, InputItr end, OutputItr out, UnaryPred op)
{
   for (; begin != end; ++begin, ++out) {
      if (op(*begin)) {
         break;
      }

      *out = *begin;
   }

   for (InputItr i = begin; ++i != end;) {
      if (!op(*i)) {
         *out = std::move(*i);
         ++out;
      }
   }

   return out;
}

template <typename Itr>
Itr Unique(Itr begin, Itr end)
{
   Itr next = begin;
   while (++next != end) {
      if (!(*begin == *next) && ++begin != next) {
         *begin = std::move(*next);
      }
   }

   return next;
}

template <typename Itr, typename BinaryPred>
Itr Unique(Itr begin, Itr end, BinaryPred op)
{
   Itr next = begin;
   while (++next != end) {
      if (!op(*begin, *next) && ++begin != next) {
         *begin = std::move(*next);
      }
   }

   return next;
}

template <typename InputItr, typename OutputItr>
OutputItr UniqueCopy(InputItr begin, InputItr end, OutputItr out)
{
   if (begin == end) {
      return out;
   }

   *out = *begin++;

   for (; begin != end; ++begin) {
      if (!(*out == *begin)) {
         ++out;
         *out = *begin;
      }
   }

   ++out;

   return out;
}

template <typename InputItr, typename OutputItr, typename BinaryPred>
OutputItr UniqueCopy(InputItr begin, InputItr end, OutputItr out, BinaryPred op)
{
   if (begin == end) {
      return out;
   }

   *out = *begin++;

   for (; begin != end; ++begin) {
      if (!op(*out, *begin)) {
         ++out;
         *out = *begin;
      }
   }

   ++out;

   return out;
}

template <typename Itr>
void Reverse(Itr begin, Itr end)
{
   if (begin == end) {
      return;
   }

   --end;

   while (begin != end) {
      std::swap(*begin, *end);
      ++begin;
      --end;
   }
}

template <typename InputItr, typename OutputItr>
OutputItr ReverseCopy(InputItr begin, InputItr end, OutputItr out)
{
   if (begin == end) {
      return out;
   }

   --end;

   do {
      *out++ = *end;
   } while (begin != end--);

   return out;
}

template <typename Itr, typename UniformRandomNumberGenerator>
void Shuffle(Itr begin, Itr end, UniformRandomNumberGenerator&& gen)
{
   using DiffT = typename IteratorTraits<Itr>::DifferenceType;
   using DistrT = std::uniform_int_distribution<DiffT>;

   DistrT Distr;
   for (typename IteratorTraits<Itr>::DifferenceType i = end - begin - 1; i >= 0; --i) {
      std::swap(begin[i], begin[Distr(gen, DistrT::param_type(0, i))]);
   }
}

// Sorting algorithms

template <typename Itr, typename BinaryPred>
void Sort(Itr begin, Itr end, BinaryPred op)
{
   if (end - begin <= 1) {
      return;
   }

   Itr pivot = end - 1;

   Itr l = begin - 1;
   Itr r = end;

   // partition
   while (true) {
      do {
         ++l;
      } while (l < r && op(*l, *pivot));

      do {
         --r;
      } while (l < r && !op(*r, *pivot));

      if (l >= r) {
         break;
      }

      std::swap(*l, *r);
   }

   std::swap(*(end - 1), *l);

   Sort(begin, l, op);
   Sort(l + 1, end, op);
}

template <typename Itr>
void Sort(Itr begin, Itr end)
{
   Sort(begin, end, Less<typename IteratorTraits<Itr>::ValueType>());
}

// Sorted-range algorithms

template <typename Itr, typename T, typename BinaryPred>
Itr LowerBound(Itr begin, Itr end, const T& val, BinaryPred op)
{
   typename IteratorTraits<Itr>::DifferenceType count = end - begin;
   typename IteratorTraits<Itr>::DifferenceType step;

   Itr it;
   while (count > 0) {
      step = count / 2;
      it = begin + step;

      if (op(*it, val)) {
         begin = ++it;
         count -= step + 1;
      } else {
         count = step;
      }
   }

   return begin;
}

template <typename Itr, typename T>
Itr LowerBound(Itr begin, Itr end, const T& val)
{
   return LowerBound(begin, end, val, Less<typename IteratorTraits<Itr>::ValueType>());
}

template <typename Itr, typename T, typename BinaryPred>
Itr UpperBound(Itr begin, Itr end, const T& val, BinaryPred op)
{
   typename IteratorTraits<Itr>::DifferenceType count = end - begin;
   typename IteratorTraits<Itr>::DifferenceType step;

   Itr it;
   while (count > 0) {
      step = count / 2;
      it = begin + step;

      if (!op(val, *it)) {
         begin = ++it;
         count -= step + 1;
      } else {
         count = step;
      }
   }

   return begin;
}

template <typename Itr, typename T>
Itr UpperBound(Itr begin, Itr end, const T& val)
{
   return UpperBound(begin, end, val, Less<typename IteratorTraits<Itr>::ValueType>());
}

template <typename Itr, typename T, typename BinaryPred>
std::pair<Itr, Itr> EqualRange(Itr begin, Itr end, const T& val, BinaryPred op)
{
   return std::make_pair(LowerBound(begin, end, val, op), UpperBound(begin, end, val, op));
}

template <typename Itr, typename T>
std::pair<Itr, Itr> EqualRange(Itr begin, Itr end, const T& val)
{
   return EqualRange(begin, end, val, Less<typename IteratorTraits<Itr>::ValueType>());
}

template <typename Itr, typename T, typename BinaryPred>
bool BinarySearch(Itr begin, Itr end, const T& val, BinaryPred op)
{
   begin = LowerBound(begin, end, val, op);
   return (!(begin == end) && !(op(val, *begin)));
}

template <typename Itr, typename T>
bool BinarySearch(Itr begin, Itr end, const T& val)
{
   return BinarySearch(begin, end, val, Less<typename IteratorTraits<Itr>::ValueType>());
}

} // namespace BIEngine
