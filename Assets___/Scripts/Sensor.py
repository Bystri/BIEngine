#Импорт библиотеки, которая занимается управлением акторами (объектами в мире)
#Сразу скажу, что все библиотеки из моего движка имеют приписку BIE
import BIEActor
import BIEDebug
import BIEProcess
import BIEPhysics3D
from typing import cast

import BIEVector
import math

class Message():
    def __init__(self, senderId):
        self.senderId = senderId
        self.graph_list = []
        
class CommunicationProcess(BIEProcess.Process):
    def __init__(self, abonent1, abonent2, g, id1, id2):
        BIEProcess.Process.__init__(self)
        print("Init from ", abonent1.GetId(), " and ", abonent2.GetId())
        self.transformComp1 = abonent1.GetComponent("TransformComponent")
        self.transformComp2 = abonent2.GetComponent("TransformComponent")
        self.g = g
        self.id1 = id1
        self.id2 = id2
        
    def SetGraph(self, g):
        self.g = g
        
    def OnUpdate(self, deltaTime):
        color =  BIEVector.Vec3(0, 0, 1)
    
        if self.g.parent[self.id1] == self.id2 or self.g.parent[self.id2] == self.id1:
            color = BIEVector.Vec3(1, 0, 0)
            BIEDebug.DbgLine(self.transformComp1.GetPosition(), self.transformComp2.GetPosition(), color)

import copy
import random

class Sensor():
    def __init__(self):
        self.Ref = False
        self.E = 100
        self.e = 5
        self.radius = 30
        self.signalEmittersNumber = 8
        self.neighbours = set()
        self.proc = []
        self.g = None
        self.optim = False
        self.maxN = -1
        
    def Clear(self):
        self.neighbours = set()
        self.maxN = -1
        
    def PostInit(self):
        pass
        
    def SetOptimMarker(self, optim):
        self.optim = optim    
        
    def SetRefMarker(self, isRef):
        self.Ref = isRef
        
    def SetGraph(self, g):
        self.g = g
    
    def StartSendingSignal(self):
        if self.Ref == False:
            self.g.AddTry()
        self.SendSignal(self.owner)
        
    def SendSignal(self, sender):
        if self.Ref == True:
            if self.owner.GetId() != sender.GetId():
                self.g.AddComplete()
            return
        
        tryNum = 0
        while self.maxN == -1 or tryNum <= self.maxN:
            rand = random.randint(1,100)
            if rand <= self.g.p:
                break
                
            self.g.AddSend()
            self.g.AddLost()
                
            tryNum += 1
        
        if self.maxN != -1 and tryNum >= self.maxN:
            return
        
        self.g.AddSend()  
        pos = self.owner.GetComponent("TransformComponent").GetPosition()
    
        minLength = 100000000
        nextSensor = None
        
        directions = [[0, 1], [1, 0], [-1, 0], [0, -1]]
        for i in directions:
            emmitX = pos.x + self.radius * i[0]
            emmitY = pos.z + self.radius * i[1]
            
            castFrom = pos
            castTo = BIEVector.Vec3(emmitX, 6, emmitY)
            info = BIEPhysics3D.Raycast(castFrom, castTo)
            
            if info.hitActor != None:
                actPos = info.hitActor.GetComponent("TransformComponent").GetPosition()
                for refPos in self.g.vecPoses:
                    if not self.optim and ((refPos.x < actPos.x or refPos.z < actPos.z) or (pos.x > actPos.x or pos.z > actPos.z)):
                        continue
                    dist = abs(refPos.x - actPos.x) + abs(refPos.z - actPos.z)
                    
                    if dist < minLength:
                        minLength = dist
                        nextSensor = info.hitActor
            
        nextSensor.GetComponent("Sensor").GetObject().SendSignal(self.owner)
            
    
    def CreateProc(self):
        for pr in self.proc:
            pr.SetGraph(copy.copy(self.g))
            BIEProcess.AttachProcess(pr)
            