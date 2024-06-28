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
        self.maxAngularSpeed = 2000.0
        self.maxAcceleration = 10.0
        self.turnSmoothTime = 0.05
        self.desiredVelocity = BIEVector.Vec3(0.0, 0.0, 0.0)
        self.turnSmoothVelocity = 0.0
        
        # Euler angles are stored as Quat in a transformComponent; A single rotation can have multiple representations in eulerAngles 
        # This way we can get different angles using self.transformComponent.GetRotation()
        # Here we storage for last euler angles for gradual increasing in the future
        self.currentEulerAngles = self.transformComponent.GetRotation()
        
        movementProducer.Init(movableActor)
        self.movementProducer = movementProducer
        
    def __del__(self):
        self.movementProducer.Terminate()

    def OnUpdate(self, dt):
        inputVec = self.movementProducer.GetVelocity()

        if inputVec.Length() > 0.001:
            targetAngle = math.degrees(math.atan2(-inputVec.z, inputVec.x))
            
            if abs(targetAngle - self.currentEulerAngles.y) > abs(targetAngle + 360.0 - self.currentEulerAngles.y):
                targetAngle += 360.0
                
            if abs(targetAngle - self.currentEulerAngles.y) > abs(targetAngle - 360.0 - self.currentEulerAngles.y):
                targetAngle -= 360.0
            
            angle, self.turnSmoothVelocity = BIEMath.SmoothDamp(self.currentEulerAngles.y, targetAngle, self.turnSmoothVelocity, self.turnSmoothTime, dt, self.maxAngularSpeed)
            self.currentEulerAngles.y = angle
            self.transformComponent.SetRotation(self.currentEulerAngles)
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