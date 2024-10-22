------------------------------------------------------------------------------------------------
--数学库
-------------------------------------------------------------------------------------------------
MathTool = MathTool or {}

MathTool.MAX_NUM = 88888888
MathTool.MIN_NUM = -88888888

MathTool.HORZ_INVALID = 0
MathTool.HORZ_LEFT = -1
MathTool.HORZ_RIGHT = 1

MathTool.EPSINON = 1e-6

------------------------------------------------------------------------------------------------
--二维向量
-------------------------------------------------------------------------------------------------
Vector2D = Vector2D or BaseClass()

Vector2D.METATABLE_FUNC_EQ = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_EQ] 参数无效")
		return false
	end
	return (math.abs(a.x - b.x) <= MathTool.EPSINON) and (math.abs(a.y - b.y) <= MathTool.EPSINON)
end
Vector2D.METATABLE_FUNC_LT = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_LT] 参数无效")
		return false
	end
	return a.x < b.x and a.y < b.y
end
Vector2D.METATABLE_FUNC_ADD = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_ADD] 参数无效")
		return self
	end
	return Vector2D.New(a.x + b.x, a.y + b.y)
end
Vector2D.METATABLE_FUNC_SUB = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_SUB] 参数无效")
		return self
	end
	return Vector2D.New(a.x - b.x, a.y - b.y)
end
Vector2D.METATABLE_FUNC_MUL = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_MUL] 参数无效")
		return self
	end
	return Vector2D.New(a.x * b.x, a.y * b.y)
end
Vector2D.METATABLE_FUNC_DIV = function(a, b)
	if type(a) ~= "table" or type(b) ~= "table" then
		LogTool.LogError("[Vector2D.METATABLE_FUNC_DIV] 参数无效")
		return self
	end
	return Vector2D.New(a.x / b.x, a.y / b.y)
end

function Vector2D:__init(x, y)
	self.x = x or 0
	self.y = y or 0

	-- 原有metatable
	local orig_metatable = getmetatable(self) or {}

	orig_metatable.__eq = Vector2D.METATABLE_FUNC_EQ
	orig_metatable.__lt = Vector2D.METATABLE_FUNC_LT

	orig_metatable.__add = Vector2D.METATABLE_FUNC_ADD
	orig_metatable.__sub = Vector2D.METATABLE_FUNC_SUB
	orig_metatable.__mul = Vector2D.METATABLE_FUNC_MUL
	orig_metatable.__div = Vector2D.METATABLE_FUNC_DIV

	-- 重载操作符
	setmetatable(self, orig_metatable)
end

function Vector2D:Init(pos)
	self.x = pos.x
	self.y = pos.y
end

function Vector2D:Length()
	-- 矢量长度
	return math.sqrt(self.x * self.x + self.y * self.y)
end

function Vector2D:LengthSqr()
	-- 矢量平方
	return self.x * self.x + self.y * self.y
end

function Vector2D:Normalize(vec_len)
	-- 自身单位化
	vec_len = vec_len or self:Length()
	if vec_len == 0 then
		return
	end
	self.x = self.x / vec_len
	self.y = self.y / vec_len
end

function Vector2D:IsZero()
	return math.abs(self.x) <= MathTool.EPSINON and math.abs(self.y) <= MathTool.EPSINON
end

function Vector2D:IsNear(vec, delta)
	local dis_vec = self - vec
	return not dis_vec:IsLongerThan(delta)
end

function Vector2D:IsNearHorz(vec, delta)
	local dis_vec = self - vec
	dis_vec.y = 0
	return not dis_vec:IsLongerThan(delta)
end

function Vector2D:IsLongerThan(len)
	return self:LengthSqr() > len * len
end

function Vector2D:Multi(var)
	return Vector2D.New(self.x * var, self.y * var)
end

function Vector2D:MultiEx(var)
	self.x = self.x * var
	self.y = self.y * var
end

function Vector2D:Dot(vec)
	-- 矢量点积
	return self.x * vec.x + self.y * vec.y
end

function Vector2D:Cross(vec)
	-- 矢量叉积
	return self.x * vec.y - vec.x * self.y
end

function Vector2D:Dist(vec)
	return (Vector2D.New(self.x - vec.x, self.y - vec.y)):Length()
end

function Vector2D:GetMaxAxis()
	-- 得到矢量最大轴
	return self.x >= self.y and Vector2D.New(1, 0) or Vector2D.New(0, 1)
end

function Vector2D:GetMinAxis()
	-- 得到矢量最小轴
	return self.x < self.y and Vector2D.New(1, 0) or Vector2D.New(0, 1)
end

------------------------------------------------------------------------------------------------
--区间
-------------------------------------------------------------------------------------------------
Section = Section or BaseClass()

function Section:__init(min, max, min_closed, max_closed)
	self.min = min
	self.max = max
	--左闭区间?
	self.min_closed = min_closed
	--右闭区间?
	self.max_closed = max_closed
end

