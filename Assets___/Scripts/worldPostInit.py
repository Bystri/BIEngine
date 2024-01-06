import BIEActor
import BIEVector
from Sensor import *
from typing import cast

print("World PostInit")

sendCount = 0
failed = 0

class Graph():
    def __init__(self, vertices):
        self.V = vertices
        self.graph = [[0 for column in range(vertices)]
                      for row in range(vertices)]
        self.parent = None
        self.vecPoses = []
        self.sends = 0
        self.lost = 0
        self.suc = 0
        self.p = 100
        self.tryNum = 0
        
    def AddComplete(self):
        self.suc += 1
        
    def AddTry(self):
        self.tryNum += 1
        
    def AddSend(self):
        self.sends += 1
        
    def AddLost(self):
        self.lost += 1
        
    def AddRefPos(self, pos):
        self.vecPoses.append(pos)
 
    # A utility function to print
    # the constructed MST stored in parent[]
    def printMST(self):
        print("Edge \tWeight")
        for i in range(1, self.V):
            print(self.parent[i], "-", i, "\t", self.graph[i][self.parent[i]])
 
    # A utility function to find the vertex with
    # minimum distance value, from the set of vertices
    # not yet included in shortest path tree
    def minKey(self, key, mstSet):
 
        # Initialize min value
        min = sys.maxsize
 
        min_index = -1
        for v in range(self.V):
            if key[v] < min and mstSet[v] == False:
                min = key[v]
                min_index = v
 
        return min_index
 
    # Function to construct and print MST for a graph
    # represented using adjacency matrix representation
    def primMST(self):
 
        # Key values used to pick minimum weight edge in cut
        key = [sys.maxsize] * self.V
        self.parent = [None] * self.V  # Array to store constructed MST
        # Make key 0 so that this vertex is picked as first vertex
        key[0] = 0
        mstSet = [False] * self.V
 
        self.parent[0] = -1  # First node is always the root of
 
        for cout in range(self.V):
 
            # Pick the minimum distance vertex from
            # the set of vertices not yet processed.
            # u is always equal to src in first iteration
            u = self.minKey(key, mstSet)
 
            # Put the minimum distance vertex in
            # the shortest path tree
            mstSet[u] = True
 
            # Update dist value of the adjacent vertices
            # of the picked vertex only if the current
            # distance is greater than new distance and
            # the vertex in not in the shortest path tree
            for v in range(self.V):
 
                # graph[u][v] is non zero only for adjacent vertices of m
                # mstSet[v] is false for vertices not yet included in MST
                # Update the key only if graph[u][v] is smaller than key[v]
                if self.graph[u][v] > 0 and mstSet[v] == False \
                and key[v] > self.graph[u][v]:
                    key[v] = self.graph[u][v]
                    self.parent[v] = u
 
        self.printMST()
        


def L(n, p):
    return n*n*(n-1)/p
    
def Lc(n, p):
    return 3*n*n/p
    
def Lp(n, p):
    return ((n**3) - 3*(n**2) + 2*n)/(6*p) + 80*(n-1)*(n-1)/(p*n*n)-0.0125*n*n/p 
    
def Lco(n, p):
    return (n**3 - n**2)/(2*p) + (5-0.0125*n*n)/p


import numpy as np
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.ticker import MaxNLocator

def Coast():
    g = Graph(25)
    actors=[]
    for p in range(100, 49, -25):

        xpoints = np.array([])
        ypoints = np.array([])

        for n in range (4, 51, 4):
            xpoints = np.append(xpoints, n)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if j==n-1:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()

            print("for p=", g.p/100, " and n=", n)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            ypoints = np.append(ypoints, g.sends) 
            g.lost = 0
            g.sends = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []

        fig, ax = plt.subplots()
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.plot(xpoints, ypoints, label='imitation')
        ax.plot(xpoints, Lco(xpoints, p/100), label='analytics')
        ax.set_xlabel('n')
        ax.set_ylabel('Sends')
        ax.grid(True)
        ax.legend()
        ax.set_title('Sends for p=' + str(g.p/100))
        plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+'coast_p='+ str(g.p/100)+'.png')
           

