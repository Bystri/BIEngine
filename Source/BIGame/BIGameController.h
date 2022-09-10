#pragma once

#include "../BIEngine/UserInterface/InputDevices.h"
#include "../BIEngine/Actors/Actor.h"

class BIGameController : public BIEngine::IPointerHandler, public BIEngine::IKeyboardHandler
{
public:
	BIGameController(BIEngine::ActorId firstPlayerActorId, BIEngine::ActorId secondPlayerActorId) : m_isKeyPressed()
		, m_firstPlayerActorId(firstPlayerActorId), m_secondPlayerActorId(secondPlayerActorId) {}

public:
	virtual bool OnPointerMove(const Point& mousePos, const int radius) { return true; }
	virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }
	virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }

	bool OnKeyDown(int key, int scancode);
	bool OnKeyUp(int key, int scancode);

protected:
	//Состояние клавиш клавиатуры
	bool m_isKeyPressed[1024];

	BIEngine::ActorId m_firstPlayerActorId;
	BIEngine::ActorId m_secondPlayerActorId;
};