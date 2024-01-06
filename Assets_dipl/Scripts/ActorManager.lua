
print("ActorManager script run");

require("Scripts/HardCodedBrain.lua");

SENSOR_BRAIN = HardCodedBrain;


ActorManagerProcess = class(ScriptProcess,
{
	_mainSensor = nil,
	_sensors = nil,  -- this points to the same table as ActorManager._sensors
});


SensorUpdater = class(ActorManagerProcess,
{
	--
});

function SensorUpdater:OnUpdate(deltaMs)
	if (self._mainSensor) then
		self._mainSensor.stateMachine:Update(deltaMs);
	end
	
	for id, actor in pairs(self._sensors) do
		actor.stateMachine:Update(deltaMs);
	end
end


SensorThinker = class(ActorManagerProcess,
{
	--
});

function SensorThinker:OnUpdate(deltaMs)
	print("Running AI update for sensors");
	
	if (self._mainSensor) then
		self._mainSensor.stateMachine:ChooseBestState();
	end
	
	for id, actor in pairs(self._sensors) do
		actor.stateMachine:ChooseBestState();
	end
end



ActorManager = class(nil,
{
	_mainSensor = nil, 	--Главный сенсор, принимающий данные от других датчиков
						--И отправляющий их к глайдеру
	
	_sensors = {}, 	--Датчиик собирающие информацию, которые отправят ее к сенсору
	_signals = {}, 	--Сигналы будут физическими объектами

	
	--Процессы
	_sensorsProcesses = nil,
	
	_sensorThinker = nil, --процесс, который заставляет принимать сенсоры новые решения 
	_sensorUpdater = nil, --процесс, которые обновляеет состояния датчиков
});

function ActorManager:AddMainSensor(scriptObject)
	print("Add main sensor");

	--Добавить скрипт к списку скриптов
	local actorId = scriptObject:GetActorId();
	if (self._mainSensor ~= nil) then
		print("Overwritin main sensor actor: id = " .. actorId);
	end
	self._mainSensor = scriptObject;
	
	scriptObject.leftReceive = 0;
	scriptObject.rightReceive = 0;
	scriptObject.upReceive = 0;
	scriptObject.downReceive = 0;
	scriptObject._needToSendSignal = 0;
	scriptObject._triedToSayHello = false;
	scriptObject._helloReceived = false;
	
	print(scriptObject.downChance);
	
	local brain = nil;
	if (SENSOR_BRAIN) then
		brain = SENSOR_BRAIN:Create({_sensor=scriptObject, _isMainSensor = true});
		if (not brain:Init()) then
			print("Failed to initialize brain");
		end;
	end;
	
	scriptObject.stateMachine = SensorStateMachine:Create({_sensor = scriptObject, _brain = brain});
	
	-- set the initial state
	--scriptObject.stateMachine:SetState(PatrolState);
	
	if (self._sensorProcesses == nil) then
		self:_CreateSensorProcesses();
	end
end

function ActorManager:AddSensor(scriptObject)
	print("Add sensor");
	--Добавить скрипт к списку скриптов
	local actorId = scriptObject:GetActorId();
	if (self._sensors[actorId] ~= nil) then
		print("Overwritin sensor actor: id = " .. actorId);
	end
	self._sensors[actorId] = scriptObject;
	
	scriptObject.leftReceive = 0;
	scriptObject.rightReceive = 0;
	scriptObject.upReceive = 0;
	scriptObject.downReceive = 0;
	scriptObject._needToSendSignal = 0;
	scriptObject._triedToSayHello = false;
	scriptObject._helloReceived = false;
	
	print(scriptObject.actorType);
	print(scriptObject.leftChance);
	
	local brain = nil;
	if (SENSOR_BRAIN) then
		brain = SENSOR_BRAIN:Create({_sensor=scriptObject, _isMainSensor = false});
		if (not brain:Init()) then
			print("Failed to initialize brain");
		end;
	end;
	
	scriptObject.stateMachine = SensorStateMachine:Create({_sensor=scriptObject, _brain = brain});
	
	--scriptObject.stateMachine:SetState(WaitState);
	
	if (self._sensorProcesses == nil) then
		self:_CreateSensorProcesses();
	end
