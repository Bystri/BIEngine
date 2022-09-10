#pragma once

#include <string>
#include <cassert>

#include <LuaPlus/LuaPlus.h>
#include <glm/glm.hpp>

namespace BIEngine
{

	class LuaStateManager
	{
	public:
		//������������� ���������� �������� 
		static bool Create();
		//����������� ���������� ��������
		static void Destroy();
		static LuaStateManager* Get() { assert(s_pSingleton); return s_pSingleton; }

		bool Init();
		//��������� ��������� lua-������
		//����� ���� ������ ������ ������ �������
		void ExecuteFile(const char* resource);
		//��������� ��������� lua-���
		//����� ���� ������ ������ ������ �������
		void ExecuteString(const char* str);

		LuaPlus::LuaObject GetGlobalVars();
		LuaPlus::LuaState* GetLuaState() const;

		void ConvertVec3ToTable(const glm::vec3& vec, LuaPlus::LuaObject& outLuaTable) const;
		void ConvertTableToVec3(const LuaPlus::LuaObject& luaTable, glm::vec3& outVec) const;

	private:
		void SetError(int errorNum);
		void ClearStack();

		//������������ �������� ��-�� ����, ��� LuaStateManager ���������� � ������� �������� "��������"
		explicit LuaStateManager();
		~LuaStateManager();

	private:
		static LuaStateManager* s_pSingleton;
		LuaPlus::LuaState* m_pLuaState;
		std::string m_lastError;
	};

}