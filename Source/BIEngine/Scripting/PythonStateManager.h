#pragma once

#include <string>
#include <cassert>

#include <glm/glm.hpp>
#include <pybind11/embed.h>
#include <pybind11/operators.h>

namespace py = pybind11;

namespace BIEngine
{

	class PythonStateManager
	{
	public:
		//Инициализация экземпляра одиночки 
		static bool Create();
		//Уничтожение экземпляра одиночки
		static void Destroy();
		static PythonStateManager* Get() { assert(s_pSingleton); return s_pSingleton; }

		bool Init();
		//Выполняет указанный Python-скрипт
		//Может быть вызван внутри самого скрипта
		void ExecuteFile(const char* resource);
		//Выполняет указанный Python-код
		//Может быть вызван внутри самого скрипта
		void ExecuteString(const char* str);

		py::module_& GetMain();

	private:
		//Конструктуры приватны из-за того, что PythonStateManager реализован с помощью паттерна "одиночка"
		explicit PythonStateManager();
		~PythonStateManager();

	private:
		static PythonStateManager* s_pSingleton;
		py::scoped_interpreter* m_pPythonGuard;
		py::module_ m_main;
		py::object m_mainScope;
	};

}