end

function ActorManager:AddHelloSignal(scriptObject)
	local actorId = scriptObject:GetActorId();
	if (self._signals[actorId] ~= nil) then
		print("Overwriting signal actor; id = " .. actorId);
	end
	self._signals[actorId] = scriptObject;
end

function ActorManager:AddSignal(scriptObject)
	local actorId = scriptObject:GetActorId();
	if (self._signals[actorId] ~= nil) then
		print("Overwriting signal actor; id = " .. actorId);
	end
	self._signals[actorId] = scriptObject;
end

function ActorManager:RemoveSignal(scriptObject)
	local actorId = scriptObject:GetActorId();
	self._signals[actorId] = nil;
end

function ActorManager:Update(deltaMs)
	if (self._mainSensor) then
		if (self._mainSensor._needToSendHelloSignal) then
			self:OnSendHelloSignal(self._mainSensor);
			self._mainSensor._needToSendHelloSignal = false;
		end
		
		if (self._mainSensor._needToSendSignal == 1) then
			self:OnSendSignal(self._mainSensor);
			self._mainSensor._needToSendSignal = 0;
		end
	end
	
	for id, actor in pairs(self._sensors) do 
		if (actor._needToSendHelloSignal) then
			self:OnSendHelloSignal(actor);
			actor._needToSendHelloSignal = false;
		end
		
		if (actor._needToSendSignal == 1) then
			self:OnSendSignal(actor);
			actor._needToSendSignal = 0;
		end
	end
end

function ActorManager:OnSendHelloSignal(sensor)
	if (sensor == nil) then
		print("Send signal from noone?");
		return;
	end;
	
	print("SendHelloSignal!");

	local pos = Vec3:Create(sensor:GetPos());
	
	if (sensor.leftReceive == 0 and sensor.leftChance > math.random()) then
		local sendDir = Vec3:Create({x=-50, y=0, z=0});
		self:_CreateSignal("actors/helloSignal.xml", pos, sendDir);
	end
	
	if (sensor.rightReceive == 0 and sensor.rightChance > math.random()) then
		local sendDir = Vec3:Create({x=50, y=0, z=0});
		self:_CreateSignal("actors/helloSignal.xml", pos, sendDir);
	end
	
	if (sensor.upReceive == 0 and sensor.upChance > math.random()) then
		local sendDir = Vec3:Create({x=0, y=-50, z=0});
		self:_CreateSignal("actors/helloSignal.xml", pos, sendDir);
	end
	
	if (sensor.downReceive == 0 and sensor.downChance > math.random()) then
		local sendDir = Vec3:Create({x=0, y=50, z=0});
		self:_CreateSignal("actors/helloSignal.xml", pos, sendDir);
	end
end

function ActorManager:OnSendSignal(sensor)
	if (sensor == nil) then
		print("Send signal from noone?");
		return;
	end;
	
	print("SendSignal!");
	local pos = Vec3:Create(sensor:GetPos());
	
	if (sensor.leftReceive > math.random()) then
		local sendDir = Vec3:Create({x=-50, y=0, z=0});
		self:_CreateSignal("actors/dataSignal.xml", pos, sendDir);
	end
	
	if (sensor.rightReceive > math.random()) then
		local sendDir = Vec3:Create({x=50, y=0, z=0});
		self:_CreateSignal("actors/dataSignal.xml", pos, sendDir);
	end
	
	if (sensor.upReceive > math.random()) then
		local sendDir = Vec3:Create({x=0, y=-50, z=0});
		self:_CreateSignal("actors/dataSignal.xml", pos, sendDir);
	end
	
	if (sensor.downReceive > math.random()) then
		local sendDir = Vec3:Create({x=0, y=50, z=0});
		self:_CreateSignal("actors/dataSignal.xml", pos, sendDir);
	end
	
	sensor._needToSendSignal = 0;
