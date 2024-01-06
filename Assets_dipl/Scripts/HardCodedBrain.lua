require("Scripts/SensorAI.lua")
require("Scripts/SensorBrains.lua");
require("Scripts/SensorStates.lua");

HardCodedBrain = class(SensorBrain,
{
});

function HardCodedBrain:Think()
	if ((self._isMainSensor or self._sensor._helloReceived) and self._sensor._triedToSayHello == false) then
		self._sensor._triedToSayHello = true;
		return SendHelloSignalState;
	elseif (self._sensor.leftReceive ~= 0 or self._sensor.rightReceive ~= 0 or self._sensor.upReceive ~= 0 or self._sensor.downReceive ~= 0) then
		return SendSignalState;
	else
		return WaitState;
	end
	
end

