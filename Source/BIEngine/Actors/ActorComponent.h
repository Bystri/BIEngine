#pragma once

#include <memory>
#include <string>

#include <tinyxml2.h>

namespace BIEngine
{

	typedef std::string ComponentId;

	class Actor;

	//ќсновной класс дл€ создани€ всех необходимых компонентов, наполн€ющих актеров жизнью
	class ActorComponent
	{
		friend class ActorFactory;

	public:
		virtual ~ActorComponent() {}

		virtual bool Init(tinyxml2::XMLElement* pData) = 0;
		virtual void PostInit() { }

		virtual void Update(float dt) {}

		virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) = 0;

		virtual ComponentId GetComponentId() const = 0;

	private:
		void SetOwner(std::shared_ptr<Actor> pOwner) { m_pOwner = pOwner; }

	protected:
		std::shared_ptr<Actor> m_pOwner;
	};
}