function Section:InSection(val)
	if not self.min and not self.max then
		return false
	end

	--左区间无穷小
	if not self.min then
		if val > self.max or 
			(not self.max_closed and val == self.max) then
			return false
		end
		return true
	end

	--右区间无穷大
	if not self.max then
		if val < self.min or 
			(not self.min_closed and val == self.min) then
			return false
		end
		return true
	end

	if val < self.min or val > self.max or 
		(not self.min_closed and val == self.min) or
		(not self.max_closed and val == self.max) then
		return false
	end
	return true
end

------------------------------------------------------------------------------------------------
--其他
-------------------------------------------------------------------------------------------------
--截取数字精度
function MathTool.GetFloatPrecision(num, precision)
	local ins = math.pow(10, precision)
	num = math.floor(num * ins)
	num = num / ins
	return num
end

--矩形与矩形的碰撞检测
function MathTool.IntersectsRect(rect1, rect2)
    return not (rect1.max_x < rect2.min_x or
    		    rect2.max_x < rect1.min_x or
    		    rect1.max_y < rect2.min_y or
    		    rect2.max_y < rect1.min_y)
end

--矩形与点的碰撞检测
function MathTool.IsInRect(rect_x, rect_y, rect_w, rect_h, x, y)
    if (x>=rect_x) and (x<=rect_x+rect_w) and (y>=rect_y) and (y<=rect_y+rect_h) then
        return true
    end
    return false
end

--计算向量X轴方向(pos1->pos2)
function MathTool.CalcHorzDirBetweenPoints(pos1, pos2)
	local pos3 = pos2 - pos1
	return pos3.x > 0 and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT
end

--设置随机种子
function MathTool.SetRandomSeed(var)
    math.randomseed(var or tonumber(tostring(TimeTool.GetTime()):reverse():sub(1, 6)))
end

--获取随机数
function MathTool.GetRandom(m, n)
    if not MathTool.set_seed then
    	MathTool.SetRandomSeed()
        MathTool.set_seed = true
    end

    if not m then
        return math.random()
   
    elseif m and not n then
    	if m < 1 then
    		return 0
    	end
        return math.random(m)
   
    else
        if m >= n then
            return math.random(n, m)
        else
	        return math.random(m, n)
       end
    end
end

--从list中随机抽取n个不重复的元素(战斗专用)
function MathTool.SelectRandomItemsFrmList(list, n)
	local sel_tbl = {}
	
	if not list or not n or n <= 0 then
		return sel_tbl
	end

	local org_index_num = 0
	local org_index_tbl = {}
	for i = 1, #list do
		org_index_num = org_index_num + 1
		table.insert(org_index_tbl, i)
	end

	local sel_index, org_index
	for i = 1, n do
		sel_index = MathTool.GetRandom(1, org_index_num)
		org_index = org_index_tbl[sel_index]
		if not org_index or not list[org_index] then
			break
		end
		table.insert(sel_tbl, list[org_index])

		if sel_index < org_index_num then
			org_index_tbl[sel_index] = org_index_tbl[org_index_num]
		end
		org_index_num = org_index_num - 1
	end

	return sel_tbl
end

--随机算法
MathTool.MY_RAND_SEED = tonumber(tostring(TimeTool.GetTime()):reverse():sub(1, 6))
MathTool.MY_RAND_RANGE = 134217728
local function my_rand(min, max)
	min = math.max(min, -MathTool.MY_RAND_RANGE)
	max = math.min(max, MathTool.MY_RAND_RANGE)
	if min == max then
		return min
	end

	local _, val = math.modf((MathTool.MY_RAND_SEED * 1103515245 + 12345) / MathTool.MY_RAND_RANGE)
	val = val * MathTool.MY_RAND_RANGE
	MathTool.MY_RAND_SEED = val

	local ret = val
	local inv = max - min + 1
	if inv > 0 then
		local tmp, _ = math.modf(val / inv)
		ret = val - tmp * inv + min
	else
		return 0
	end
	return ret
end

--[[
转换概率系数(万分比->小数)
--]]
function MathTool.TransProbCoeff2Decimal(var)
	if not var then
		return 0
	end
	return var * 0.0001
end

--[[
转换概率系数(万分比->百分比)
--]]
function MathTool.TransProbCoeffPercentage(var)
	if not var then
		return 0
	end
	return var * 0.01
end

--[[
概率检测
@ params：
	prob_coeff		[number]概率系数(万分比)
--]]
function MathTool.CheckProbability(prob_coeff)
	local prob_coeff = MathTool.TransProbCoeff2Decimal(prob_coeff)
	return MathTool.GetRandom() <= prob_coeff
end

--[[
获取整数位的位数
--]]
function MathTool.GetIntDigitNum(var)
	var = math.floor(var)
	var = tostring(var)
	return string.len(var)
end

--[[
精确到小数点后几位
比如：20.69563 希望精确到第3位就是20.695
--]]
function MathTool.GetDecimalPointNum(org_num,decimal_num)
	if not org_num then
		return 0
	end
	decimal_num = decimal_num or 1
	local change_num = org_num * math.pow(10,decimal_num)
	return math.floor(change_num)/math.pow(10,decimal_num)
end

--[[
比较浮点数
--]]
function MathTool.CompareFloat(var1, var2)
	return math.abs(var1 - var2) <= MathTool.EPSINON
end
