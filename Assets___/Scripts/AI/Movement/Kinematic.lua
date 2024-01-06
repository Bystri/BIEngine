
Static = class(nil,
{
	_position = nil,
	_orientation = 0,
});

KinematicSteeringOutput = class(nil,
{
	_linear = nil,
	_angular = 0,
});

--Вычесляет поворот относительно скорости или, если она нулевая, возвращает поворот - current
function newOrintation(current, velocity)
	-- убеждаемся, то у нас есть скорость
	if (velocity:Length2() > 0) then
		--тогда высчитываем новую ориентацию по вектору скорости
		return math.atan(velocity.y, velocity.x)
	else
		return current;
	end
end

--Алгортим движения, для преследования цели
KinematicSeek = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxSpeed = 0,
});

--Высчитывает необходимую скорость чтобы убежать от цели
function KinematicSeek:getSteering()
	--Сначала получаем позицию до цели
	local velocity = self._target._position - self._character._position;
		
	--Затем назначаем максимальную скорость 
	velocity:Normalize();
	
	velocity = velocity * self._maxSpeed;
	
	local result = KinematicSteeringOutput:Create({_linear = velocity, _angular = 0});
	
	--Разворачиваемся лицом от цели
	self._character._orientation = newOrintation(self._character._orientation, velocity);
	
	return result;
end



--Алгортим движения, для ухода от цели
KinematicFlee = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxSpeed = 0,
});

--Высчитывает необходимую скорость чтобы убежать от цели
function KinematicFlee:getSteering()
	--Сначала получаем позицию до цели
	local velocity = self._character._position - self._target._position;
		
	--Затем назначаем максимальную скорость 
	velocity:Normalize();
	
	velocity = velocity * self._maxSpeed;
	
	local result = KinematicSteeringOutput:Create({_linear = velocity, _angular = 0});
	
	--Разворачиваемся лицом от цели
	self._character._orientation = newOrintation(self._character._orientation, velocity);
	
	return result;
end



--Алгоритм движения, который учивает дистанцию до цели, чтобы плавно останавливаться при подходе к ней
KinematicArrive = class(nil,
{
	_character = Static:Create(),
	_target = Static:Create(),
	
	_maxSpeed = 0,
	
	--Дистанция до цели, в пределах которой мы можем остановиться
	_radius = 0,
	
	_timeToTarget = 0.25
});

--Высчитывает необходимую скорость чтобы достичь цели
function KinematicArrive:getSteering()
	--Сначала получаем позицию до цели
	local velocity = self._target._position - self._character._position;
		
	--Если мы в необходимой близости от цели - можем остановиться
	if (velocity:Length() < self._radius) then
		return nil;
	end
	
	--Если нам нужно двигаться к цели, то мы бы хотели оказаться там за _timeToTarget секунд
	velocity = velocity / self._timeToTarget;
	
	--Если это слишком быстро - ограничиваем скорость
	if (velocity:Length() > self._maxSpeed) then 
		velocity:Normalize();
		velocity = velocity * self._maxSpeed;
	end;
	
	local result = KinematicSteeringOutput:Create({_linear = velocity, _angular = 0});
	
	--Разворачиваемся лицом к цели
	self._character._orientation = newOrintation(self._character._orientation, velocity);
	
	return result;
end


--Алгоритм движения, который позволяет персонажу двигаться вперед (по направлению движения) немного извиваясь
KinematicWander = class(nil,
{
	_character = Static:Create(),
	
	_maxSpeed = 0,
	
	--Максимальная скорость поворота.
	_maxRotation = 0,
});

--Возвращает случайное числов в диапазоне [-1, 1]
function randomBinomial()
	return math.random() - math.random();
end

--Высчитывает необходимую скорость чтобы достичь цели
function KinematicWander:getSteering()
	--Сначала получаем вектор скорости вдоль направления обзора
	local velocity = Vec3:Create(
		{
			x = -math.cos(math.rad(self._character._orientation)), 
			y = math.sin(math.rad(self._character._orientation)),
			z = 0
		});

	velocity = velocity * self._maxSpeed;
	
	--Случайно меняем нашу ориентацию
	local rotation = randomBinomial() * self._maxRotation;
	
	local result = KinematicSteeringOutput:Create({_linear = velocity, _angular = rotation});
	return result;
end
