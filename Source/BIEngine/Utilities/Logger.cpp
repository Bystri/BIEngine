#include "Logger.h"

#include <iostream>
#include <cassert>

namespace BIEngine
{

	void Logger::WriteLog(LogType type, const std::string& msg)
	{
		switch (type)
		{
		case LogType::MESSAGE:
			std::cout << "[MESSAGE]: " << msg << L"\n";
			break;
		case LogType::WARNING:
			std::cout << "[WARNING]: " << msg << "\n";
			break;
		case LogType::ERROR:
			//Предполагается, что ошибка это настолько ужасная вещь, что вскоре может произойти падение программы, поэтому мы сразу записываем данные из буфера.
			std::cerr << "[ERROR]: " << msg << std::endl;
			break;
		default:
			assert(0 && "Unknown message type");
			break;
		}
	}

	void Logger::Flush()
	{
		std::cout.flush();
	}
}