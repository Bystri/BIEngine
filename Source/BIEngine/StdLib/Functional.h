#pragma once

namespace BIEngine {

template <typename T>
struct Negate {
   T operator()(const T& val)
   {
      return -val;
   }
};

template <typename T>
struct Plus {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs + rhs;
   }
};

template <typename T>
struct Minus {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs - rhs;
   }
};

template <typename T>
struct Multiplies {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs * rhs;
   }
};

template <typename T>
struct Divides {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs / rhs;
   }
};

template <typename T>
struct Modulus {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs / rhs;
   }
};

template <typename T>
struct EqualTo {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs == rhs;
   }
};

template <typename T>
struct NotEqualTo {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs != rhs;
   }
};

template <typename T>
struct Less {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs < rhs;
   }
};

template <typename T>
struct Greater {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs < rhs;
   }
};

template <typename T>
struct LessEqual {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs <= rhs;
   }
};

template <typename T>
struct GreaterEqual {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs >= rhs;
   }
};

template <typename T>
struct LogicalNot {
   bool operator()(const T& val)
   {
      return !val;
   }
};

template <typename T>
struct LogicalAnd {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs && rhs;
   }
};

template <typename T>
struct LogicalOr {
   bool operator()(const T& lhs, const T& rhs)
   {
      return lhs || rhs;
   }
};

template <typename T>
struct BitAnd {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs & rhs;
   }
};

template <typename T>
struct BitOr {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs | rhs;
   }
};

template <typename T>
struct BitXor {
   T operator()(const T& lhs, const T& rhs)
   {
      return lhs ^ rhs;
   }
};

} // namespace BIEngine
