require("Scripts/SensorStates.lua");
require("Scripts/SensorBrains.lua");

-----------------------------------------------------------------------------------------------------------------------
-- State machine								
-----------------------------------------------------------------------------------------------------------------------
SensorStateMachine = class(nil,
{
	_sensor = nil,
	_currentState = nil,
	_brain = nil,
});

function SensorStateMachine:Destroy()
	self._currentState = nil;
	self._brain = nil;
end

function SensorStateMachine:SetState(newState)
	if (self._currentState == nil or not self._currentState:IsInstance(newState)) then
		self:_InternalSetState(newState);
	end
end

function SensorStateMachine:ChooseBestState()
	if (self._brain) then
		local newState = self._brain:Think();
		
		if (newState == nil) then
			print("Sensor chose nil");
		end
		
		self:SetState(newState);
	end
end

function SensorStateMachine:Update(deltaMs)
	if (self._currentState) then
		self._currentState:Update(deltaMs);
	end
end

function SensorStateMachine:_InternalSetState(newState)
	self._currentState = newState:Create({_sensor = self._sensor});
	self._currentState:Init();
end