def Perim():
    g = Graph(25)
    actors=[]
    for p in range(100, 49, -25):

        xpoints = np.array([])
        ypoints = np.array([])

        for n in range (4, 51, 4):
            xpoints = np.append(xpoints, n)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if i==0 or i==n-1 or j==0 or j==n-1:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()

            print("for p=", g.p/100, " and n=", n)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            ypoints = np.append(ypoints, g.sends) 
            g.lost = 0
            g.sends = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []

        fig, ax = plt.subplots()
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.plot(xpoints, ypoints, label='imitation')
        ax.plot(xpoints, Lp(xpoints, p/100), label='analytics')
        ax.set_xlabel('n')
        ax.set_ylabel('Sends')
        ax.grid(True)
        ax.legend()
        ax.set_title('Sends for p=' + str(g.p/100))
        plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+'perim_p='+ str(g.p/100)+'.png')
           

def Claster():
    g = Graph(25)
    actors=[]
    for p in range(100, 49, -25):

        xpoints = np.array([])
        ypoints = np.array([])

        for n in range (4, 51, 4):
            xpoints = np.append(xpoints, n)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if (i+1)%4==0 and (j+1)%4 == 0:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))

                    #actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()

            print("for p=", g.p/100, " and n=", n)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            ypoints = np.append(ypoints, g.sends) 
            g.lost = 0
            g.sends = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []

        fig, ax = plt.subplots()
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.plot(xpoints, ypoints, label='imitation')
        ax.plot(xpoints, Lc(xpoints, p/100), label='analytics')
        ax.set_xlabel('n')
        ax.set_ylabel('Sends')
        ax.grid(True)
        ax.legend()
        ax.set_title('Sends for p=' + str(g.p/100))
        plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+'claster_p='+ str(g.p/100)+'.png')
                   

def SingleRef():
    g = Graph(25)
    actors=[]
    for p in range(100, 49, -25):

        xpoints = np.array([])
        ypoints = np.array([])

        for n in range (4, 51, 4):
            xpoints = np.append(xpoints, n)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if i == n-1 and j == n-1:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))

                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()

            print("for p=", g.p/100, " and n=", n)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            ypoints = np.append(ypoints, g.sends) 
            g.lost = 0
            g.sends = 0
            g.suc = 0
            g.tryNum = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []

        fig, ax = plt.subplots()
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.plot(xpoints, ypoints, label='imitation')
        ax.plot(xpoints, L(xpoints, p/100), label='analytics')
        ax.set_xlabel('n')
        ax.set_ylabel('Sends')
        ax.grid(True)
        ax.legend()
        ax.set_title('Sends for p=' + str(g.p/100))
        plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+'single_left_bot_ref_png_p='+ str(g.p/100)+'.png')
  
#Coast()  
#Perim()  
#Claster()
#SingleRef()

import math
import sys

def LPCoast(p,n,N):
    sum = 0
    for i in range(1, n):
        sum += (1-(1-p**N)**i)
        
    if sum < sys.float_info.epsilon:
        sum = sys.float_info.epsilon   
        
    if (p**N) < sys.float_info.epsilon:
        sum /= sys.float_info.epsilon
    else:
        sum /= (p**N)
    return n*sum*(1-p**N)/(1-p)
    
def EPCoast(p,n,N):
    sum = 0
    for i in range(1, n):
        sum += (1-(1-p**N)**i)/(p**N)
    return 2*sum*(1-p**N)/(n*n-n)
    
def EPFull(p,n):
    return (n*n*n-n*n)/(1-p)*0.5
    
def LPClaster(p,n,N):
    m = (n*n/25)
    return 4*m*(1-p**N)*(15-14*(p**N)+6*(p**(2*N))-(p**(3*N)))/(1-p)
    
def EPClasterFull(p,n):
    m = (n*n/25)
    sum = 0
    for i in range(1, 5):
        sum+=min(i, 5-i)*i/(1-p)
    return 4*m*sum
    
