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
   /*
   TiXmlDocument* optionsDoc = g_pApp->m_Options.m_pDoc;
   TiXmlElement* pRoot = optionsDoc->RootElement();
   if (!pRoot)
           return;

   int debugModes = btIDebugDraw::DBG_NoDebug;
   TiXmlElement* pNode = pRoot->FirstChildElement("PhysicsDebug");
   if (pNode)
   {
           if (pNode->Attribute("DrawWireFrame"))
           {
                   std::string attribute(pNode->Attribute("DrawWireFrame"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawWireframe;
           }

           if (pNode->Attribute("DrawAabb"))
           {
                   std::string attribute(pNode->Attribute("DrawAabb"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawAabb;
           }

           if (pNode->Attribute("DrawFeaturesText"))
           {
                   std::string attribute(pNode->Attribute("DrawFeaturesText"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawFeaturesText;
           }

           if (pNode->Attribute("DrawContactPoints"))
           {
                   std::string attribute(pNode->Attribute("DrawContactPoints"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawContactPoints;
           }

           if (pNode->Attribute("NoDeactivation"))
           {
                   std::string attribute(pNode->Attribute("NoDeactivation"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_NoDeactivation;
           }

           if (pNode->Attribute("NoHelpText"))
           {
                   std::string attribute(pNode->Attribute("NoHelpText"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_NoHelpText;
           }

           if (pNode->Attribute("DrawText"))
           {
                   std::string attribute(pNode->Attribute("DrawText"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawText;
           }

           if (pNode->Attribute("ProfileTimings"))
           {
                   std::string attribute(pNode->Attribute("ProfileTimings"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_ProfileTimings;
           }

           if (pNode->Attribute("EnableSatComparison"))
           {
                   std::string attribute(pNode->Attribute("EnableSatComparison"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_EnableSatComparison;
           }

           if (pNode->Attribute("DisableBulletLCP"))
           {
                   std::string attribute(pNode->Attribute("DisableBulletLCP"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DisableBulletLCP;
           }

           if (pNode->Attribute("EnableCCD"))
           {
                   std::string attribute(pNode->Attribute("EnableCCD"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_EnableCCD;
           }

           if (pNode->Attribute("DrawConstraints"))
           {
                   std::string attribute(pNode->Attribute("DrawConstraints"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawConstraints;
           }

           if (pNode->Attribute("DrawConstraintLimits"))
           {
                   std::string attribute(pNode->Attribute("DrawConstraintLimits"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawConstraintLimits;
           }

           if (pNode->Attribute("FastWireframe"))
           {
                   std::string attribute(pNode->Attribute("FastWireframe"));
                   if (attribute == "yes") debugModes |= btIDebugDraw::DBG_FastWireframe;
           }

           if (debugModes != btIDebugDraw::DBG_NoDebug)
           {
                   setDebugMode(debugModes);
           }
   }
   */

   int debugModes = btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE;

   setDebugMode(debugModes);
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
   std::shared_ptr<HumanView> humanView = g_pApp->TryGetHumanView(0);
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
