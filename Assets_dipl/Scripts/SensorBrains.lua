require("Scripts/SensorStates.lua");

SensorBrain = class(nil,
{
	_sensor = nil,
	_isMainSensor = false,
	
	_helloReceived = false,
	_triedToSayHello = false,
});

function SensorBrain:Init()
	return true;
end

function SensorBrain:Think()
	error("Calling unimplemented base class version of SensorBrain.Think()");
end

