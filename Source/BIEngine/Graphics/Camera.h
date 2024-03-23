#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace BIEngine {

class Scene;

class Camera {
public:
   enum class ProjectionType {
      ORTHO,
      PERSPECTIVE
   };

   explicit Camera(glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
      : m_projType(ProjectionType::PERSPECTIVE)

        ,
        m_position(glm::vec3(0.0f)), m_forward(glm::vec3(0.0f, 0.0f, -1.0f)), m_up(worldUp), m_right(glm::vec3(1.0f, 0.0f, 0.0f)), m_worldUp(worldUp)

        ,
        m_rotationY(0.0f), m_rotationZ(0.0f)

        ,
        m_fov(45.0f)

        ,
        m_aspectRatio(800.0f / 600.0f)

        ,
        m_near(0.1f), m_far(1000.0f)
   {
      updateCameraVectors();
   }

   Camera(float upX, float upY, float upZ)
      : Camera(glm::vec3(upX, upY, upZ))
   {
   }

   glm::mat4 GetViewMatrix() const;
   glm::mat4 GetProjMatrix() const;

   const glm::vec3& GetPosition() const { return m_position; }

   const glm::vec3& GetForward() const { return m_forward; }

   const glm::vec3& GetUp() const { return m_up; }

   const glm::vec3& GetRight() const { return m_right; }

   float GetRotationY() const { return m_rotationY; }

   float GetRotationZ() const { return m_rotationZ; }

   void LookAt(const glm::vec3& eyePos, const glm::vec3& forward, const glm::vec3& up);

   void MoveTo(const glm::vec3& pos);
   void MoveRelForward(const float posDelta);
   void MoveRelSide(const float posDelta);
   void SetRotationY(float rotation);
   void SetRotationZ(float rotation);
   void TurnRelAroundY(const float turnDelta);
   void TurnRelAroundZ(const float turnDelta);

   void SetFov(const float fov) { m_fov = fov; }

   float GetFov() const { return m_fov; }

   void SetAspectRatio(const float aspectRatio);

   float GetAspectRatio() const { return m_aspectRatio; }

   void SetNear(const float near);

   float GetNear() const { return m_near; }

   void SetFar(const float far);

   float GetFar() const { return m_far; }

private:
   void updateCameraVectors();
   void updateCameraVals();

private:
   ProjectionType m_projType;

   glm::vec3 m_position;
   glm::vec3 m_forward;
   glm::vec3 m_up;
   glm::vec3 m_right;
   glm::vec3 m_worldUp;

   float m_rotationY;
   float m_rotationZ;

   float m_fov;

   float m_aspectRatio;

   float m_near;
   float m_far;
};

} // namespace BIEngine
