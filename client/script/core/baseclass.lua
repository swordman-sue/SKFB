
-- 保存类类型的虚表(key:class_type, value:vtbl)
local _class = {}
 
function BaseClass(super)
	-- 生成一个类类型
	local class_type = {}

	-- 在创建对象的时候自动调用
	class_type.__init = false
	class_type.__delete = false
	class_type.__super = super
	class_type.New = function(...)
		-- 生成一个类对象
		local obj = {}
		obj.__class_type = class_type

		-- 在初始化之前注册基类函数
		setmetatable(obj, { __index = _class[class_type] })

		-- 调用初始化函数
		do
			local create 
			create = function(c, ...)
				if c.__super then
					create(c.__super, ...)
				end
				if c.__init then
					c.__init(obj, ...)
				end
			end

			create(class_type, ...)
		end

		-- 注册一个delete函数
		obj.DeleteMe = function(obj_self, ...)
			obj_self.__is_deleted = true
			local now_super = obj_self.__class_type 
			while now_super ~= nil do	
				if now_super.__delete then
					now_super.__delete(obj_self, ...)
				end
				now_super = now_super.__super
			end
		end

		return obj
	end

	local vtbl = {}
	_class[class_type] = vtbl

	setmetatable(class_type, {__newindex =
		function(t,k,v)
			-- 注册到vtbl
			vtbl[k] = v
		end
		,
		-- For call parent method when override
		__index = vtbl,
	})

	if super then
		setmetatable(vtbl, {__index =
			function(t,k)
				local ret = _class[super][k]
				-- 避免热更后，无法使用最新的super.funcA，当child.funcA == nil
				-- Do not do accept, make hot update work right! 
				-- vtbl[k] = ret
				return ret
			end
		})
	end
 
	return class_type
end

function IsClass(obj, class)
	return obj.__class_type == class
end

function IsClassOrInheritClass(obj, class)
	local now_class = obj.__class_type 
	while now_class ~= nil do	
		if now_class == class then
			return true
		end
		now_class = now_class.__super
	end	
end
