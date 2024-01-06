require("Scripts/ActorManager.lua");

print("worldPreInit script run");

-----------------------------------------------------------------------------------------------------------------------
-- Global actor manager										
-----------------------------------------------------------------------------------------------------------------------
g_actorMgr = ActorManager:Create();

ActorProcess = class(ScriptProcess,
{
});

function ActorProcess:OnUpdate(deltaMs)
	print("Do Actors update");
	g_actorMgr:Update(deltaMs);
end

AttachProcess(ActorProcess:Create({frequency = 5.0}));

-----------------------------------------------------------------------------------------------------------------------
-- constructor / destructor functions for actors
-----------------------------------------------------------------------------------------------------------------------
function AddMainSensor(scriptObject)
	g_actorMgr:AddMainSensor(scriptObject);
end

function RemoveMainSensor(scriptObject)
	g_actorMgr:RemoveMainSensor(scriptObject);
end

function AddSensor(scriptObject)
	g_actorMgr:AddSensor(scriptObject);
end

function RemoveSensor(scriptObject)
	g_actorMgr:RemoveSensor(scriptObject);
end

function AddHelloSignal(scriptObject)
	g_actorMgr:AddHelloSignal(scriptObject);
end

function AddSignal(scriptObject)
	g_actorMgr:AddSignal(scriptObject);
end


