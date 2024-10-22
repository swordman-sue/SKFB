--
-- @brief 天梯推送
-- @author: yjg
-- @date: 2017年11月3日15:24:06
--

Modules = Modules or {}

Modules.PushLadderView = Modules.PushLadderView or BaseClass(BaseView)

function Modules.PushLadderView:__init( )
    self.__layout_name = "templ_push_ladder"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI( id )
    end
end

function Modules.PushLadderView:__Getwidget( ... )
    -- 内容背景
    self.Image_morrow = self:GetWidget("Image_morrow") 

    --时间框
    self.Image_frame = self:GetWidget("Image_frame")

    --按钮
    self.Button_btn = Util:WidgetButton(  
                                            self:GetWidget("Button_btn") , 
                                            CPPGameLib.GetString("common_confirm") , 
                                            CPPGameLib.Handler(self , self.OnButtonClose) ,
                                            nil,nil,Macros.Color.button_red
                                            ) 
    --按钮
    self.Button_close = Util:WidgetButton(  
                                            self:GetWidget("Button_close") , 
                                            "" , 
                                            CPPGameLib.Handler(self , self.OnButtonClose) 
                                            ) 
end

function Modules.PushLadderView:__ShowUI(  )
    self.Image_morrow:SetImage(Resource.PathTool.GetUIResPath("push/ladder/ladder",true), TextureResType.LOCAL)

    local info = self:CountDown( )
        
    local t1 = {
            [1] = "ladder_day_1",
            [2] = "ladder_day_2",
            [3] = "ladder_day_3",
        }

    local countDown = info.countDown
    if not info.countDown or info.countDown <= 0 then
        countDown = nil
    end

    if countDown then

        local icon_point = Resource.PathTool.GetUIResPath("push/ladder/ladder_no_open",true)
        local ladder_no_open = Util:Sprite( icon_point )

        local icon_point = Resource.PathTool.GetUIResPath("push/ladder/" .. t1[info.countDown] ,true)
        local ladder_day_num = Util:Sprite( icon_point )

        local icon_point = Resource.PathTool.GetUIResPath("push/ladder/ladder_day" ,true)
        local ladder_day = Util:Sprite( icon_point )

        local layer = Util:Layout( ladder_no_open:GetContentWidth() + 5 + ladder_day_num:GetContentHeight() + 5 + ladder_day:GetContentWidth() )
        layer:SetAnchorPoint( 0.5 , 0.5 )
        layer:SetPosition( self.Image_frame:GetContentWidth()/2 , self.Image_frame:GetContentHeight() + 55 )
        self.Image_frame:AddChild( layer )

        ladder_no_open:SetAnchorPoint( 0 , 0.5 )
        ladder_no_open:SetPosition( 0 , layer:GetContentHeight()/2 )
        layer:AddChild( ladder_no_open ) 

        layer:AddChild( ladder_day_num ) 
        PosTool.RightTo( ladder_no_open , ladder_day_num , 5 )

        layer:AddChild( ladder_day ) 
        PosTool.RightTo( ladder_day_num , ladder_day , 5 )

    else
        
        local icon_point = Resource.PathTool.GetUIResPath("push/ladder/ladder_open" ,true)
        local ladder_open = Util:Sprite( icon_point )
        ladder_open:SetAnchorPoint( 0.5 , 0.5 )
        ladder_open:SetPosition( self.Image_frame:GetContentWidth()/2 , self.Image_frame:GetContentHeight() + 55 )
        self.Image_frame:AddChild( ladder_open )

    end
end

function Modules.PushLadderView:OnButtonClose()
    if unpack(self.__open_data) == "push" then
        self:CloseManual(true)
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.LADDER)
    else
        self:CloseManual()
    end
end

function Modules.PushLadderView:CountDown()

    local cnf = CPPGameLib.GetConfig("LadderCompetitionSeason" , nil , nil , true)

    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)

    local t1 = {}
    for i , v in ipairs( cnf ) do

        local nowTime = n_nian .. n_yue .. n_ri .. string.format( "%02d" , n_shi ) .. string.format( "%02d" , n_fen )
        local openTime = v.start_date .. string.format( "%04d" , v.start_time )
        local endTime = v.over_date .. string.format( "%04d" , v.over_time )

        local o_nian = string.sub( v.start_date , 0 , 4 )
        local o_yue = string.sub( v.start_date , 5 , 6 )
        local o_ri = string.sub( v.start_date , 7 , 8 )
        local o_shi = string.sub( v.start_time , 0 , 2 )
        local o_fen = string.sub( v.start_time , 3 , 4 )

        local e_nian = string.sub( v.over_date , 0 , 4 )
        local e_yue = string.sub( v.over_date , 5 , 6 )
        local e_ri = string.sub( v.over_date , 7 , 8 )
        local e_shi = string.sub( v.over_time , 0 , 2 )
        local e_fen = string.sub( v.over_time , 3 , 4 )

        t1 = { 
                --活动开始时间
                openTime = o_nian.."-"..o_yue.."-"..o_ri,
                --活动结束时间
                closeTime = e_nian.."-"..e_yue.."-"..e_ri,
                --赛季
                season = v.index ,
                --活动开始时段
                daysOpenTime = o_shi .. ":" .. o_fen ,
                --活动结束时段
                daysCloseTime = e_shi .. ":" .. e_fen ,
            }
        --开启中
        if ( tonumber( openTime ) <= tonumber( nowTime ) ) and ( tonumber( endTime ) >= tonumber( nowTime ) ) then
            t1.countDown = 0

            break
        elseif ( tonumber( openTime ) >= tonumber( nowTime ) ) then

            local time = TimeTool.GetTimeStampFromDate( o_nian , o_yue , o_ri )

            t1.countDown = math.ceil( ( time - n_Time ) / ( 24 * 60 * 60 ) )

            break
        end
    end

    return t1
end

