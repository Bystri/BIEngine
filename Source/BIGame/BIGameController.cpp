#include "BIGameController.h"

#include <GLFW/glfw3.h>

#include "../BIEngine/EventManager/EventManager.h"

#include "BIGameEvents.h"

const float PADDLE_X_VELOCITY = 250.0f;


bool BIGameController::OnKeyDown(int key, int scancode)
{
    //��������� ������� ������
    m_isKeyPressed[key] = true;

    //���������� ������ �������� ��� ������� ������, ���� �����
    if (key == GLFW_KEY_A || key == GLFW_KEY_D)
    {
        std::shared_ptr<EvtData_StartPaddleMove> pEvent = std::make_shared<EvtData_StartPaddleMove>(m_firstPlayerActorId, (key == GLFW_KEY_D ? PADDLE_X_VELOCITY : (-PADDLE_X_VELOCITY)));
        BIEngine::EventManager::Get()->QueueEvent(pEvent);
    }

    //���������� ������ �������� ��� ������� ������, ���� �����
    if (key == GLFW_KEY_J || key == GLFW_KEY_L)
    {
        std::shared_ptr<EvtData_StartPaddleMove> pEvent = std::make_shared<EvtData_StartPaddleMove>(m_secondPlayerActorId, (key == GLFW_KEY_L ? PADDLE_X_VELOCITY : (-PADDLE_X_VELOCITY)));
        BIEngine::EventManager::Get()->QueueEvent(pEvent);
    }

    return true;
}

bool BIGameController::OnKeyUp(int key, int scancode)
{
    //��������� ������� ������
    m_isKeyPressed[key] = false;

    //���������� ��������� �������� ��� ������� ������, ���� �����
    if (key == GLFW_KEY_A || key == GLFW_KEY_D)
    {
        std::shared_ptr<EvtData_EndPaddleMove> pEvent = std::make_shared<EvtData_EndPaddleMove>(m_firstPlayerActorId);
        BIEngine::EventManager::Get()->QueueEvent(pEvent);
    }

    //���������� ��������� �������� ��� ������� ������, ���� �����
    if (key == GLFW_KEY_J || key == GLFW_KEY_L)
    {
        std::shared_ptr<EvtData_EndPaddleMove> pEvent = std::make_shared<EvtData_EndPaddleMove>(m_secondPlayerActorId);
        BIEngine::EventManager::Get()->QueueEvent(pEvent);
    }

    return true;
}
