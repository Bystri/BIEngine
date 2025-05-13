import BIEActor
import BIGActor

from CharacterCommands.CharacterCommand import CharacterCommand

class TurnCommand(CharacterCommand):
    def __init__(self, actor : BIEActor.Actor, desiredDir):
        CharacterCommand.__init__(self, actor)
        
        self.locomotionInfoComponent = actor.GetComponent("LocomotionInfoComponent")
        self.desiredDir = desiredDir
        
    def OnUpdate(self, dt):
        self.locomotionInfoComponent.SetInputDir(self.desiredDir)
        self.Succeed()