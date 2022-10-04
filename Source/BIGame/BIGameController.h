#pragma once

#include "../BIEngine/UserInterface/InputDevices.h"

class BIGameController : public BIEngine::IPointerHandler, public BIEngine::IKeyboardHandler
{
public:
	BIGameController() 
		: m_isKeyPressed()	 
	{
	}

public:
	virtual bool OnPointerMove(const Point& mousePos, const int radius) { return true; }
	virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }
	virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName) { return true; }

	bool OnKeyDown(int key, int scancode);
	bool OnKeyUp(int key, int scancode);

protected:
	//Состояние клавиш клавиатуры
	bool m_isKeyPressed[1024];
};