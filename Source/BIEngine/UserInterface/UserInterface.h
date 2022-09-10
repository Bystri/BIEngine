#pragma once

#include <map>
#include <string>

namespace BIEngine
{
	class IElementUI
	{
		friend class UserInterface;
	public:
		void SetCoord(float x, float y, float scale)
		{
			m_x = x;
			m_y = y;
			m_scale = scale;
		}

		float GetX() const { return m_x; }
		float GetY() const { return m_y; }
		float GetScale() const { return m_scale; }

		virtual void OnRender(float dt) = 0;

	protected:
		//Только UserInterface класс может создавать и удалять элементы интерфейса
		explicit IElementUI(int id) : m_id(id), m_x(0.0f), m_y(0.0f), m_scale(0.0f) {}

	protected:
		int m_id;

		float m_x;
		float m_y;
		float m_scale;
	};

	class Text : public IElementUI
	{
		friend class UserInterface;
	public:
		void SetText(const std::string& text) { m_text = text; }

		virtual void OnRender(float dt);

	private:
		explicit Text(int id);

	private:
		std::string m_text;
	};

	class UserInterface
	{
	public:
		UserInterface();
		UserInterface(const UserInterface& orig) = delete;
		UserInterface& operator=(const UserInterface& rhs) = delete;

		bool Init(unsigned int width, unsigned int height);
		void Shutdown();

		int CreateStatic(float x, float y, float scale, const std::string& text);
		Text* GetStatic(int id);

		void OnRender(float dt);

	private:
		//Хранит ID, который будет присвоен следующему элементу
		int m_nextId;

		std::map<int, IElementUI* const> m_elements;
	};

}