def EPPerim(p,n,N):
    q1 = p**N
    print("a ", str(600*(1-q1)*(n*n*q1*q1+4*q1*(((1-q1)**int(n/2))-n-1)-8+8*((1-q1)**int(n/2)))))
    print("b ", str(((q1*q1*q1)*(n*n*n-3*n*n+2*n))))
    b = math.log(q1)+math.log(q1)+math.log(q1)+math.log(n*n*n-3*n*n+2*n)
    print("b ", str(b))
    print("a ", str((1-q1)*(n*n*q1*q1+4*q1*(((1-q1)**int(n/2))-n-1)-8+8*(1-q1)**int(n/2))))
    a = math.log((1-q1)*(n*n*q1*q1+4*q1*(((1-q1)**int(n/2))-n-1)-8+8*((1-q1)**int(n/2))))
    b = 600*math.exp(a-b)
    print("b ", str(b))
    print("ans ", str(a))
    return b

def EPClaster(p,n,N):
    return (1-p**N)*(15-14*(p**N)+6*(p**(2*N))-(p**(3*N)))/15    
    
#def LPCoast(p,n):
#    return (n*n*n-n*n)/(1-p)*0.5

#def LPPerim(p,n,N):
#    q1 = p**N
#    return (1-q1)*(n*n*q1*q1+4*q1*(((1-q1)**(n/2))-n-1)-8+8*(1-q1)**(n/2))/(q1*q1*q1*(1-p))

def LPPerim(p,n,N):
    sum = 0
    for k in range(1, int(n/2)):
        sum += 4*(n-2*k-1)*(1-((1-p**N)**k))
    if sum < sys.float_info.epsilon:
        sum = sys.float_info.epsilon   
    if ((1-p)*(p**N)) < sys.float_info.epsilon:
        return (1-p**N)*sum  
    return (1-p**N)*sum/((1-p)*(p**N))


def Qbit(x):
    if x == 0:
        return 1-0.5
    elif x == 1:
        return 1-0.95
    else:
        return 1-0.7

def lostAvgCoast(p, n, N): 
    return (((1-(p**N))**n) + n*(p**N) - 1)*100/((n-1)*(p**N))
    
def lostAvgClaster(p, n, N): 
    return 50*(p**N)*(15-14*(p**N)+6*(p**(2*N))-(p**(3*N)))/3

def lostAvgPerim(p, n, N): 
    sum = 0
    for k in range(1, int(n/2)):
        sum += 400*(n-2*k-1)*(1-((1-p**N)**k))/(n*n-4*(n-1))
    return sum

