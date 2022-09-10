#pragma once

#include <string>

namespace BIEngine
{

	//�������� �� ������ � ������. 
	//�� ����� ���� ������ ������� � ����������� ����� ������, ������� ��������� ���� �������� ����� ��� stdout � stderr ���� ���������, ����� ��� ������� � �����-�� ������ ����� 
	class Logger
	{
	public:
		enum class LogType
		{
			MESSAGE,
			WARNING,
			ERROR
		};

		static void WriteLog(LogType type, const std::string& msg);
		static void Flush();
	};

}