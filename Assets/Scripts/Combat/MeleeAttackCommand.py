import BIEActor

from CharacterCommands.CharacterCommand import CharacterCommand

class MeleeAttackCommand(CharacterCommand):
    def __init__(self, actor : BIEActor.Actor):
        CharacterCommand.__init__(self, actor)
        
        self.meleeAttackComponent = self.GetOwner().GetComponent("MeleeAttackComponent")
        self.startAttack = False
        self.isInterruptible = False
        
    def OnUpdate(self, dt):
        if not self.startAttack:
            self.startAttack = True
            self.meleeAttackComponent.GetObject().Attack()
            return
            
        if not self.meleeAttackComponent.GetObject().IsAttackInProgress():
            self.Succeed()