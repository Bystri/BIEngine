import BIEActor
import BIEProcess
import BIEVector

import math
from typing import cast

import MovementProducer

class MoveProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor, movementProducer : MovementProducer.MovementProducer):
        BIEProcess.Process.__init__(self)
        
        self.physics3DComponent = cast(BIEActor.Physics3DComponent, movableActor.GetComponent("Physics3DComponent"))
        self.speed = 5.0
        
        movementProducer.Init(movableActor)
        self.movementProducer = movementProducer
        
    def __del__(self):
        self.movementProducer.Terminate()

    def OnUpdate(self, dt):
        inputVec = self.movementProducer.GetVelocity() * self.speed
         
        if inputVec.Length() > 0.001:
            self.physics3DComponent.SetVelocity(inputVec)


class CharacterMovement():
    def __init__(self):
        pass 
        
    def Clear(self):
        pass
        
    def OnInit(self):
        self.proc = MoveProcess(self.owner, self.movementProducer)    
        BIEProcess.AttachProcess(self.proc)
        
    def OnActivate(self):
        self.proc.UnPause()
        
    def OnDeactivate(self):
        self.proc.Pause()
        
    def OnTerminate(self):
        self.proc.Success()