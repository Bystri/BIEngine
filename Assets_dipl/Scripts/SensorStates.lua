-----------------------------------------------------------------------------------------------------------------------
-- SensorState										
-- Базовый класс для всех состояний сенсеров
-----------------------------------------------------------------------------------------------------------------------
SensorState = class(nil,
{
	_sensor = nil,
});

function SensorState:Init()
	if (self._sensor == nil) then
		print("Invalid sensor in SensorState");
		return false;
	end
	
	return true;
end

function SensorState:Update(deltaMs)
	error("Calling unimplemented SensorState.Update() fucntion");
end


-----------------------------------------------------------------------------------------------------------------------
-- WaitState
-----------------------------------------------------------------------------------------------------------------------
WaitState = class(SensorState, 
{
	name = "WaitState",
});

function WaitState:Update(deltaMs)
	
end

-----------------------------------------------------------------------------------------------------------------------
-- SendHelloSignalState
-----------------------------------------------------------------------------------------------------------------------
SendHelloSignalState = class(SensorState, 
{
	name = "SendHelloSignalState",
});

function SendHelloSignalState:Update(deltaMs)
	print("_needToSendHelloSignal is true now");
	self._sensor._needToSendHelloSignal = true;
end

-----------------------------------------------------------------------------------------------------------------------
-- SendSignalState
-----------------------------------------------------------------------------------------------------------------------
SendSignalState = class(SensorState,
{
	name = "SendSignalState",
	
	_sendInterval = 1.5,
	_sendTimer = 0,
});

function SendSignalState:Init()
	self.base.Init(self);
end

function SendSignalState:Update(deltaMs)
	-- Обновляем таймер отправки сообщений
	self._sendTimer = self._sendTimer + deltaMs;

	self:_AttemptToSendSignal();
end

function SendSignalState:_AttemptToSendSignal()
	if (self._sendTimer >= self._sendInterval and self._sensor._needToSendSignal ~= 1) then
        self._sensor._needToSendSignal = 1;
		self._sendTimer = 0;
	end
end