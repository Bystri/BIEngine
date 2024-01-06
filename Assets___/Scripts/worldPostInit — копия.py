#Импорт библиотеки, которая занимается управлением акторами (объектами в мире)
#Сразу скажу, что все библиотеки из моего движка имеют приписку BIE
import BIEActor
from typing import cast

print("World PostInit")

#Создаем актора. Настройки актора прописаны в указанном файле. 
#Из настроек там просто указано, что объект должен появится в точке (0, 0, 0) и иметь форму кубу 
actor = BIEActor.CreateActor("Actors/sensor.xml", BIEVector.Vec3(0, 0, 0), BIEVector.Vec3(0, 0, 0))

#У актора есть компоненты, например, у нашего куба есть компонент, который отвечает за его положением в мире 
#и компонент, который отвечает за его вид (то есть форму куба)
#Тут мы с помощью вызова функции   actor.GetComponent("TransformComponent")   получаем компонент, который отвечает за положением куба
transformComp = cast(BIEActor.TransformComponent, actor.GetComponent("TransformComponent"))


#Тут мы начинаем задавать дейтсвия нашему кубу. Для этого нам потребуются вектора и система процессов(щас расскажу)
import BIEVector
import BIEProcess
import math

#Процессы, это объекты, которые определяют какое-либо дейтсвие. Все процессы должны наследоваться от BIEProcess.Process
#У процесса каждый кадр будет вызываться функция OnUpdate
import BIEDebug

#Определим процесс движения по окружности
class MoveProcess(BIEProcess.Process):
    #Мы будем передавать актора, при создании
    def __init__(self, movableActor : BIEActor.Actor):
        BIEProcess.Process.__init__(self)
        
        #От актора нам потребуется только его TransformComponent, так как он отвечает за положение - это как раз то, что нам надо изменять
        self.transformComp = cast(BIEActor.TransformComponent, movableActor.GetComponent("TransformComponent"))
        #Угловая скорость всех поворотов
        self.angularSpeedGrad = 60
        #Текущий угол всех поворотов
        self.currentAngle = 0
        #Радиус окружности, по которой двигается куб
        self.radius = 5
        
    #Функция, которая будет вызываться каждый кадр
    #Чтобы было понятно, сначала каждый кадр у всех процессов вызывается функция OnUpdate(deltaTime), 
    #где deltaTime - время, прошедшее с прошлого кадра
    #А уже потом и сама отрисовка
    def OnUpdate(self, deltaTime):
        #Считаем новый угол в радианах
        self.currentAngle = self.currentAngle + self.angularSpeedGrad * deltaTime 

        #Высчитываем положение нашего куба
        newX = self.radius * math.cos(math.radians(self.currentAngle))
        newY = self.radius * math.sin(math.radians(self.currentAngle))
        vecPos = BIEVector.Vec3(newX, newY, 0)
        
        #Задаем повороты куба вокруг своих осей 
        vecRot = BIEVector.Vec3(self.currentAngle, self.currentAngle, self.currentAngle)
        
        #Передаем это в TransformComponent нашего актора
        self.transformComp.SetPosition(vecPos)
        self.transformComp.SetRotation(vecRot)
        
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(0, 6, 4), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(0, 6, -4), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(4, 6, 0), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(-4, 6, 0), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(4, 6, 4), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(-4, 6, -4), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(-4, 6, 4), BIEVector.Vec3(0, 0, 1))
        BIEDebug.DbgLine(BIEVector.Vec3(0, 6, 0), BIEVector.Vec3(4, 6, -4), BIEVector.Vec3(0, 0, 1))

    
#Теперь мы создаем наш процесс
proc = MoveProcess(actor)
#И отдаем его нашему движку
BIEProcess.AttachProcess(proc)

