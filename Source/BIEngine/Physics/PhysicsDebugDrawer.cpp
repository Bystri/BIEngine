#include "PhysicsDebugDrawer.h"

#include "../EngineCore/GameApp.h"
#include "../Utilities/Logger.h"
#include "../Utilities/DebugDraw.h"
#include "../UserInterface/HumanView.h"

namespace BIEngine {

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
   btVector3 const startPoint = PointOnB;
   btVector3 const endPoint = PointOnB + normalOnB * distance;

   drawLine(startPoint, endPoint, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
   Logger::WriteLog(Logger::LogType::WARNING, warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
   // TODO: Add 3d Text
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
   m_DebugModes = (DebugDrawModes)debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
   return m_DebugModes;
}

void BulletDebugDrawer::ReadOptions()
{
   int debugModes = btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE;

   setDebugMode(debugModes);
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
   SharedPtr<HumanView> humanView = g_pApp->TryGetHumanView(0);
   if (humanView == nullptr) {
      return;
   }

   glm::vec3 vec3From, vec3To;
   vec3From.x = from.x();
   vec3From.y = from.y();
   vec3From.z = from.z();

   vec3To.x = to.x();
   vec3To.y = to.y();
   vec3To.z = to.z();

   ColorRgba color;

   color.r = lineColor.x();
   color.g = lineColor.y();
   color.b = lineColor.z();

   DebugDraw::Line(vec3From, vec3To, color);
}

} // namespace BIEngine
