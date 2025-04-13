import BIEActor
import BIEProcess
import BIEVector

import math
from typing import cast

class AnimationControllerProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.transformComponent = cast(BIEActor.TransformComponent, movableActor.GetComponent("TransformComponent"))
        self.animationComponent = cast(BIEActor.AnimationComponent, movableActor.GetComponent("AnimationComponent"))
        self.meleeAttackComponent = movableActor.GetComponent("MeleeAttackComponent").GetObject()
        self.cachedPosition = self.transformComponent.GetPosition()
        
        self.isRunning = True
        
    def OnUpdate(self, dt):
        newPosition = self.transformComponent.GetPosition()
        traveledVec = newPosition - self.cachedPosition  
        
        if traveledVec.Length() > 0.001 and not self.meleeAttackComponent.IsAttackInProgress():
            if self.isRunning == False:
                self.animationComponent.PlayAnimation("run")
                self.isRunning = True
        else:
            if self.isRunning == True:
                self.animationComponent.PlayAnimation("idle")
                self.isRunning = False
                
        self.cachedPosition = newPosition


class AnimationControllerComponent():
    def __init__(self):
        pass 
        
    def Clear(self):
        pass
        
    def OnInit(self):
        self.proc = AnimationControllerProcess(self.owner)    
        BIEProcess.AttachProcess(self.proc)
        
    def OnActivate(self):
        self.proc.UnPause()
        
    def OnDeactivate(self):
        self.proc.Pause()
        
    def OnTerminate(self):
        self.proc.Success()