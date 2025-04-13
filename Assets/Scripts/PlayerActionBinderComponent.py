import BIEActor
import BIEEvent
import BIEVector
import BIGActionEvent

from Movement.MovementCommand import MovementCommand
from Combat.MeleeAttackCommand import MeleeAttackCommand

from typing import cast

class PlayerActionBinderComponent():
    def __init__(self):
        self.up = False
        self.down = False
        self.left = False
        self.right = False
        
        self.isNeedMoving = False
        self.isNeedAttack = False
        
    def OnInit(self):
        self.characterCommandMngComponent = self.owner.GetComponent("CharacterCommandMngComponent").GetObject()
        
    def OnActivate(self):
        self.OnMoveCallback = lambda eventData : self.OnMoveDelegate(eventData)
        self.OnMoveListenerHandler = BIEEvent.RegisterEventListener(BIGActionEvent.EvtData_Move.eventType, self.OnMoveCallback)
        
    def OnDeactivate(self):  
        BIEEvent.RemoveEventListener(self.OnMoveListenerHandler)
        
    def OnTerminate(self):
        pass
        
    def OnMoveDelegate(self, eventData : BIEEvent.BaseEventData):
        onMoveData = cast(BIGActionEvent.EvtData_Move, eventData)
        
        playerId = self.owner.GetComponent("PlayerComponent").GetPlayerId()
         
        if onMoveData.GetPlayerId() != playerId:
            return
            
        res = BIEVector.Vec3(onMoveData.GetDesiredHorizontalAmount(), 0.0, onMoveData.GetDesiredVerticalAmount())
        resLength = res.Length()
        if resLength > 0.0:
            res /= resLength
                 
        command = MovementCommand(self.owner, res)
        self.characterCommandMngComponent.ExecuteCommand(command)  
            
    def OnUpdate(self, dt):
        return
        if self.isNeedAttack:
            attackCommand = MeleeAttackCommand(self.owner)
            self.characterCommandMngComponent.ExecuteCommand(attackCommand)
            self.isNeedAttack = False
            return
    
        if not self.isNeedMoving:
            return
    
        vertical = 0
        horizontal = 0
        
        if self.up:
            vertical += 1.0
            
        if self.down:
            vertical -= 1.0
            
        if self.left:
            horizontal -= 1.0
            
        if self.right:
            horizontal += 1.0
            
        res = BIEVector.Vec3(vertical, 0.0, horizontal)
         
        resLength = res.Length()
        if resLength > 1.0:
            res /= resLength
            
        command = MovementCommand(self.owner, res)
        self.characterCommandMngComponent.ExecuteCommand(command)