def ImitationBase2(rule, name, lostAvgFunc, LPFunc):
    g = Graph(25)
    actors=[]
    n = 10

    plt.figure(1)
    plt.cla()
    plt.figure(2)
    plt.cla()
    plt.figure(3)
    plt.cla()

    for p in [95, 50, 70]:

        xpoints = np.array([])
        ypoints = np.array([])
        ypoints2 = np.array([])
        ypoints3 = np.array([])
        ypoints4 = np.array([])  
        ypoints5 = np.array([])
        ypoints6 = np.array([]) 

        imitations = 50
        convergateYPointsSends = []
        for im in range(imitations):
            convergateYPointsSends.append(np.array([]))
        
        for maxN in range(2, 11):
            sendsTotal = 0
            lostsTotal = 0
            tryNumTotal = 0
            sucTotal = 0
            
            xpoints = np.append(xpoints, maxN)
            
            for im in range(imitations):
        
                check = 0

                g.vecPoses = []
                for i in range(n):
                    for j in range(n):
                        if rule(i, j, n):
                            actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                            g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                            actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                        else:
                            actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        
                        actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                      
                for act in actors:
                    scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                    sensor = cast(Sensor, scriptComp.GetObject())
                    sensor.SetGraph(g)
                    sensor.maxN = maxN
                    g.p = p
                    
                for act in actors:
                    scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                    sensor = cast(Sensor, scriptComp.GetObject()) 
                    if sensor.Ref == True:
                        continue
                    
                    sensor.StartSendingSignal()
                    for act2 in actors:
                        act2.GetComponent("Sensor").GetObject().Clear()
                        act2.GetComponent("Sensor").GetObject().maxN = maxN
                
                for act in actors:
                    BIEActor.DestroyActor(act)
                
                sendsTotal += g.sends
                lostsTotal += g.lost
                tryNumTotal += g.tryNum
                sucTotal += g.suc
                g.lost = 0
                g.sends = 0
                g.tryNum = 0
                g.suc = 0
                
                curLost = lostsTotal/(im+1)
                curSend = sendsTotal/(im+1)
                curTryNum = tryNumTotal/(im+1)
                curSuc = sucTotal/(im+1)
                convergateYPointsSends[im] = np.append(convergateYPointsSends[im], curSend)
                   
                
                actors = []
            
            curLost = lostsTotal/imitations
            curSend = sendsTotal/imitations
            curTryNum = tryNumTotal/imitations
            curSuc = sucTotal/imitations
            ypoints = np.append(ypoints, lostAvgFunc(1-p/100, n, maxN)) 
            ypoints2 = np.append(ypoints2, (curTryNum-curSuc)*100/curTryNum)
            ypoints3 = np.append(ypoints3, curSend)
            ypoints4 = np.append(ypoints4, LPFunc(1-p/100, n, maxN))
            ypoints5 = np.append(ypoints5, curSend*100/LPFunc(1-p/100, n,10))
            ypoints6 = np.append(ypoints6, LPFunc(1-p/100, n, maxN)*100/LPFunc(1-p/100, n, 10))
            
        
        print("Try draw for ", p)
        plt.figure(1)
        plt.plot(xpoints, ypoints, label='for p='+str(p/100) + ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints2, label='for p='+str(p/100)+ ' imitation')
        plt.figure(2)
        plt.plot(xpoints, ypoints4, label='for p='+str(p/100)+ ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints3, label='for p='+str(p/100)+ ' imitation')
        plt.figure(3)
        plt.plot(xpoints, ypoints6, label='for p='+str(p/100)+ ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints5, label='for p='+str(p/100)+ ' imitation')
        plt.figure(4)
        plt.cla()
        plt.plot(xpoints, ypoints4, label='for p='+str(p/100) + ' analytics')
        for pl in range(0, imitations, 10):
            plt.plot(xpoints, convergateYPointsSends[pl], label='for imitation='+str(pl+1), linestyle='dashed')
        ax = plt.gca()
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.set_xlabel('N')
        ax.set_ylabel('Lp')
        ax.grid(True)
        ax.legend()
        ax.set_title('Среднее число пересылок (сходимость)')
        plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+name+'_conv_p_'+str(p/100)+'_avg_resends.png')


    plt.figure(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('EpsP')
    ax.grid(True)
    ax.legend()
    ax.set_title('Средний процент потерянных сообщений')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+name+'_avg_lost_percent.png')
    plt.figure(2)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Lp')
    ax.grid(True)
    ax.legend()
    ax.set_title('Среднее число перепосылок')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+name+'_avg_resends.png')
    plt.figure(3)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Ep(%)')
    ax.grid(True)
    ax.legend()
    ax.set_title('Удельные энергозатры')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\'+name+'_powercost.png')

def CoastRule(i, j, n):
    return j==n-1
    
def Coast2Impl():
    ImitationBase2(CoastRule,'coast2', lostAvgCoast, LPCoast)

