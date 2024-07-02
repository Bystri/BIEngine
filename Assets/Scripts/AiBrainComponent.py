import BIEActor
import BIEVector

from Movement.MovementCommand import MovementCommand

import random
from typing import cast

class AiBrainComponent():  
    def OnInit(self):
        self.characterCommandMngComponent = self.owner.GetComponent("CharacterCommandMngComponent").GetObject()
        self.transformComponent = cast(BIEActor.TransformComponent, self.owner.GetComponent("TransformComponent"))
        self.navAgentComponent = cast(BIEActor.NavAgentComponent, self.owner.GetComponent("NavAgentComponent"))
        
        self.target = self.transformComponent.GetPosition() 
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        pass
            
    def OnUpdate(self, dt):
        dist = self.transformComponent.GetPosition() - self.target
        
        if dist.Length() < 3:
           self.target = BIEVector.Vec3(random.randint(-20, 20), self.transformComponent.GetPosition().y, random.randint(-20, 20))
           if not self.navAgentComponent.SetDestination(self.target):
              print("can't satisfy request for " + str(self.target.x) + " " + str(self.target.y) + " " + str(self.target.z))
           else:
              print("go to " + str(self.target.x) + " " + str(self.target.y) + " " + str(self.target.z))
              
        res = self.navAgentComponent.GetVelocity()
        
        resLength = res.Length()
        if resLength > 1.0:
            res /= resLength
            
        command = MovementCommand(self.owner, res)
        self.characterCommandMngComponent.ExecuteCommand(command)