#pragma once

namespace BIEngine {

template <typename T>
class DefaultDeleter {
public:
   void operator()(T* ptr)
   {
      delete ptr;
   }
};

template <typename T>
class DefaultDeleter<T[]> {
public:
   void operator()(T* ptr)
   {
      delete[] ptr;
   }
};

} // namespace BIEngine
