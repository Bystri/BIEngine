import BIEActor
import BIEProcess

from CameraControl.StaticCameraFollowProcess import CameraFollowProcess

class CameraFollowComponent():
    def __init__(self):
        pass 
        
    def Clear(self):
        pass
        
    def OnInit(self):
        self.proc = CameraFollowProcess(self.owner, self.dist, self.height)    
        BIEProcess.AttachProcess(self.proc)
        
    def OnActivate(self):
        pass
        
    def OnDeactivate(self):
        pass
        
    def OnTerminate(self):
        self.proc.Success()