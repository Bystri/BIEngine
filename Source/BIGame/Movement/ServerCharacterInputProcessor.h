#pragma once

class CharacterMovementComponent;
class ServerCharacterInputQueue;

class ServerCharacterInputProcessor {
public:
   static void ProcessPending(ServerCharacterInputQueue& queue,
                              CharacterMovementComponent& movement,
                              float fixedDt);
};
