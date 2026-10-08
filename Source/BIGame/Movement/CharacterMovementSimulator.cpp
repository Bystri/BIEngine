#include "CharacterMovementSimulator.h"

#include <cmath>
#include <limits>

#include "../../BIEngine/Math/Math.h"

CharacterMovementStep CharacterMovementSimulator::Step(const CharacterMovementState& previous,
                                                       const CharacterInputCommand& command,
                                                       const CharacterMovementConfig& config,
                                                       float dt)
{
   CharacterMovementStep result;
   result.state = previous;
   result.state.inputVelocity = command.inputVelocity;
   result.state.inputDirection = command.inputDirection;

   float orientation = previous.orientation;
   float angularVelocity = previous.angularVelocity;
   if (glm::length(command.inputDirection) > std::numeric_limits<float>::epsilon()) {
      float targetAngle = glm::degrees(std::atan2(-command.inputDirection.y, command.inputDirection.x));
      constexpr float fullAngle = 360.0f;
      if (std::abs(targetAngle - orientation) > std::abs(targetAngle + fullAngle - orientation)) {
         targetAngle += fullAngle;
      }
      if (std::abs(targetAngle - orientation) > std::abs(targetAngle - fullAngle - orientation)) {
         targetAngle -= fullAngle;
      }
      orientation = BIEngine::SmoothDamp(orientation, targetAngle, angularVelocity,
                                         config.turnSmoothTime, dt, config.maxAngularSpeed);
   } else {
      angularVelocity = 0.0f;
   }

   const float orientationRad = orientation * 3.14f / 180.0f;
   result.state.direction = glm::normalize(glm::vec2(std::cos(orientationRad), std::sin(orientationRad)));
   result.state.orientation = orientation;
   result.state.angularVelocity = angularVelocity;
   result.state.rotation = glm::vec3(0.0f, orientation, 0.0f);

   const glm::vec3 desiredVelocity = glm::vec3(command.inputVelocity.x, 0.0f, command.inputVelocity.z) * config.maxSpeed;
   const float maxSpeedChange = config.maxAcceleration * dt;
   result.state.velocity = glm::vec3(
      BIEngine::MoveTowards(previous.velocity.x, desiredVelocity.x, maxSpeedChange),
      0.0f,
      BIEngine::MoveTowards(previous.velocity.z, desiredVelocity.z, maxSpeedChange));
   result.displacement = result.state.velocity * dt;
   result.state.position = previous.position + result.displacement;
   return result;
}
