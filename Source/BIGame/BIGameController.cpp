#include "BIGameController.h"

bool BIGameController::OnKeyDown(int key, int scancode)
{
    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = true;

    return true;
}

bool BIGameController::OnKeyUp(int key, int scancode)
{
    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = false;

    return true;
}
