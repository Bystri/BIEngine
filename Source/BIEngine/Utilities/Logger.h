#pragma once

#include <string>

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

   static void WriteLog(LogType type, const std::string& msg);
   static void WriteLog(LogType type, const char* format, ...);

   static void WriteMsgLog(const char* format, ...);
   static void WriteErrorLog(const char* format, ...);

   static void Flush();
};

} // namespace BIEngine
