
LuaGCHandle = LuaGCHandle or {}

local is_custom_gc
local gc_collect_finish_time = 0
local gc_collect_step = 3
local collect_increase_var = 4 * 1024
local last_memory_size = 16 * 1024

LuaGCHandle.StartCustomGC = function()
	is_custom_gc = true
	collectgarbage("stop")	
end

LuaGCHandle.StartDefaultGC = function()
	is_custom_gc = false
	collectgarbage("restart")	
end

LuaGCHandle.GCCollectGarbage = function(interval)
	if not is_custom_gc then
		return
	end

	gc_collect_finish_time = gc_collect_finish_time + interval
	if collectgarbage("step", gc_collect_step) then	
		local cur_memory_size = collectgarbage("count")
		if(cur_memory_size - last_memory_size > collect_increase_var) then
			gc_collect_step = gc_collect_step + 1
		elseif(last_memory_size - cur_memory_size > collect_increase_var) then
			gc_collect_step = math.max(1, gc_collect_step - 1)
		end
		last_memory_size = cur_memory_size
		gc_collect_finish_time = 0
	end
end
