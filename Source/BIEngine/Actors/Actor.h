#pragma once

#include <map>
#include <memory>
#include <string>

#include <tinyxml2.h>

#include "ActorComponent.h"


namespace BIEngine
{

	class Actor;

	typedef unsigned long ActorId;

	//������, ������� �������� ������� ����������� ���������� �� �����. �������� ��� �������� �������� � ������� �����������, ������� ����������� � XML-����� ������
	class Actor
	{
		friend class ActorFactory;

		typedef std::map<ComponentId, std::shared_ptr<ActorComponent>> ActorComponents;

	public:
		const static ActorId INBALID_ACTOR_ID = -1;

		explicit Actor(ActorId id);
		~Actor();

		//������������� ������ �� ������ �������� � XML �����
		bool Init(tinyxml2::XMLElement* pData);
		void PostInit();
		void Destroy();

		void Update(int deltaMs);

		tinyxml2::XMLElement* Actor::ToXML(tinyxml2::XMLDocument* pDoc);

		ActorId GetId() const { return m_id; }
		std::string GetName() const { return m_name; }

		//������ ������� ��� ��������� �����������
		template <class ComponentType> std::weak_ptr<ComponentType> GetComponent(ComponentId id) 
		{
			ActorComponents::iterator findIt = m_components.find(id);
			if (findIt != m_components.end()) 
			{
				std::shared_ptr<ActorComponent> pBase(findIt->second);

				//���������� � ��������� ���� ����������
				std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
				std::weak_ptr<ComponentType> pWeakSub(pSub); //���������� �������� ��������� � �������
				return pWeakSub;
			}
			else 
			{
				return std::weak_ptr<ComponentType>();
			}
		}

	private:
		//������ ����� ����� ���������� ������ ��������
		void AddComponent(std::shared_ptr<ActorComponent> pComponent);

	private:
		ActorId m_id;
		std::string m_name; //���� ����� ����� ������������� ��������. ������������ � ���������.
		ActorComponents m_components;
	};

}