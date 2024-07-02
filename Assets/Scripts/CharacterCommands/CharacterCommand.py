import BIEProcess
import BIEActor

class CharacterCommand(BIEProcess.Process):
    def __init__(self, actor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        self.owner = actor
        self.isInterruptible = True
        
    def GetOwner(self):
        return self.owner
        
    def IsInterruptible(self):
        return self.isInterruptible