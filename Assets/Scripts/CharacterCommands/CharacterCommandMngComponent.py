import BIEProcess

class CharacterCommandMngComponent():
    def __init__(self):
        self.currentCommand = None 
        
    def OnInit(self):
        pass
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        pass
        
    def OnUpdate(self, dt):
        if self.currentCommand is not None and self.currentCommand.IsDead():
            self.currentCommand = None
        
    def ExecuteCommand(self, command):
        if self.currentCommand is not None:
            if self.currentCommand.IsInterruptible():
                self.currentCommand.Fail()
            else:
                return
            
        self.currentCommand = command
        BIEProcess.AttachProcess(self.currentCommand)