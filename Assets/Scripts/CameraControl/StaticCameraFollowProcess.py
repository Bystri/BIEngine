import BIEActor
import BIEProcess
import BIEVector
import BIEScene

import math
from typing import cast

class CameraFollowProcess(BIEProcess.Process):
    def __init__(self, target : BIEActor.Actor, dist : float, height : float):
        BIEProcess.Process.__init__(self)
        
        self.targetTransformComponent = cast(BIEActor.TransformComponent, target.GetComponent("TransformComponent"))
        self.camera = BIEScene.GetCamera()
        self.tForward = BIEVector.Vec3(1.0, 0.0, 0.0)
        self.tUp = BIEVector.Vec3(0.0, 1.0, 0.0)
        self.dist = dist
        self.height = height

    def OnUpdate(self, dt):
        tPos = self.targetTransformComponent.GetPosition()
        
        eyePos = tPos - self.tForward * self.dist + self.tUp * self.height
        camForward = tPos - eyePos
        camForward = camForward.Normalize()
        
        camLeft = BIEVector.Cross(self.tUp, camForward).Normalize()
        camUp = BIEVector.Cross(camForward, camLeft).Normalize()
        
        self.camera.LookAt(eyePos, camForward, camUp)