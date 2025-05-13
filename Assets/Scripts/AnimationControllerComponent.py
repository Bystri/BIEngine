import BIEActor
import BIEProcess
import BIEVector

import BIGActor

import math
from typing import cast

class AnimationControllerProcess(BIEProcess.Process):
    def __init__(self, movableActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.locomotionInfoComponent = cast(BIGActor.LocomotionInfoComponent, movableActor.GetComponent("LocomotionInfoComponent"))
        self.transformComponent = cast(BIEActor.TransformComponent, movableActor.GetComponent("TransformComponent"))
        self.animationComponent = cast(BIEActor.AnimationComponent, movableActor.GetComponent("AnimationComponent"))
        self.meleeAttackComponent = movableActor.GetComponent("MeleeAttackComponent").GetObject()
        
        self.isRunningFront = True
        self.isRunningBack = True
        self.isRunningRight = True
        self.isRunningLeft = True
        
    def OnUpdate(self, dt):
        currentVel2d = BIEVector.Vec2(self.locomotionInfoComponent.GetCurrentVel().x, self.locomotionInfoComponent.GetCurrentVel().z)
        if currentVel2d.Length() > 0.001 and not self.meleeAttackComponent.IsAttackInProgress():
            currentVel2d = currentVel2d.Normalize()
            dirDot = BIEVector.Dot(self.locomotionInfoComponent.GetCurrentDir(), currentVel2d)
            
            right3d = self.transformComponent.GetRight()
            right2d = BIEVector.Vec2(right3d.x, right3d.z)
            right2d.Normalize()
            
            rightDot = BIEVector.Dot(currentVel2d, right2d)
            if dirDot > 0.707:
                if self.isRunningFront == True:
                    return
                    
                self.animationComponent.PlayAnimation("run")
                
                self.isRunningFront = True
                self.isRunningBack = False
                self.isRunningRight = False
                self.isRunningLeft = False
            elif dirDot < -0.707:
                if self.isRunningBack == True:
                    return
                    
                self.animationComponent.PlayAnimation("walking_backwards")
                
                self.isRunningFront = False
                self.isRunningBack = True
                self.isRunningRight = False
                self.isRunningLeft = False
            elif rightDot > 0.707:
                if self.isRunningRight == True:
                    return
                
                self.animationComponent.PlayAnimation("strafe_right")
                
                self.isRunningFront = False
                self.isRunningBack = False
                self.isRunningRight = True
                self.isRunningLeft = False
                
            else:
                if self.isRunningLeft == True:
                    return
                
                self.animationComponent.PlayAnimation("strafe_left")
                
                self.isRunningFront = False
                self.isRunningBack = False
                self.isRunningRight = False
                self.isRunningLeft = True
        else:
            if self.isRunningFront == True or self.isRunningBack == True or self.isRunningRight == True or self.isRunningLeft == True:
                self.animationComponent.PlayAnimation("idle")
                self.isRunningFront = False
                self.isRunningBack = False
                self.isRunningRight = False
                self.isRunningLeft = False


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