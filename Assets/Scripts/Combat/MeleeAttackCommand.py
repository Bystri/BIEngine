import BIEActor

from CharacterCommands.CharacterCommand import CharacterCommand

class MeleeAttackCommand(CharacterCommand):
    def __init__(self, actor : BIEActor.Actor):
        CharacterCommand.__init__(self, actor)
        
        self.combatControllerComponent = self.GetOwner().GetComponent("CombatControllerComponent")
        self.combatStateComponent = self.GetOwner().GetComponent("CombatStateComponent")
        self.startAttack = False
        self.isInterruptible = False
        
    def OnUpdate(self, dt):
        if not self.startAttack:
            self.startAttack = True
            self.combatControllerComponent.RequestMeleeAttack()
            return
            
        if not self.combatStateComponent.IsAttackInProgress():
            self.Succeed()