
print("Dynamic script run");

Static = class(nil,
{
	_position = nil,
	_orientation = 0,
	
	_velocity = nil,
	_rotation = nil,
});

DynamicSteeringOutput = class(nil,
{
	_linear = nil,
	_angular = 0,
});

--Алгортим движения, для преследования цели
DynamicSeek = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxAcceleration = 0,
});

--Высчитывает необходимую скорость чтобы добраться до цели
function DynamicSeek:getSteering()
	--Сначала получаем позицию до цели
	local linear = self._target._position - self._character._position;
		
	--Затем назначаем максимальное ускорость 
	linear:Normalize();
	
	linear = linear * self._maxAcceleration;
	
	local result = DynamicSteeringOutput:Create({_linear = linear, _angular = 0});

	return result;
end



--Алгортим движения, для ухода от цели
DynamicFlee = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxAcceleration = 0,
});

--Высчитывает необходимую скорость чтобы убежать от цели
function DynamicFlee:getSteering()
	--Сначала получаем позицию до цели
	local linear = self._target._position - self._character._position;
		
	--Затем назначаем максимальную скорость 
	linear:Normalize();
	
	linear = linear * self._maxAcceleration;
	
	local result = DynamicSteeringOutput:Create({_linear = linear, _angular = 0});

	return result;
end



--Алгоритм движения, который учивает дистанцию до цели, чтобы плавно останавливаться при подходе к ней
DynamicArrive = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxAcceleration = 0,
	_maxSpeed = 0,
	
	--Дистанция до цели, в пределах которой мы можем остановиться
	_targetRadius = 50,
	--Дистанция до цели, в пределах которой мы должны начать тормозить
	_slowRadius = 0,
	
	_timeToTarget = 0.25,
});

--Высчитывает необходимую скорость чтобы достичь цели
function DynamicArrive:getSteering()
	--Сначала получаем позицию до цели
	local direction = self._target._position - self._character._position;
		
	local dist = direction:Length();	
	--Если мы в необходимой близости от цели - можем остановиться
	if (dist < self._targetRadius) then
		return nil;
	end

	local targetSpeed = self._maxSpeed;
	--Если это слишком быстро - ограничиваем скорость
	if (dist <= self._slowRadius) then 
		targetSpeed = targetSpeed * dist / self._slowRadius;
	end
	
	local targetVelocity = Vec3:Create({x = direction.x, y = direction.y, z = direction.z});
	targetVelocity:Normalize();
	targetVelocity = targetVelocity * targetSpeed;

	--Пытаемся найти ускорение, необходимое для нужной скорости
	targetVelocity = (targetVelocity - self._character._velocity) / self._timeToTarget;
	
	if (targetVelocity:Length() > self._maxAcceleration) then
		targetVelocity:Normalize();
		targetVelocity = targetVelocity * self._maxAcceleration;
	end
	
	local result = DynamicSteeringOutput:Create({_linear = targetVelocity, _angular = 0});
	
	return result;
end

--Алгоритм движения, позволящий повернуться персонажу так, чтобы его ориентация стала равна ориентации цели
DynamicAlign = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	--Макисмальное угловое усокрение
	_maxAngularAcceleration = 0,
	--Максимальная скорость поворота.
	_maxRotation = 0,
	
	--Дистанция до цели, в пределах которой мы можем остановиться
	_targetRadius = 0,
	--Дистанция до цели, в пределах которой мы должны начать тормозить
	_slowRadius = 0,
	
	_timeToTarget = 0.1,
});

--Приводит градусы к значениям в диапазоне (-180, 180)
function mapDegreesToRange(degree)
	if (degree > 180) then
		degree = degree - 360 * (degree / 360 + 1)
	end
	
	if (degree < -180) then
		degree = degree + 360 * (degree / 360 + 1)
	end

	return degree
end

--Высчитывает необходимую скорость, чтобы достичь ориентации цели
function DynamicAlign:getSteering()
	--Сначала получаем разницу в угле
	local rotation = self._target._orientation - self._character._orientation;
		
	--Приводит градусы к значениям в диапазоне (-180, 180)
	rotation = mapDegreesToRange(rotation);
	local rotationSize = math.abs(rotation);
	--Если мы уже в необходимом пределе - останаливаем кручение
	if (rotationSize < self._targetRadius) then
		print("nil");
		return nil;
	end
	
	local targetRotation = self._maxRotation;
	--Если это слишком быстро - ограничиваем скорость
	if (rotationSize <= self._slowRadius) then 
		targetRotation = targetRotation * rotationSize / self._slowRadius;
	end
	
	targetRotation = targetRotation * rotation / rotationSize;
	
	--Пытаемся найти ускорение, необходимое для нужной скорости разворота
	local angularAcceleration = targetRotation - self._character._orientation;
	angularAcceleration = angularAcceleration / self._timeToTarget;
	
	if (math.abs(angularAcceleration) > self._maxAngularAcceleration) then
		angularAcceleration = angularAcceleration / math.abs(angularAcceleration);
		angularAcceleration = angularAcceleration * self._maxAngularAcceleration;
	end
	
	local result = DynamicSteeringOutput:Create({_linear = Vec3:Create({x = 0, y = 0, z = 0}), _angular = angularAcceleration});
	
	return result;
end


--Алгоритм движения, который позволяет персонажу получить равную с целью скорость
DynamicVelocityMatch = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxAcceleration = 0,
	
	_timeToTarget = 1,
});

--Высчитывает необходимое ускорение, чтобы сравнять скорость персонажа со скоростью цели
function DynamicVelocityMatch:getSteering()
	--Сначала получаем вектор ускорения, которого нам нехватает, чтобы достичь скорости цели
	local acceleration = (self._target._velocity - self._character._velocity) / self._timeToTarget;
		
	if (acceleration:Length() > self._maxAcceleration) then
		acceleration:Normalize();
		acceleration = acceleration * self._maxAcceleration;
	end
	
	local result = DynamicSteeringOutput:Create({_linear = acceleration, _angular = 0});
	return result;
end
