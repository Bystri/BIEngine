import MovementProducer

import BIEActor
import BIEEvent
import BIEVector
import BIGInputEvent

from typing import cast

class InputMovementProducer(MovementProducer.MovementProducer):
    def Init(self, owner : BIEActor.Actor):
        super().Init(owner)
        
        self.up = False
        self.down = False
        self.left = False
        self.right = False
        
        self.OnKeyDownCallback = lambda eventData : self.OnKeyDown(eventData)
        self.keyDownListenerHandler = BIEEvent.RegisterEventListener(BIGInputEvent.EvtData_OnKeyDown.eventType, self.OnKeyDownCallback)
        
        self.OnKeyUpCallback = lambda eventData : self.OnKeyUp(eventData)
        self.keyUpListenerHandler = BIEEvent.RegisterEventListener(BIGInputEvent.EvtData_OnKeyUp.eventType, self.OnKeyUpCallback)
        
    def Terminate(self):
        BIEEvent.RemoveEventListener(self.keyDownListenerHandler)
        BIEEvent.RemoveEventListener(self.keyUpListenerHandler)
        
        super().Terminate()
        
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
            
    def GetVelocity(self):
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
            
        return res