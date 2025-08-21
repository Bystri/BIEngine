#pragma once

#include "../StdLib/String.h"

namespace BIEngine {

// Отвечает за работу с логами.
// На самом деле запись ведется в стандартный поток вывода, поэтому требуется явно изменить вывод для stdout и stderr если требуется, чтобы лог писался в какое-то особое место
class Logger {
public:
   enum class LogType {
      MESSAGE,
      WARNING,
      ERROR
   };

   static void WriteLog(LogType type, const String& msg);
   static void WriteLog(LogType type, const char* format, ...);
   static void WriteVLog(LogType type, const char* format, va_list args);

   static void WriteMsgLog(const char* format, ...);
   static void WriteWarningLog(const char* format, ...);
   static void WriteErrorLog(const char* format, ...);

   static void Flush();
};

} // namespace BIEngine