def Coast2():
    g = Graph(25)
    actors=[]
    
    for p in [95, 50, 70]:

        xpoints = np.array([])
        ypoints = np.array([])
        ypoints2 = np.array([])
        ypoints3 = np.array([])
        ypoints4 = np.array([])  
        ypoints5 = np.array([])
        ypoints6 = np.array([])  
        for maxN in range(2, 11):
        
            check = 0

            n = 10
            xpoints = np.append(xpoints, maxN)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if j==n-1:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                sensor.maxN = maxN
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()
                    act2.GetComponent("Sensor").GetObject().maxN = maxN

            print("for p=", g.p/100, " and n=", n)
            print("for N=", maxN)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            print("Try: ", g.tryNum)
            print("Passed: ", g.suc)
    
            ypoints = np.append(ypoints, lostAvgCoast(1-p/100, n, maxN)) 
            ypoints2 = np.append(ypoints2, (g.tryNum-g.suc) * 100/g.tryNum)
            ypoints3 = np.append(ypoints3, g.sends)
            ypoints4 = np.append(ypoints4, LPCoast(1-p/100, n, maxN))
            ypoints5 = np.append(ypoints5, g.sends*100/EPFull(1-p/100, n))
            ypoints6 = np.append(ypoints6, EPCoast(1-p/100, n, maxN)*100)
            
            g.lost = 0
            g.sends = 0
            g.tryNum = 0
            g.suc = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []
         
        plt.figure(1)
        plt.plot(xpoints, ypoints, label='for p='+str(p/100) + ' analytics')
        plt.plot(xpoints, ypoints2, label='for p='+str(p/100)+ ' imitation')
        plt.figure(2)
        plt.plot(xpoints, ypoints4, label='for p='+str(p/100)+ ' analytics')
        plt.plot(xpoints, ypoints3, label='for p='+str(p/100)+ ' imitation')
        plt.figure(3)
        plt.plot(xpoints, ypoints6, label='for p='+str(p/100)+ ' analytics')
        plt.plot(xpoints, ypoints5, label='for p='+str(p/100)+ ' imitation')


    plt.figure(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('EpsP')
    ax.grid(True)
    ax.legend()
    ax.set_title('Средний процент потерянных сообщений')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\coast_avg_lost_percent.png')
    plt.figure(2)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Lp')
    ax.grid(True)
    ax.legend()
    ax.set_title('Среднее число перепосылок')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\coast_avg_resends.png')
    plt.figure(3)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Ep(%)')
    ax.grid(True)
    ax.legend()
    ax.set_title('Удельные энергозатры')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\coast_powercost.png')
  

def ClasterRule(i, j, n):
    return (i - 2) % 5 == 0 and (j-2) % 5 == 0

def Claster2Impl():
    ImitationBase2(ClasterRule,'claster2', lostAvgClaster, LPClaster)  
  
def Claster2():
    g = Graph(25)
    actors=[]
    
    for p in [95, 50, 70]:

        xpoints = np.array([])
        ypoints = np.array([])
        ypoints2 = np.array([])
        ypoints3 = np.array([])
        ypoints4 = np.array([]) 
        ypoints5 = np.array([])
        ypoints6 = np.array([])         
        for maxN in range(2, 11):
        
            check = 0

            n = 10
            xpoints = np.append(xpoints, maxN)
            g.vecPoses = []
            nextSensorI=2
            for i in range(n):
                nextSensorJ=2
                for j in range(n):
                    #print(str(i) + " " + str(j) + " " + str(nextSensorI) + " " + str(nextSensorJ))
                    if i==nextSensorI and j==nextSensorJ:
                        #print("put ref")
                        nextSensorJ += 5
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                if i == nextSensorI:
                    nextSensorI+=5
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                sensor.maxN = maxN
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()
                    act2.GetComponent("Sensor").GetObject().maxN = maxN

            print("for p=", g.p/100, " and n=", n)
            print("for N=", maxN)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            print("Try: ", g.tryNum)
            print("Passed: ", g.suc)
            print("Full: ", LPClaster(1-p/100, n, maxN))
            print("Full: ", EPClasterFull(1-p/100, n))
    
            ypoints = np.append(ypoints, lostAvgClaster(1-p/100, n, maxN)) 
            ypoints2 = np.append(ypoints2, (g.tryNum-g.suc) * 100/g.tryNum)
            ypoints3 = np.append(ypoints3, g.sends)
            ypoints4 = np.append(ypoints4, LPClaster(1-p/100, n, maxN))
            ypoints5 = np.append(ypoints5, g.sends*100/EPClasterFull(1-p/100, n))
            ypoints6 = np.append(ypoints6, LPClaster(1-p/100, n, maxN)/EPClasterFull(1-p/100, n)*100)
            
            g.lost = 0
            g.sends = 0
            g.tryNum = 0
            g.suc = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []
        
        plt.figure(1)        
        plt.plot(xpoints, ypoints, label='for p='+str(p/100)+' analytics')
        plt.plot(xpoints, ypoints2, label='for p='+str(p/100)+' imitation')
        plt.figure(2)
        plt.plot(xpoints, ypoints4, label='for p='+str(p/100)+' analytics')
        plt.plot(xpoints, ypoints3, label='for p='+str(p/100)+' imitation')
        plt.figure(3)
        plt.plot(xpoints, ypoints6, label='for p='+str(p/100)+ ' analytics')
        plt.plot(xpoints, ypoints5, label='for p='+str(p/100)+ ' imitation')


    plt.figure(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('EpsP')
    ax.grid(True)
    ax.legend()
    ax.set_title('Средний процент потерянных сообщений')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\claster_avg_lost_percent.png')
    plt.figure(2)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Lp')
    ax.grid(True)
    ax.legend()
    ax.set_title('Среднее число перепосылок')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\claster_avg_resends.png')
    plt.figure(3)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Ep(%)')
    ax.grid(True)
    ax.legend()
    ax.set_title('Удельные энергозатры')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\claster_powercost.png')
 
def PerimRule(i, j, n):
    return j==0 or j==n-1 or i==0 or i==n-1
    
def Perim2Impl():
    ImitationBase2(PerimRule,'perim2', lostAvgPerim, LPPerim) 
 
def Premi2():
    g = Graph(25)
    actors=[]
    
    for p in [95, 50, 70]:

        xpoints = np.array([])
        ypoints = np.array([])
        ypoints2 = np.array([])
        ypoints3 = np.array([])
        ypoints4 = np.array([])  
        ypoints5 = np.array([])
        ypoints6 = np.array([]) 
        for maxN in range(2, 11):
        
            check = 0

            n = 10
            xpoints = np.append(xpoints, maxN)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if j==0 or j==n-1 or i==0 or i==n-1:
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                sensor.maxN = maxN
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()
                    act2.GetComponent("Sensor").GetObject().maxN = maxN

            print("for p=", g.p/100, " and n=", n)
            print("for N=", maxN)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            print("Try: ", g.tryNum)
            print("Passed: ", g.suc)
    
            ypoints = np.append(ypoints, lostAvgPerim(1-p/100, n, maxN)) 
            ypoints2 = np.append(ypoints2, (g.tryNum-g.suc) * 100/g.tryNum)
            ypoints3 = np.append(ypoints3, g.sends)
            ypoints4 = np.append(ypoints4, LPPerim(1-p/100, n, maxN))
            ypoints5 = np.append(ypoints5, EPPerim(1-p/100, n, maxN))
            ypoints6 = np.append(ypoints6, EPPerim(1-p/100, n, maxN))
            
            g.lost = 0
            g.sends = 0
            g.tryNum = 0
            g.suc = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []
          
        plt.figure(1)
        plt.plot(xpoints, ypoints, label='for p='+str(p/100) + ' analytics')
        plt.plot(xpoints, ypoints2, label='for p='+str(p/100)+ ' imitation')
        plt.figure(2)
        plt.plot(xpoints, ypoints4, label='for p='+str(p/100)+ ' analytics')
        plt.plot(xpoints, ypoints3, label='for p='+str(p/100)+ ' imitation')
        plt.figure(3)
        plt.plot(xpoints, ypoints6, label='for p='+str(p/100)+ ' analytics')
        plt.plot(xpoints, ypoints5, label='for p='+str(p/100)+ ' imitation')


    plt.figure(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('EpsP')
    ax.grid(True)
    ax.legend()
    ax.set_title('Средний процент потерянных сообщений')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\perim_avg_lost_percent.png')
    plt.figure(2)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Lp')
    ax.grid(True)
    ax.legend()
    ax.set_title('Среднее число перепосылок')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\perim_avg_resends.png')
    plt.figure(3)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Ep(%)')
    ax.grid(True)
    ax.legend()
    ax.set_title('Удельные энергозатры')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\perim_powercost.png')
 
def SKin(mu, k):
    return 4*(2*mu-2*k-1)
    
def SKout(v, mu, k, kmax):
    if k >= 1 and k <= kmax/2:
        return 4 * (2*mu+k-1)
    else:
        return 4 * (2*v-(2*mu+k-1))
        
def lostAvgContour(p, n, N, kin, kout, S, mu, v):
    suminmax = 0
    for i in range(1, int(kin)+1):
        suminmax += SKin(mu, i)*(1-(1-p**N)**i)/S
        
    for i in range(1, int(kout)+1):
        suminmax += SKout(v, mu, i, kout)*(1-(1-p**N)**i)/S
        
    return suminmax * 100

def lp(q, N, k):
    p = 1 - q
    p1 = (1-q**N)
    return (1-p1**k)*p1/((1-p1)*p)

def LPCont2(p,n,N, kin, kout, mu, v):
    suml = 0.0
    for i in range(1, int(kin) + 1):
        suml += SKin(mu, i) * lp(p, N, i)
    for i in range(1, int(kout) + 1):
        suml += SKout(v, mu, i, kout) * lp(p, N, i)
    return suml
    
def EPContFull(p,n,N, kin, kout, mu, v):
    return LPCont2(p, n, N, kin, kout, mu, v)/LPCont2(p, n, 50, kin, kout, mu, v)


def ContourForPowercost2():
    g = Graph(25)
    actors=[]
 
    realSends = np.array([])
        
    p=50
    n = 10
    v=n/2
    for mu in range(1, int(v)+1, 2):
        check = 0

        g.vecPoses = []
        for i in range(n):
            for j in range(n):
                if (j >= v-mu and j < (v-mu)+(mu*2) and i==v-mu) or (i==(v-mu)+(mu*2)-1 and j >= v-mu and j < (v-mu)+(mu*2)) or (i >= v-mu and i < (v-mu)+(mu*2) and j==v-mu) or (i >= v-mu and i < (v-mu)+(mu*2) and j==(v-mu)+(mu*2)-1):
                    actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                    actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                else:
                    actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                
                actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
              
        for act in actors:
            scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
            sensor = cast(Sensor, scriptComp.GetObject())
            sensor.SetGraph(g)
            sensor.maxN = -1
            g.p = p
            
        for act in actors:
            scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
            sensor = cast(Sensor, scriptComp.GetObject()) 
            if sensor.Ref == True:
                continue
            
            sensor.StartSendingSignal()
            for act2 in actors:
                act2.GetComponent("Sensor").GetObject().Clear()
                act2.GetComponent("Sensor").GetObject().maxN = -1

        print("for p=", g.p/100, " and n=", n)
        print("Losts: ", g.lost)
        print("Sends: ", g.sends)
        print("Try: ", g.tryNum)
        print("Passed: ", g.suc)

        realSends = np.append(realSends, g.sends)
        
        g.lost = 0
        g.sends = 0
        g.tryNum = 0
        g.suc = 0

        for act in actors:
            BIEActor.DestroyActor(act)
            
        actors = []

    return realSends

def ContourRule(i, j, n, mu, v):
    return (j >= v-mu and j < (v-mu)+(mu*2) and i==v-mu) or (i==(v-mu)+(mu*2)-1 and j >= v-mu and j < (v-mu)+(mu*2)) or (i >= v-mu and i < (v-mu)+(mu*2) and j==v-mu) or (i >= v-mu and i < (v-mu)+(mu*2) and j==(v-mu)+(mu*2)-1)

def Contour2Impl(n, mu):
    v = n/2
    kin = mu - 1
    kout = 2*(v-mu)
    Scont = n*n - 4*(2*mu-1)

    CountourRuleLambda = lambda i, j, n : ContourRule(i, j, n, mu, v)
    
    lostAvgContourLambda = lambda Q, n, maxN : lostAvgContour(Q, n, maxN, kin, kout, Scont, mu, v)
    LPContourLambda = lambda Q, n, maxN : LPCont2(Q, n, maxN, kin, kout, mu, v)
    
    ImitationBase2(CountourRuleLambda, 'contour2_mu_'+str(mu), lostAvgContourLambda, LPContourLambda)

def Contour2():
    g = Graph(25)
    actors=[]
    
    index = 0
    realSends = ContourForPowercost2()
    
    p=50
    n=10
    v=n/2
    for mu in range(1, int(v)+1, 2):
        kin = mu - 1
        kout = 2*(v-mu)
        Scont = n*n - 4*(2*mu-1)
        
        xpoints = np.array([])
        ypoints = np.array([])
        ypoints2 = np.array([])
        ypoints3 = np.array([])
        ypoints4 = np.array([])  
        ypoints5 = np.array([])
        ypoints6 = np.array([])
        for maxN in range(2, 11):
        
            check = 0

            xpoints = np.append(xpoints, maxN)
            g.vecPoses = []
            for i in range(n):
                for j in range(n):
                    if (j >= v-mu and j < (v-mu)+(mu*2) and i==v-mu) or (i==(v-mu)+(mu*2)-1 and j >= v-mu and j < (v-mu)+(mu*2)) or (i >= v-mu and i < (v-mu)+(mu*2) and j==v-mu) or (i >= v-mu and i < (v-mu)+(mu*2) and j==(v-mu)+(mu*2)-1):
                        print("put ref")
                        actors.append(BIEActor.CreateActor("Actors/sinker.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                        g.AddRefPos(actors[-1].GetComponent("TransformComponent").GetPosition())
                        actors[-1].GetComponent("Sensor").GetObject().SetRefMarker(True)
                    else:
                        actors.append(BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(-30 + i * 15, 6, -30 + j * 15), BIEVector.Vec3(0, 0, 0)))
                    
                    actors[-1].GetComponent("Sensor").GetObject().SetOptimMarker(True)
                  
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject())
                sensor.SetGraph(g)
                sensor.maxN = maxN
                g.p = p
                
            for act in actors:
                scriptComp = cast(BIEActor.ScriptComponent, act.GetComponent("Sensor"))
                sensor = cast(Sensor, scriptComp.GetObject()) 
                if sensor.Ref == True:
                    continue
                
                sensor.StartSendingSignal()
                for act2 in actors:
                    act2.GetComponent("Sensor").GetObject().Clear()
                    act2.GetComponent("Sensor").GetObject().maxN = maxN

            print("for p=", g.p/100, " and n=", n)
            print("for N=", maxN)
            print("Losts: ", g.lost)
            print("Sends: ", g.sends)
            print("Try: ", g.tryNum)
            print("Passed: ", g.suc)
    
            ypoints = np.append(ypoints, lostAvgContour(1-p/100, n, maxN, kin, kout, Scont, mu, v)) 
            ypoints2 = np.append(ypoints2, (g.tryNum-g.suc) * 100/g.tryNum)
            ypoints3 = np.append(ypoints3, g.sends)
            ypoints4 = np.append(ypoints4, LPCont2(1-p/100, n, maxN, kin, kout, mu, v))
            ypoints6 = np.append(ypoints6, g.sends*100/realSends[index])
            ypoints5 = np.append(ypoints5, EPContFull(1-p/100, n, maxN, kin, kout, mu, v)*100)
            
            g.lost = 0
            g.sends = 0
            g.tryNum = 0
            g.suc = 0

            for act in actors:
                BIEActor.DestroyActor(act)
                
            actors = []
           
        index += 1
        plt.figure(1)
        plt.plot(xpoints, ypoints, label='for mu='+str(mu) + ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints2, label='for mu='+str(mu)+ ' imitation')
        plt.figure(2)
        plt.plot(xpoints, ypoints4, label='for mu='+str(mu)+ ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints3, label='for mu='+str(mu)+ ' imitation')
        plt.figure(3)
        plt.plot(xpoints, ypoints5, label='for mu='+str(mu)+ ' analytics', linestyle='dashed')
        plt.plot(xpoints, ypoints6, label='for mu='+str(mu)+ ' imitation')


    plt.figure(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('EpsP')
    ax.grid(True)
    ax.legend()
    ax.set_title('Средний процент потерянных сообщений')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\cont_avg_lost_percent.png')
    plt.figure(2)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Lp')
    ax.grid(True)
    ax.legend()
    ax.set_title('Среднее число перепосылок')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\cont_avg_resends.png')
    plt.figure(3)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_xlabel('N')
    ax.set_ylabel('Ep(%)')
    ax.grid(True)
    ax.legend()
    ax.set_title('Удельные энергозатры')
    plt.savefig('C:\\Users\\BystrovI\\Downloads\\imgs\\cont_powercost.png')
  
#Coast2()
#Coast2Impl()
#Claster2()
#Claster2Impl()
#Premi2()
#Perim2Impl()
#Contour2()
Contour2Impl(10, 5)