#pragma once

#include <cstdint>
#include <glm/glm.hpp>

// One input sampled for one fixed simulation step. The network event is only
// an adapter for transporting this command.
struct CharacterInputCommand {
   uint32_t sequence = 0;
   glm::vec3 inputVelocity = glm::vec3(0.0f);
   glm::vec2 inputDirection = glm::vec2(0.0f);
};

// Complete state needed to resume a character movement simulation.
struct CharacterMovementState {
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec3 rotation = glm::vec3(0.0f);
   glm::vec3 velocity = glm::vec3(0.0f);
   glm::vec2 direction = glm::vec2(0.0f);
   glm::vec3 inputVelocity = glm::vec3(0.0f);
   glm::vec2 inputDirection = glm::vec2(0.0f);
   float orientation = 0.0f;
   float angularVelocity = 0.0f;
};

struct CharacterMovementSnapshot {
   uint32_t lastProcessedSequence = 0;
   CharacterMovementState state;
};

struct CharacterMovementConfig {
   float maxSpeed = 5.0f;
   float maxAngularSpeed = 2000.0f;
   float maxAcceleration = 10.0f;
   float turnSmoothTime = 0.05f;
};

struct CharacterMovementStep {
   CharacterMovementState state;
   glm::vec3 displacement = glm::vec3(0.0f);
};
