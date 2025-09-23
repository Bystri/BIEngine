import BIEProcess

class CharacterCommandMngComponent():
    def __init__(self):
        self.currentCommands = [] 
        
    def OnInit(self):
        pass
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        pass
        
    def OnUpdate(self, dt):
        newCommandsList = []
        for i in range(len(self.currentCommands)):
            if not self.currentCommands[i].IsDead():
                newCommandsList.append(self.currentCommands[i])
        self.currentCommands = newCommandsList
        
    def ExecuteCommand(self, command):
        #if self.currentCommand is not None:
        #   if self.currentCommand.IsInterruptible():
        #       self.currentCommand.Fail()
        #   else:
        #       return
        self.currentCommands.append(command)
        BIEProcess.AttachProcess(command)