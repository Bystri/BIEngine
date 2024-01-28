#include "Logger.h"

#include <iostream>
#include <cassert>
#include <cstdarg>

namespace BIEngine {

void Logger::WriteLog(LogType type, const std::string& msg)
{
   switch (type) {
      case LogType::MESSAGE:
         std::cout << "[MESSAGE]: " << msg << L"\n";
         break;
      case LogType::WARNING:
         std::cout << "[WARNING]: " << msg << "\n";
         break;
      case LogType::ERROR:
         // Предполагается, что ошибка это настолько ужасная вещь, что вскоре может произойти падение программы, поэтому мы сразу записываем данные из буфера.
         std::cerr << "[ERROR]: " << msg << std::endl;
         break;
      default:
         assert(0 && "Unknown message type");
         break;
   }
}

void Logger::WriteLog(LogType type, const char* format, ...)
{
   static constexpr int MSG_SIZE = 512;
   char msg[MSG_SIZE];
   std::va_list argList;
   va_start(argList, format);
   int len = vsnprintf(msg, MSG_SIZE, format, argList);
   if (len >= MSG_SIZE) {
      len = MSG_SIZE - 1;
      msg[MSG_SIZE - 1] = '\0';

      const std::string errorMessage = "Log message was truncated";
      WriteLog(LogType::ERROR, errorMessage);
   }
   va_end(argList);

   const std::string meesage(msg, len);
   WriteLog(type, meesage);
}

void Logger::WriteMsgLog(const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteLog(LogType::MESSAGE, format, argList);
   va_end(argList);
}

void Logger::WriteErrorLog(const char* format, ...)
{
   std::va_list argList;
   va_start(argList, format);
   WriteLog(LogType::ERROR, format, argList);
   va_end(argList);
}

void Logger::Flush()
{
   std::cout.flush();
}

} // namespace BIEngine
