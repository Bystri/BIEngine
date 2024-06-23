import BIEActor
import BIEProcess
import BIEVector
import BIEMath

import math
from typing import cast

import MovementProducer

class MoveProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor, movementProducer : MovementProducer.MovementProducer):
        BIEProcess.Process.__init__(self)
        
        self.transformComponent = cast(BIEActor.TransformComponent, movableActor.GetComponent("TransformComponent"))
        self.physics3DComponent = cast(BIEActor.Physics3DComponent, movableActor.GetComponent("Physics3DComponent"))
        self.maxSpeed = 5.0
        self.maxAngularSpeed = 360.0
        self.maxAcceleration = 10.0
        self.turnSmoothTime = 0.01
        self.desiredVelocity = BIEVector.Vec3(0.0, 0.0, 0.0)
        self.turnSmoothVelocity = 0.0
        
        movementProducer.Init(movableActor)
        self.movementProducer = movementProducer
        
    def __del__(self):
        self.movementProducer.Terminate()

    def OnUpdate(self, dt):
        inputVec = self.movementProducer.GetVelocity()

        if inputVec.Length() > 0.001:
            targetAngle = math.degrees(math.atan2(-inputVec.z, inputVec.x))
            angle, self.turnSmoothVelocity = BIEMath.SmoothDamp(self.transformComponent.GetRotation().y, targetAngle, self.turnSmoothVelocity, self.turnSmoothTime, dt, self.maxAngularSpeed)
            self.transformComponent.SetRotation(BIEVector.Vec3(0.0, angle, 0.0))
        else:
            self.turnSmoothVelocity = 0.0
            
        self.desiredVelocity = BIEVector.Vec3(inputVec.x, 0.0, inputVec.z) * self.maxSpeed
         
        velocity = self.physics3DComponent.GetVelocity()
        maxSpeedChange = self.maxAcceleration * dt
        velocity.x = BIEMath.MoveTowards(velocity.x, self.desiredVelocity.x, maxSpeedChange)
        velocity.z = BIEMath.MoveTowards(velocity.z, self.desiredVelocity.z, maxSpeedChange)
        self.physics3DComponent.SetVelocity(velocity)


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