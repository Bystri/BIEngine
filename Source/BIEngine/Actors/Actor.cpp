#include "Actor.h"

#include <iostream>
#include <string>
#include <cassert>

#include "../Utilities/Logger.h"
#include "ActorFactory.h"

namespace BIEngine
{

	Actor::Actor(ActorId id) 
	{
		m_id = id;
	}

	Actor::~Actor() 
	{
		std::wstring msg;
		Logger::WriteLog(Logger::LogType::MESSAGE, "Actor: Destroying Actor " + std::to_string(m_id));
	}

	bool Actor::Init(tinyxml2::XMLElement* pData) 
	{
		if (!pData)
			return false;

		const char* actorName;
		pData->QueryStringAttribute("name", &actorName);
		m_name = actorName;
		
		return true;
	}

	void Actor::PostInit() 
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
			it->second->PostInit();
	}

	void Actor::Destroy() 
	{
		m_components.clear();
	}

	void Actor::Update(double dt)
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
			it->second->Update(dt);
	}

	void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent) 
	{
		std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->GetComponentId(), pComponent));
		assert(success.second);
	}

	tinyxml2::XMLElement* Actor::ToXML(tinyxml2::XMLDocument* pDoc) const
	{
		if (!pDoc)
			return nullptr;

		//Создание и присоединение основного корня XML-элемента
		tinyxml2::XMLElement* pActorElement = pDoc->NewElement("Actor");
		pActorElement->SetAttribute("name", m_name.c_str());
		pDoc->LinkEndChild(pActorElement);

		//Создание и присоединение XML-элементов компонентов 
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			auto pComponent = it->second;
			tinyxml2::XMLElement* pComponentElement = pComponent->GenerateXml(pDoc);
			pActorElement->LinkEndChild(pComponentElement);
		}

		return pActorElement;
	}
}