#pragma once

#include <list>
#include <cassert>

#include "Process.h"

namespace BIEngine
{

	//ƒанный класс реализован за счет шаблона "одиночка" и доступ к нему может быть получен через вызов  ProcessManager::Get()
	//ѕеред использованием класса требуетс€ предварительна€ инициализаци€ через Create()
	class ProcessManager
	{
		typedef std::list<StrongProcessPtr> ProcessList;
		ProcessList m_processList;

		static ProcessManager* s_pSingleton;

	public:
		static bool Create();
		static void Destroy();
		static ProcessManager* Get() { assert(s_pSingleton); return s_pSingleton; }

		//¬ыполн€ет один тик работы дл€ всех активных процессов
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