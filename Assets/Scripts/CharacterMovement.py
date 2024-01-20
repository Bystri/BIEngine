import BIEActor
import BIEProcess
import BIEVector

import math
from typing import cast

class MoveProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.physics3DComponent = cast(BIEActor.Physics3DComponent, movableActor.GetComponent("Physics3DComponent"))
        
        self.angularSpeedAngle = 60
        self.currentAngle = 0
        self.speed = 5

        
    def OnUpdate(self, dt):
        self.currentAngle = self.currentAngle + self.angularSpeedAngle * dt 

        newX = self.speed * math.cos(math.radians(self.currentAngle))
        newY = self.speed * math.sin(math.radians(self.currentAngle))
        vecVel = BIEVector.Vec3(newX, newY, 0)

        self.physics3DComponent.SetVelocity(vecVel);


class CharacterMovement():
    def __init__(self):
        pass
        
    def Clear(self):
        pass
        
    def PostInit(self):
        self.proc = MoveProcess(self.owner)
        BIEProcess.AttachProcess(self.proc)