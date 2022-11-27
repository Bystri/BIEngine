
print("Lua init");

-- Переопределяем функцию require, чтобы мы подцепляли функции скрипты из хранилища ресурсов
do
	local OldRequire = require;  -- сохраняем старую (встроенную) реализаци require() 
	local resourceIdMap = {};  -- map ресурсов, которые мы загрузили
	
	require = function(script)
		-- Загружаем ресурс, который еще не был загружен
		if (not resourceIdMap[script]) then
			print("Load " .. script);
			if (LoadAndExecuteScriptResource(script)) then
				print("Loaded from assets");
				resourceIdMap[script] = true;
			else
				-- Если не получилось загрузить ресурс нашими средствами, загружаем его встроенной реализацией require()
				print("Load from standard require");
				OldRequire(script);
			end
		end
	end
end

--Classes and Inheritance
function class(baseClass, body)
	local ret = body or {};
	
	-- if there's a base class, attach our new class to it
	if (baseClass ~= nil) then
		setmetatable(ret, ret);
		ret.__index = baseClass;
		ret.base = baseClass;
	end
	
	-- Add the Create() function.  This will end up being added to each subclass which isn't ideal.  This 
	-- function should probably be stored in a single place so we only have to create it once.
	-- 		-self: The self pointer that's calling this function.
	--		-constructionData: The table sent in as construction data.  Think of it like a constructor.
	--		-originalSubClass: Used for recursion.  Don't pass anything into this parameter, it's filled 
	--						   automatically.  It's used in case the C++ side needs access to the leaf 
	--						   subclass that is being instantiated.  For an example, see ScriptProcess 
	--						   in C++.
	ret.Create = function(self, constructionData, originalSubClass)
		local obj;
		if (self.__index ~= nil) then
			if (originalSubClass ~= nil) then
				obj = self.__index:Create(constructionData, originalSubClass);
			else
				obj = self.__index:Create(constructionData, self);
			end
		else
			obj = constructionData or {};
		end
		
		setmetatable(obj, obj);
		obj.__index = self;
		
		-- copy any operators over
		if (self.__operators ~= nil) then
			for key, val in pairs(self.__operators) do
				obj[key] = val;
			end
		end
		
		return obj;
	end
	
	-- Returns true if otherClass appears in this objects class hierarchy anywhere.
	ret.IsInstance = function(self, otherClass)
		local cls = self.__index;
		while cls do
			if cls == otherClass then 
				return true
			end
			cls = cls.base
		end
		return false
	end
	
	return ret;
end

-----------------------------------------------------------------------------------------------------------------------
-- Vec2 class
-----------------------------------------------------------------------------------------------------------------------
Vec2 = class(nil, 
{
	x = 0,
	y = 0,
	__operators = {},
});

function Vec2:Length()
	return math.sqrt((self.x * self.x) + (self.y * self.y));
end

function Vec2:Length2()
	return ((self.x * self.x) + (self.y * self.y));
end

function Vec2:Normalize()
	local len = self:Length();
	self.x = self.x / len;
	self.y = self.y / len;
end

function Vec2.__operators.__add(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x + right;
		temp.y = left.y + right;
	else
		temp.x = left.x + right.x;
		temp.y = left.y + right.y;
	end
	return temp;
end

function Vec2.__operators.__sub(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x - right;
		temp.y = left.y - right;
	else
		temp.x = left.x - right.x;
		temp.y = left.y - right.y;
	end
	return temp;
end

function Vec2.__operators.__mul(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x * right;
		temp.y = left.y * right;
	else
		temp.x = left.x * right.x;
		temp.y = left.y * right.y;
	end
	return temp;
end

function Vec2.__operators.__div(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x / right;
		temp.y = left.y / right;
	else
		temp.x = left.x / right.x;
		temp.y = left.y / right.y;
	end
	return temp;
end

-----------------------------------------------------------------------------------------------------------------------
-- Vec3 class
-----------------------------------------------------------------------------------------------------------------------
Vec3 = class(nil, 
{
	x = 0,
	y = 0,
	z = 0,
	__operators = {},
});

function Vec3:Length()
	return math.sqrt((self.x * self.x) + (self.y * self.y) + (self.z * self.z));
end

function Vec3:Length2()
	return ((self.x * self.x) + (self.y * self.y) + (self.z * self.z));
end

function Vec3:Normalize()
	local len = self:Length();
	self.x = self.x / len;
	self.y = self.y / len;
	self.z = self.z / len;
end

function Vec3.__operators.__add(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x + right;
		temp.y = left.y + right;
		temp.z = left.z + right;
	else
		temp.x = left.x + right.x;
		temp.y = left.y + right.y;
		temp.z = left.z + right.z;
	end
	return temp;
end

function Vec3.__operators.__sub(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x - right;
		temp.y = left.y - right;
		temp.z = left.z - right;
	else
		temp.x = left.x - right.x;
		temp.y = left.y - right.y;
		temp.z = left.z - right.z;
	end
	return temp;
end

function Vec3.__operators.__mul(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x * right;
		temp.y = left.y * right;
		temp.z = left.z * right;
	else
		temp.x = left.x * right.x;
		temp.y = left.y * right.y;
		temp.z = left.z * right.z;
	end
	return temp;
end

function Vec3.__operators.__div(left, right)
	local temp = Vec2:Create();
	if (type(right) == "number") then
		temp.x = left.x / right;
		temp.y = left.y / right;
		temp.z = left.z / right;
	else
		temp.x = left.x / right.x;
		temp.y = left.y / right.y;
		temp.z = left.z / right.z;
	end
	return temp;
end


-----------------------------------------------------------------------------------------------------------------------
-- Event system
-----------------------------------------------------------------------------------------------------------------------
Event = {};
EventType = {};
