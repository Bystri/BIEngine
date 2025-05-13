#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Scene.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

Camera::Camera(glm::vec3 worldUp /* = glm::vec3(0.0f, 1.0f, 0.0f) */)
   : m_projType(ProjectionType::PERSPECTIVE),
     m_position(glm::vec3(0.0f)), m_forward(glm::vec3(0.0f, 0.0f, -1.0f)), m_up(worldUp), m_right(glm::vec3(1.0f, 0.0f, 0.0f)), m_worldUp(worldUp),
     m_rotationY(0.0f), m_rotationZ(0.0f),
     m_fov(45.0f), m_aspectRatio((float)g_pApp->m_options.screenWidth / (float)g_pApp->m_options.screenHeight),
     m_near(0.1f), m_far(1000.0f)
{
   updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
   return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::GetProjMatrix() const
{
   if (m_projType == ProjectionType::ORTHO) {
      return glm::ortho(0.0f, (float)g_pApp->m_options.screenWidth, (float)g_pApp->m_options.screenHeight, 0.0f, -1.0f, 1.0f);
   }

   return glm::perspective(glm::radians(m_fov), (float)g_pApp->m_options.screenWidth / (float)g_pApp->m_options.screenHeight, m_near, m_far);
}

void Camera::MoveTo(const glm::vec3& pos)
{
   m_position = pos;
}

void Camera::MoveRelForward(const float posDelta)
{
   m_position += m_forward * posDelta;
}

void Camera::MoveRelSide(const float posDelta)
{
   m_position += m_right * posDelta;
}

void Camera::SetRotationY(float rotation)
{
   m_rotationY = rotation;
   updateCameraVectors();
}

void Camera::SetRotationZ(float rotation)
{
   m_rotationZ = rotation;
   updateCameraVectors();
}

glm::vec3 Camera::ScreenToWorldPoint(const glm::vec2& pos) const
{
   const glm::mat4 invVP = glm::inverse(GetProjMatrix() * GetViewMatrix());

   const float ndcX = (pos.x / (float)g_pApp->m_options.screenWidth) * 2.f - 1.f;
   const float ndcY = -(pos.y / (float)g_pApp->m_options.screenHeight) * 2.f + 1.f;

   glm::vec4 worldPos = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
   worldPos /= worldPos.w;

   return worldPos;
}

glm::vec3 Camera::ScreenToViewportPoint(const glm::vec2& pos) const
{
   const glm::mat4 invVP = glm::inverse(GetProjMatrix());

   const float ndcX = (pos.x / (float)g_pApp->m_options.screenWidth) * 2.f - 1.f;
   const float ndcY = -(pos.y / (float)g_pApp->m_options.screenHeight) * 2.f + 1.f;

   glm::vec4 worldPos = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
   worldPos /= worldPos.w;

   return worldPos;
}

void Camera::LookAt(const glm::vec3& eyePos, const glm::vec3& forward, const glm::vec3& up)
{
   m_position = eyePos;
   m_forward = glm::normalize(forward);
   m_up = up;

   updateCameraVals();
}

void Camera::TurnRelAroundY(const float turnDelta)
{
   m_rotationY += turnDelta;
   updateCameraVectors();
}

void Camera::TurnRelAroundZ(const float turnDelta)
{
   m_rotationZ += turnDelta;
   updateCameraVectors();
}

void Camera::updateCameraVectors()
{
   // Calculate forward vector
   const float pitchRads = glm::radians(m_rotationY);
   const float yawRads = glm::radians(m_rotationZ);

   m_forward.x = cos(yawRads) * cos(pitchRads);
   m_forward.y = sin(pitchRads);
   m_forward.z = sin(yawRads) * cos(pitchRads);
   m_forward = glm::normalize(m_forward);

   // Calculate right and up vectors
   m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
   m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Camera::updateCameraVals()
{
   const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
   const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

   m_rotationY = glm::degrees(asin(m_forward.y));
   m_rotationZ = glm::degrees(asin(m_forward.z / (std::sqrt(1 - m_forward.y * m_forward.y))));

   m_right = glm::normalize(glm::cross(m_forward, m_up));
}

void Camera::SetAspectRatio(const float aspectRatio)
{
   m_aspectRatio = aspectRatio;
}

void Camera::SetNear(const float near)
{
   m_near = near;
}

void Camera::SetFar(const float far)
{
   m_far = far;
}

} // namespace BIEngine
