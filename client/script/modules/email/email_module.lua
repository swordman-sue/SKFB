
Modules = Modules or {}

Modules.EmailModule = Modules.EmailModule or BaseClass(BaseModule)

function Modules.EmailModule:__init()
	self:__RegistProtocolForOpenView(BaseViewType.EMAIL, Net.ProtocolNo.C2S_EMAIL_LIST_REQ, false)
end

function Modules.EmailModule:__delete()
end

function Modules.EmailModule:EmailListRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EMAIL_LIST_REQ)  
	--发送请求消息
	GlobalNetAdapter:OnSend(data) 
end

function Modules.EmailModule:EmailListResponse( info )
	for i,v in ipairs(info.email_list) do
		v.context_list = v.context_list:GetData()
	end
	self.__email_list = info.email_list
	table.sort(self.__email_list,function (info_1,info_2)
		return info_1.receive_time > info_2.receive_time
	end)
end

--邮件
function Modules.EmailModule:EmailNewResponse(info)
	if info.context_list then
		if self.__email_list then
			table.insert(self.__email_list,1,info)
			local view = self:GetView()
			if view and view:IsOpen() then
				view:AttemData(info)
			end
		end
	end
end

function Modules.EmailModule:GetEmailList()
	return self.__email_list
end
