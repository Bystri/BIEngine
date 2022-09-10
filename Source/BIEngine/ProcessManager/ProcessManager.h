#pragma once

#include <list>
#include <cassert>

#include "Process.h"

namespace BIEngine
{

	//������ ����� ���������� �� ���� ������� "��������" � ������ � ���� ����� ���� ������� ����� �����  ProcessManager::Get()
	//����� �������������� ������ ��������� ��������������� ������������� ����� Create()
	class ProcessManager
	{
		typedef std::list<StrongProcessPtr> ProcessList;
		ProcessList m_processList;

		static ProcessManager* s_pSingleton;

	public:
		static bool Create();
		static void Destroy();
		static ProcessManager* Get() { assert(s_pSingleton); return s_pSingleton; }

		//��������� ���� ��� ������ ��� ���� �������� ���������
		unsigned int UpdateProcesses(unsigned long deltaMs);

		WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);
		void AbortAllProcesses(bool immediate);

		std::size_t GetProcessCount() const { return m_processList.size(); }

	private:
		void ClearAllProcesses();

		explicit ProcessManager();
		~ProcessManager();
	};

}