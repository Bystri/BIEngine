import BIEActor
import BIEEvent
import BIEProcess
import BIGInputEvent

from Combat.DamagableComponent import DamagableComponent

from typing import cast

class ActivateTriggerProcess(BIEProcess.Process):
    def __init__(self, triggerActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.triggerActor = triggerActor

    def OnUpdate(self, dt):
        self.triggerActor.SetActivate(True)
        self.Succeed()


class DeactivateTriggerProcess(BIEProcess.Process):
    def __init__(self, triggerActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        self.triggerActor = triggerActor

    def OnUpdate(self, dt):
        self.triggerActor.SetActivate(False)
        self.Succeed()


class MeleeAttackComponent():
    def __init__(self):
        pass 
        
    def Clear(self):
        pass
        
    def OnInit(self):
        self.OnTriggerEnterCallback = lambda eventData : self.OnTriggerEnter(eventData)
        self.onTriggerEnterListenerHandler = BIEEvent.RegisterEventListener(BIEEvent.EvtData_Phys3DTrigger_Enter.eventType, self.OnTriggerEnterCallback)
        
        self.animationComponent = cast(BIEActor.AnimationComponent, self.owner.GetComponent("AnimationComponent"))
        self.meleeTriggerActor = self.owner.GetActorByPath("MeleeAttackTrigger")
        self.meleeTriggerActor.SetActivate(False)
        
        self.startAttack = None
        self.activateProcess = None
        self.hitWindow = None
        self.deactivateProcess = None      
        
    def OnTerminate(self):
        pass
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTriggerEnter(self, eventData : BIEEvent.BaseEventData):
        onTriggerEnterData = cast(BIEEvent.EvtData_Phys3DTrigger_Enter, eventData)
        collidedActor = BIEActor.GetActor(onTriggerEnterData.GetOtherActor())
        
        if collidedActor.GetId() == self.owner.GetId():
            return
        
        targetDamagableComponent = collidedActor.GetComponent("DamagableComponent")
        if targetDamagableComponent is not None:
            targetDamagableComponent.GetObject().GotDamage()
            
    def OnUpdate(self, dt):
        if self.deactivateProcess is not None and self.deactivateProcess.IsDead():
            self.startAttack = None
            self.activateProcess = None
            self.hitWindow = None
            self.deactivateProcess = None
            
    def IsAttackInProgress(self):
        return self.deactivateProcess is not None
            
    def Attack(self):
        self.startAttack = BIEProcess.DelayProcess(0.75) 
        self.activateProcess = ActivateTriggerProcess(self.meleeTriggerActor)
        self.startAttack.AttachChild(self.activateProcess)
        
        self.hitWindow = BIEProcess.DelayProcess(0.5) 
        self.activateProcess.AttachChild(self.hitWindow)
        
        self.deactivateProcess = DeactivateTriggerProcess(self.meleeTriggerActor)
        self.hitWindow.AttachChild(self.deactivateProcess)
        
        BIEProcess.AttachProcess(self.startAttack)
        
        self.animationComponent.PlayAnimation("2H_Melee_Attack_Chop")