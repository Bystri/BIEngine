#pragma once

#include "SceneNodes.h"

namespace BIEngine
{

	class DirectionalLightNode : public SceneNode
	{
	public:
		DirectionalLightNode(const ActorId actorId)
			: SceneNode(actorId, RenderLayer::LIGHT)
		{
		}

		virtual ~DirectionalLightNode() {}

		void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
		const glm::vec3& GetAmbient() const { return m_ambient; }

		void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
		const glm::vec3& GetDiffuse() const { return m_diffuse; }

		void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
		const glm::vec3& GetSpecular() const { return m_specular; }

		virtual bool OnRender(Scene* pScene);

	private:
		glm::vec3 direction = glm::vec3(0.0, -1.0f, 0.0f);

		glm::vec3 m_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 m_diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		glm::vec3 m_specular = glm::vec3(0.5f, 0.5f, 0.5f);
	};


	class PointLightNode : public SceneNode
	{
	public:
		PointLightNode(const ActorId actorId)
			: SceneNode(actorId, RenderLayer::LIGHT)
		{
		}

		virtual ~PointLightNode() {}

		void SetAttenuationParams(float constant, float linear, float quadratic) {
			m_attenuationConstant = constant;
			m_attenuationLinear = linear;
			m_attenuationQuadratic = quadratic;
		}

		float GetAttenuationConstant() const { return m_attenuationConstant; }
		float GetAttenuationLinear() const { return m_attenuationLinear; }
		float GetAttenuationQuadratic() const { return m_attenuationQuadratic; }

		void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
		const glm::vec3& GetAmbient() const { return m_ambient; }

		void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
		const glm::vec3& GetDiffuse() const { return m_diffuse; }

		void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
		const glm::vec3& GetSpecular() const { return m_specular; }

		virtual bool OnRender(Scene* pScene);

	private:
		float m_attenuationConstant = 1.0f;
		float m_attenuationLinear = 0.09f;
		float m_attenuationQuadratic = 0.032f;

		glm::vec3 m_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 m_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec3 m_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	};


	class SpotLightNode : public SceneNode
	{
	public:
		SpotLightNode(const ActorId actorId)
			: SceneNode(actorId, RenderLayer::LIGHT)
		{
		}

		virtual ~SpotLightNode() {}

		void SetAttenuationParams(float constant, float linear, float quadratic) {
			m_attenuationConstant = constant;
			m_attenuationLinear = linear;
			m_attenuationQuadratic = quadratic;
		}

		float GetAttenuationConstant() const { return m_attenuationConstant; }
		float GetAttenuationLinear() const { return m_attenuationLinear; }
		float GetAttenuationQuadratic() const { return m_attenuationQuadratic; }

		void SetCutOff(float cutOff) { m_cutOff = cutOff; }
		float GetCutOff() const { return m_cutOff; }

		void SetOuterCutOff(float outerCutOff) { m_outerCutOff = outerCutOff; }
		float GetOuterCutOff() const { return m_outerCutOff; }

		void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
		const glm::vec3& GetAmbient() const { return m_ambient; }

		void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
		const glm::vec3& GetDiffuse() const { return m_diffuse; }

		void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
		const glm::vec3& GetSpecular() const { return m_specular; }

		virtual bool OnRender(Scene* pScene);

	private:
		float m_attenuationConstant = 1.0f;
		float m_attenuationLinear = 0.09f;
		float m_attenuationQuadratic = 0.032f;

		glm::vec3 m_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 m_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec3 m_specular = glm::vec3(1.0f, 1.0f, 1.0f);

		float m_cutOff = glm::cos(glm::radians(12.5f));
		float m_outerCutOff = glm::cos(glm::radians(25.0f));
	};


	class LightManagerNode : public SceneNode
	{
	public:
		LightManagerNode();
		~LightManagerNode();

		virtual bool RenderChildren(Scene* pScene) override;

		virtual bool PreRender(Scene* pScene) override { return true; }
		virtual bool PostRender(Scene* pScene) override { return true; }
	};
}