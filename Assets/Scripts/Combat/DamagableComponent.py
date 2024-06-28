import BIEActor

class DamagableComponent():
    def __init__(self):
        pass 
        
    def OnInit(self):
        pass
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        pass
        
    def GotDamage(self):
        self.OnDying()
        
    def OnDying(self):
        BIEActor.DestroyActor(self.owner.GetId())
        