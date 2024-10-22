
--[[
%% @module: 公会动态
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TemplUnionDynamic = Modules.TemplUnionDynamic or BaseClass(BaseTabView)

function Modules.TemplUnionDynamic:__init()
    self.__tab_params_ex = {6}
end

--选项卡界面创建完成回调
function Modules.TemplUnionDynamic:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplUnionDynamic:__OpenCallback()
	--请求事件
	CallModuleFunc(ModuleType.UNION , "GuildEventListReq")
	
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_TRENDS,function (_, data)
        self:__SetShowUI( data )
    end)
    
end

--选项卡界面关闭回调
function Modules.TemplUnionDynamic:__CloseCallback()

end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplUnionDynamic:__Dispose()
	if self.__tab then
		self.__tab:DeleteMe()
		self.__tab = nil
	end
end

--读取组件
function Modules.TemplUnionDynamic:__Getwidget()
    --居中层
    self.Image_content = self:GetWidget("Image_content")
end

--创建组件
function Modules.TemplUnionDynamic:__ShowUI()
    self.__tab:SetContentSize( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight())
    self.__tab:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_content:AddChild( self.__tab:GetNode() )
    PosTool.Center( self.__tab )
end

--设置组件
function Modules.TemplUnionDynamic:__SetShowUI( data )
	self.__tab:SetData( data )
    self.__tab:SetCeil()
end

function Modules.TemplUnionDynamic:Ceil( index , date )
	
	local t = CPPGameLib.GetConfig("GuildEvent", date.event_id )

	local tab = 
		{
			{ zi = CPPGameLib.GetString( "common_brackets" , TimeTool.GetTimeFormat( date.time , TimeTool.defaul_format[16]) ) , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly },
			{ zi = string.format( t.desc , date.target_name , date.value ) , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content  },
		}
	local text = Util:LabCenter_3( tab , Macros.TypefaceSize.slightly , self.Image_content:GetContentWidth() - 10 )

	local layer = Util:Layout( self.Image_content:GetContentWidth() , text:GetContentHeight() + 10 )

	text:SetAnchorPoint( 0 , 1 )
	text:SetPosition( 5 , layer:GetContentHeight() - 5 )
	layer:AddChild( text )

	return layer
end