#include "Assert.h"

#include <cassert>
#include <cstdarg>

#include "../Utilities/Logger.h"

namespace BIEngine {

void Assert(const bool expr, const char* format, ...)
{
   if (expr) {
      return;
   }

#ifndef _RELEASE
   static constexpr int MSG_SIZE = 512;
   char msg[MSG_SIZE];
   std::va_list argList;
   va_start(argList, format);
   int len = vsnprintf(msg, MSG_SIZE, format, argList);
   if (len >= MSG_SIZE) {
      len = MSG_SIZE - 1;
      msg[MSG_SIZE - 1] = '\0';

      const std::string errorMessage = "Log message was truncated";
      Logger::WriteLog(Logger::LogType::ERROR, errorMessage);
   }
   msg[len] = '\0';
   va_end(argList);

   const std::string strMsg(msg);

   Logger::WriteLog(Logger::LogType::ERROR, "[ASSERT]: " + strMsg);

   assert(expr);
#endif
}

} // namespace BIEngine
