#ifndef BIEDITORCONTROLLER_H
#define BIEDITORCONTROLLER_H

#include "../BIEngine/UserInterface/InputDevices.h"
#include "../BIEngine/Graphics/SceneNodes.h"
#include "../BIEngine/Actors/Actor.h"

class BIEditorController : public BIEngine::IPointerHandler, public BIEngine::IKeyboardHandler
{
public:
    BIEditorController(std::shared_ptr<BIEngine::Actor> cameraActor) : m_isKeyPressed(),
        m_cameraActor(cameraActor) {}

public:
    virtual bool OnPointerMove(const Point& mousePos, const int radius) { return true; }
    virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }
    virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }

    bool OnKeyDown(int key, int scancode);
    bool OnKeyUp(int key, int scancode);

protected:
    //Состояние клавиш клавиатуры
    bool m_isKeyPressed[1024];

    std::shared_ptr<BIEngine::Actor> m_cameraActor;
};

#endif // BIEDITORCONTROLLER_H
