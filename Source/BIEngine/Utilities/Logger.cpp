#include "Logger.h"

#include <iostream>
#include <cstdarg>

#include "../StdLib/Assert.h"

namespace BIEngine {

uint32_t Logger::m_frameNum = -1;

void Logger::SetFrameNum(uint32_t num)
{
   m_frameNum = num;
}

void Logger::WriteLog(LogType type, const String& msg)
{
   std::cout << "[Frame:" << m_frameNum << "]";

   switch (type) {
      case LogType::MESSAGE:
         std::cout << "[MESSAGE]: " << msg << "\n";
         break;
      case LogType::WARNING:
         std::cout << "[WARNING]: " << msg << "\n";
         break;
      case LogType::ERROR:
         // Предполагается, что ошибка это настолько ужасная вещь, что вскоре может произойти падение программы, поэтому мы сразу записываем данные из буфера.
         std::cerr << "[ERROR]: " << msg << std::endl;
         break;
      default:
         Assert(false, "Unknown message type for log writing");
         break;
   }
}

void Logger::WriteVLog(LogType type, const char* format, va_list args)
{
   static constexpr int MSG_SIZE = 512;
   char msg[MSG_SIZE];
   int len = vsnprintf(msg, MSG_SIZE, format, args);
   if (len >= MSG_SIZE) {
      len = MSG_SIZE - 1;
      msg[MSG_SIZE - 1] = '\0';

      const String errorMessage = "Log message was truncated";
      WriteLog(LogType::ERROR, errorMessage);
   }
   msg[len] = '\0';

   const String meesage(msg);
   WriteLog(type, meesage);
}

void Logger::WriteLog(LogType type, const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteVLog(type, format, argList);
   va_end(argList);
}

void Logger::WriteMsgLog(const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteVLog(LogType::MESSAGE, format, argList);
   va_end(argList);
}

void Logger::WriteWarningLog(const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteVLog(LogType::WARNING, format, argList);
   va_end(argList);
}

void Logger::WriteErrorLog(const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteVLog(LogType::ERROR, format, argList);
   va_end(argList);
}

void Logger::Flush()
{
   std::cout.flush();
}

} // namespace BIEngine