end

function ActorManager:_CreateSignal(actorResource, sensorPosition, direction)
	print("_CreateSignal!");
	local signalObj = CreateActor(actorResource);
	if (signalObj ~= -1) then
		local signalPos = Vec3:Create({x=sensorPosition.x + direction.x,y=sensorPosition.y + direction.y, z=sensorPosition.z + direction.z});
		SetPosition(signalPos, signalObj);
		direction.x = direction.x / 10;
		direction.y = direction.y / 10;
		direction.z = direction.z / 10;
		SetVelocity(direction, signalObj);
	end
end

function ActorManager:OnPhysicsCollision(eventData)
	local actorA = self:GetActorById(eventData.actorA);
	local actorB = self:GetActorById(eventData.actorB);
	
	if (actorA == nil or actorB == nil) then
		print("Cannot find all collision objects");
		return;
	end
	
	local sensor = nil;
	local signalObj = nil;
	
	print(actorA.actorType);
	print(actorB.actorType);
	
	if (actorA.actorType == "Sensor" and actorB.actorType == "Signal") then
		sensor = actorA;
		signalObj = actorB;
	elseif (actorB.actorType == "Sensor" and actorA.actorType == "Signal") then
		sensor = actorB;
		signalObj = actorA;
	elseif (actorA.actorType == "Sensor" and actorB.actorType == "HelloSignal") then
		sensor = actorA;
		signalObj = actorB;
	elseif (actorB.actorType == "Sensor" and actorA.actorType == "HelloSignal") then
		sensor = actorB;
		signalObj = actorA;
	end
	
	if (sensor == nil or signalObj == nil) then
		print("Cannot determinate sensor or signalObj or both");
		return;
	end
	
	self:_ReceiveSignal(sensor, signalObj.actorType, eventData.sumNormalForce);
	
	self:RemoveSignal(signalObj);
	QueueEvent(EventType.EvtData_Request_Destroy_Actor, signalObj:GetActorId());
end

function ActorManager:UpdateUi()
	
end

function ActorManager:GetActorById(actorId)
	local actor = nil;

	if (self._mainSensor and self._mainSensor:GetActorId() == actorId) then
		actor = self._mainSensor;
	end
	
	if (not actor) then
		actor = self:GetSensor(actorId);
	end
	
	if (not actor) then
		actor = self:GetSignal(actorId);
	end
	
	return actor;
end

function ActorManager:GetSensor(actorId)
	return self._sensors[actorId];
end

function ActorManager:GetSignal(actorId)
	return self._signals[actorId];
end


function ActorManager:_CreateSensorProcesses()
	self._sensorProcesses = {};

	self._sensorProcesses[#self._sensorProcesses+1] = SensorUpdater:Create({_mainSensor = self._mainSensor, _sensors = self._sensors, frequency = 1.499});
	self._sensorProcesses[#self._sensorProcesses+1] = SensorThinker:Create({_mainSensor = self._mainSensor, _sensors = self._sensors, frequency = 3.499});

	-- attach all the processes
	for i, proc in ipairs(self._sensorProcesses) do
		AttachProcess(proc);
	end
end

function ActorManager:_ReceiveSignal(sensor, signalType, sumNormalForce)
	if (signalType == "HelloSignal") then
		sensor._helloReceived = true;
	
		if (sumNormalForce.x == 1) then
			sensor.leftReceive = sensor.leftChance;
		end 
		
		if (sumNormalForce.x == -1) then
			sensor.rightReceive = sensor.rightChance;
		end 
		
		if (sumNormalForce.y == -1) then
			sensor.upReceive = sensor.upChance;
		end 
		
		if (sumNormalForce.y == 1) then
			sensor.downReceive = sensor.downChance;
		end 
	end
end




