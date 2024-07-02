import BIEActor
import BIEProcess
import BIEVector
import BIEMath

import math
from typing import cast

class CharacterMovementComponent():
    def __init__(self):
        pass 
        
    def OnInit(self):
        self.transformComponent = cast(BIEActor.TransformComponent, self.owner.GetComponent("TransformComponent"))
        self.physics3DComponent = cast(BIEActor.Physics3DComponent, self.owner.GetComponent("Physics3DComponent"))
        self.maxSpeed = 5.0
        self.maxAngularSpeed = 2000.0
        self.maxAcceleration = 10.0
        self.turnSmoothTime = 0.05
        self.turnSmoothVelocity = 0.0
        
        self.inputVector = BIEVector.Vec3(0.0, 0.0, 0.0)
        self.desiredVelocity = BIEVector.Vec3(0.0, 0.0, 0.0)
        
        # Euler angles are stored as Quat in a transformComponent; A single rotation can have multiple representations in eulerAngles 
        # This way we can get different angles using self.transformComponent.GetRotation()
        # Here we storage for last euler angles for gradual increasing in the future
        self.currentEulerAngles = self.transformComponent.GetRotation()
        
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        pass
        
    def Move(self, inputVector, dt):
        self.inputVector = inputVector
        
    def OnUpdate(self, dt):
        if self.inputVector.Length() > 0.001:
            targetAngle = math.degrees(math.atan2(-self.inputVector.z, self.inputVector.x))
            
            if abs(targetAngle - self.currentEulerAngles.y) > abs(targetAngle + 360.0 - self.currentEulerAngles.y):
                targetAngle += 360.0
                
            if abs(targetAngle - self.currentEulerAngles.y) > abs(targetAngle - 360.0 - self.currentEulerAngles.y):
                targetAngle -= 360.0
            
            angle, self.turnSmoothVelocity = BIEMath.SmoothDamp(self.currentEulerAngles.y, targetAngle, self.turnSmoothVelocity, self.turnSmoothTime, dt, self.maxAngularSpeed)
            self.currentEulerAngles.y = angle
            self.transformComponent.SetRotation(self.currentEulerAngles)
        else:
            self.turnSmoothVelocity = 0.0
            
        self.desiredVelocity = BIEVector.Vec3(self.inputVector.x, 0.0, self.inputVector.z) * self.maxSpeed
         
        velocity = self.physics3DComponent.GetVelocity()
        maxSpeedChange = self.maxAcceleration * dt
        velocity.x = BIEMath.MoveTowards(velocity.x, self.desiredVelocity.x, maxSpeedChange)
        velocity.z = BIEMath.MoveTowards(velocity.z, self.desiredVelocity.z, maxSpeedChange)
        self.physics3DComponent.SetVelocity(velocity)
        
        self.inputVector = BIEVector.Vec3(0.0, 0.0, 0.0)