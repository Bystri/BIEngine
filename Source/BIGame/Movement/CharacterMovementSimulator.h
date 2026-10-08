#pragma once

#include "MovementTypes.h"

class CharacterMovementSimulator {
public:
   static CharacterMovementStep Step(const CharacterMovementState& previous,
                                     const CharacterInputCommand& command,
                                     const CharacterMovementConfig& config,
                                     float dt);
};
