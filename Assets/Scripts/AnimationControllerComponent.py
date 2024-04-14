import BIEActor
import BIEProcess
import BIEVector

import math
from typing import cast

import MovementProducer

class AnimationControllerProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.physics3DComponent = cast(BIEActor.Physics3DComponent, movableActor.GetComponent("Physics3DComponent"))
        self.animationComponent = cast(BIEActor.AnimationComponent, movableActor.GetComponent("AnimationComponent"))
        
        self.isRunning = True
        
    def OnUpdate(self, dt):
        if self.physics3DComponent.GetVelocity().Length() > 1.5:
            if self.isRunning == False:
                self.animationComponent.PlayAnimation("run")
                self.isRunning = True
        else:
            if self.isRunning == True:
                self.animationComponent.PlayAnimation("idle")
                self.isRunning = False


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