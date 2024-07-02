import BIEActor
import BIEEvent
import BIEVector
import BIGInputEvent

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
        self.isNeedAttack = False;
        
    def OnInit(self):
        self.characterCommandMngComponent = self.owner.GetComponent("CharacterCommandMngComponent").GetObject()
        
    def OnActivate(self):
        self.OnKeyDownCallback = lambda eventData : self.OnKeyDown(eventData)
        self.keyDownListenerHandler = BIEEvent.RegisterEventListener(BIGInputEvent.EvtData_OnKeyDown.eventType, self.OnKeyDownCallback)
    
        self.OnKeyDownCallback = lambda eventData : self.OnKeyDown(eventData)
        self.keyDownListenerHandler = BIEEvent.RegisterEventListener(BIGInputEvent.EvtData_OnKeyDown.eventType, self.OnKeyDownCallback)
        
        self.OnKeyUpCallback = lambda eventData : self.OnKeyUp(eventData)
        self.keyUpListenerHandler = BIEEvent.RegisterEventListener(BIGInputEvent.EvtData_OnKeyUp.eventType, self.OnKeyUpCallback)
        
    def OnDeactivate(self):
        BIEEvent.RemoveEventListener(self.keyDownListenerHandler)
        BIEEvent.RemoveEventListener(self.keyUpListenerHandler)
        
    def OnTerminate(self):
        pass
        
    def OnKeyDown(self, eventData : BIEEvent.BaseEventData):
        onKeyDownData = cast(BIGInputEvent.EvtData_OnKeyDown, eventData)
        keyCode = onKeyDownData.GetKey()
        
        if keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.I:
            self.up = True 
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.K:
            self.down = True            
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.J:
            self.left = True        
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.L:
            self.right = True 
            
        self.isNeedMoving = self.up or self.down or self.left or self.right
        
        if keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.SPACE:
            self.isNeedAttack = True
            
    def OnKeyUp(self, eventData : BIEEvent.BaseEventData):
        onKeyUpData = cast(BIGInputEvent.EvtData_OnKeyUp, eventData)
        keyCode = onKeyUpData.GetKey()
        
        if keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.I:
            self.up = False
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.K:
            self.down = False           
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.J:
            self.left = False      
        elif  keyCode == BIGInputEvent.EvtData_OnKeyEvent.Key.L:
            self.right = False 
            
        self.isNeedMoving = self.up or self.down or self.left or self.right
            
    def OnUpdate(self, dt):
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