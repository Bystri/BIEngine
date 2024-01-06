function OnPhysicsCollision(eventData)
	g_actorMgr:OnPhysicsCollision(eventData);
end

function RegisterListeners()
	print("Register listeners");
	if (EventType.EvtData_PhysCollision ~= nil) then
		RegisterEventListener(EventType.EvtData_PhysCollision, OnPhysicsCollision);
	end
end

