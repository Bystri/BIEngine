#include "LuaStateManager.h"

#include <iostream>

#include "../Utilities/Logger.h"

namespace BIEngine
{

    LuaStateManager* LuaStateManager::s_pSingleton = nullptr;

    bool LuaStateManager::Create()
    {
        if (s_pSingleton)
        {
            Logger::WriteLog(Logger::LogType::ERROR, "Overwriting LuaStateManager singleton");
            if (s_pSingleton)
            {
                delete s_pSingleton;
                s_pSingleton = nullptr;
            }
        }

        s_pSingleton = new LuaStateManager();
        if (s_pSingleton)
            return s_pSingleton->Init();

        return false;
    }

    void LuaStateManager::Destroy()
    {
        assert(s_pSingleton);
        if (s_pSingleton)
        {
            delete s_pSingleton;
            s_pSingleton = nullptr;
        }
    }

    LuaStateManager::LuaStateManager()
        : m_pLuaState(nullptr)
    {
    }

    LuaStateManager::~LuaStateManager()
    {
        if (m_pLuaState)
        {
            LuaPlus::LuaState::Destroy(m_pLuaState);
            m_pLuaState = nullptr;
        }
    }

    bool LuaStateManager::Init()
    {
        m_pLuaState = LuaPlus::LuaState::Create(true);
        if (m_pLuaState == nullptr)
            return false;

        //������������ �������, ������� ����� ���������� �� �������
        m_pLuaState->GetGlobals().RegisterDirect("ExecuteFile", (*this), &LuaStateManager::ExecuteFile);
        m_pLuaState->GetGlobals().RegisterDirect("ExecuteString", (*this), &LuaStateManager::ExecuteString);

        return true;
    }

    void LuaStateManager::ExecuteFile(const char* path)
    {
        int result = m_pLuaState->DoFile(path);
        if (result != 0)
            SetError(result);
    }

    void LuaStateManager::ExecuteString(const char* chunk)
    {
        int result = 0;

        //���������� ������ �������� � �������������
        if (strlen(chunk) <= 1 || chunk[0] != '=')
        {
            result = m_pLuaState->DoString(chunk);
            if (result != 0)
                SetError(result);
        }
        //���� ������ ���������� � ������� '=', ����������� lua-��� � ������� print()
        else
        {
            std::string buffer("print(");
            buffer += (chunk + 1);
            buffer += ")";
            result = m_pLuaState->DoString(buffer.c_str());
            if (result != 0)
                SetError(result);
        }
    }

    void LuaStateManager::SetError(int errorNum)
    {
        LuaPlus::LuaStackObject stackObj(m_pLuaState, -1);
        const char* errStr = stackObj.GetString();
        if (errStr)
        {
            m_lastError = errStr;
            ClearStack();
        }
        else
            m_lastError = "Unknown Lua parse error";

        Logger::WriteLog(Logger::LogType::ERROR, m_lastError);
    }

    void LuaStateManager::ClearStack()
    {
        m_pLuaState->SetTop(0);
    }

    LuaPlus::LuaObject LuaStateManager::GetGlobalVars()
    {
        assert(m_pLuaState);
        return m_pLuaState->GetGlobals();
    }

    LuaPlus::LuaState* LuaStateManager::GetLuaState() const
    {
        return m_pLuaState;
    }

    void LuaStateManager::ConvertVec3ToTable(const glm::vec3& vec, LuaPlus::LuaObject& outLuaTable) const
    {
        outLuaTable.AssignNewTable(GetLuaState());
        outLuaTable.SetNumber("x", vec.x);
        outLuaTable.SetNumber("y", vec.y);
        outLuaTable.SetNumber("z", vec.z);
    }

    void LuaStateManager::ConvertTableToVec3(const LuaPlus::LuaObject& luaTable, glm::vec3& outVec3) const
    {
        LuaPlus::LuaObject temp;

        temp = luaTable.Get("x");
        if (temp.IsNumber())
            outVec3.x = temp.GetFloat();
        else
            std::cerr << "luaTable.x is not a number" << std::endl;

        temp = luaTable.Get("y");
        if (temp.IsNumber())
            outVec3.y = temp.GetFloat();
        else
            std::cerr << "luaTable.y is not a number" << std::endl;

        temp = luaTable.Get("z");
        if (temp.IsNumber())
            outVec3.z = temp.GetFloat();
        else
            std::cerr << "luaTable.z is not a number" << std::endl;
    }

}