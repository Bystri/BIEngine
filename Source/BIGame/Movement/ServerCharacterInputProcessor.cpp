#include "ServerCharacterInputProcessor.h"

#include "ServerCharacterInputQueue.h"
#include "../CharacterMovementComponent.h"

void ServerCharacterInputProcessor::ProcessPending(ServerCharacterInputQueue& queue,
                                                    CharacterMovementComponent& movement,
                                                    float fixedDt)
{
   CharacterInputCommand command;
   while (queue.PopNext(command)) {
      movement.SimulateInputStep(command, fixedDt);
      queue.MarkProcessed(command.sequence);
   }
}
