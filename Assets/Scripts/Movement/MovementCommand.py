import BIEActor

from CharacterCommands.CharacterCommand import CharacterCommand

class MovementCommand(CharacterCommand):
    def __init__(self, actor : BIEActor.Actor, inputVector):
        CharacterCommand.__init__(self, actor)
        
        self.characterMovementComponent = self.GetOwner().GetComponent("CharacterMovementComponent")
        self.inputVector = inputVector
        
    def OnUpdate(self, dt):
        self.characterMovementComponent.GetObject().Move(self.inputVector, dt)
        self.Succeed()