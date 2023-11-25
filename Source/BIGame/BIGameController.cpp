#include "BIGameController.h"

#include <assert.h>

bool BIGameController::OnPointerMove(const Point& mousePos, const int radius)
{
    m_currentPointerPos = mousePos;

    return true;
}


bool BIGameController::IsKeyPressed(int key) const
{
    assert(key >= 0 && key < MAX_NUMBER_OF_KEYS);
    if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
        return false;
    }

    return m_isKeyPressed[key];
}


bool BIGameController::OnKeyDown(int key, int scancode)
{
    assert(key >= 0 && key < MAX_NUMBER_OF_KEYS);
    if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
        return false;
    }

    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = true;

    return true;
}


bool BIGameController::OnKeyUp(int key, int scancode)
{
    assert(key >= 0 && key < MAX_NUMBER_OF_KEYS);
    if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
        return false;
    }

    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = false;

    return true;
}
