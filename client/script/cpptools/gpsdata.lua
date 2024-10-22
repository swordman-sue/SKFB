-- 按time_inteval秒定位一次获取数据后发送给服务器
local time_inteval = 30 * 60
local location_timer = nil
LocationService = LocationService or {}

--处理经纬度数据
function LocationService.handlerLocationData(code,data)
	--LogTool.LogInfo("LocationService.handlerLocationData : %s",data)
	--停止继续获取位置
	CPPLocation:StopLocation()
	--获取经纬度成功
	if code == 0 then
		local pos_data, pos, err = CPPGameLib.JSON.decode(data)
		if not err then
			--LogTool.LogInfo("LocationService  latitude : %s longitude : %s",pos_data.latitude,pos_data.longitude)
			-- NetWorkHandleMsg.RequestChangePos(pos_data)
		end
	elseif code == -1 then
		
    elseif code == -2 then
		
	end
end
local location_callback = GlobalCallbackMgr:AddCallBackFunc(LocationService.handlerLocationData)

LocationService.getLocationData = function()
	local startLocationService = function()
		--LogTool.LogInfo("startLocationService  lua")
		LocationService.startLocationService(true)
	end
	--先执行一次
	startLocationService()
	--定时执行
	location_timer = CPPGameLib.TimerCallBack(startLocationService,time_inteval)
end

function LocationService.startLocationService(isAuto)
	local isOpenGps  = CPPSystemTools.IsOpenLocationService()
	--没有开启且自动获取数据时不定位
	if isAuto then
		if not isOpenGps then
			return
		else
			CPPLocation:StartLocation(location_callback)
		end
	else
		CPPLocation:StartLocation(location_callback)
	end
end
--开始获取位置
--LocationService.getLocationData()