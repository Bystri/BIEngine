import MovementProducer

import BIEActor
import BIEVector

import random
from typing import cast

class AiMovementProducer(MovementProducer.MovementProducer):
    def Init(self, owner : BIEActor.Actor):
        super().Init(owner)
        
        self.transformComponent = cast(BIEActor.TransformComponent, owner.GetComponent("TransformComponent"))
        self.navAgentComponent = cast(BIEActor.NavAgentComponent, owner.GetComponent("NavAgentComponent"))
        
        self.target = self.transformComponent.GetPosition() 
            
    def GetVelocity(self):
        dist = self.transformComponent.GetPosition() - self.target
        
        if dist.Length() < 3:
           self.target = BIEVector.Vec3(random.randint(-45, 45), self.transformComponent.GetPosition().y, random.randint(-45, 45))
           if not self.navAgentComponent.SetDestination(self.target):
              print("can't satisfy request for " + str(self.target.x) + " " + str(self.target.y) + " " + str(self.target.z))
           else:
              print("go to " + str(self.target.x) + " " + str(self.target.y) + " " + str(self.target.z))
              
        res = self.navAgentComponent.GetVelocity()
        
        resLength = res.Length()
        if resLength > 1.0:
            res /= resLength
            
        return res