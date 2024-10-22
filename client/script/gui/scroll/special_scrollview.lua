
GUI = GUI or {}

GUI.SpecialScrollView = GUI.SpecialScrollView or BaseClass(GUI.ScrollView)

function GUI.SpecialScrollView:__init()
    self.__max_factor = 0.9
    self.__scale_factor = 0.3
    self.__opacity_factor = 0.5

    --设置松手回调
    self.__touch_controller:SetTouchUpCallback(function ()      
        self:__SetTouchUpCallback()
    end)       
    --设置回弹回调
    self.__touch_controller:SetFixPosXCallback(function (scroll_x, off)
        return self:__SetFixPosCallback(scroll_x, off)
    end) 
end

--更新回调
function GUI.SpecialScrollView:Update(elapse_time)
    GUI.ScrollView.Update(self, elapse_time)

    if self.__need_action_update then 
        self:__MyScrollCallBack()   
    end
end

--选中条目
function GUI.SpecialScrollView:SelectItem(index)
    if not index then
        return
    end     
    local select_changed = self:__IsSelectedChanged(index)
    if select_changed then
        local selected_item = self:GetItem(index, true)
        if not selected_item then
            return
        end

        if self.__need_action_update then
            return
        end

        local is_need_callback 

        if self.__select_filter_func then
            --选择条件过滤
            if not self.__select_filter_func(index, selected_item:GetData()) then
                if self.__select_filter_func_callback then
                    self.__select_filter_func_callback(index,selected_item:GetData())
                end             
                return
            end
        end

        local last_selected_item = self:GetItem(self.__selected_item_index) 
        if last_selected_item then
            --反选上一个
            last_selected_item:SetSelected(false)
        end

        if selected_item then
            --选择下一个
            selected_item:SetSelected(true)
        end

        if self.__selected_item_index then
            if self.__selected_item_index ~= index then
                is_need_callback = false                
                if selected_item then
                    local target_point = NodeUnity.ConvertPosBetweenTwoContainer(selected_item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))  
                    self.__need_action_update = true
                    self.__parent_node:SetTouchEnabled(false)
                    self.__touch_controller:ActionLocatePos(self.__screen_center_point.x - target_point.x, self.__screen_center_point.y - target_point.y, 0.3, function ()
                        self.__need_action_update = false
                        self.__parent_node:SetTouchEnabled(true)
                        --选择条目过渡效果完成回调
                        if self.__select_item_effect_end_callback then
                            self.__select_item_effect_end_callback()
                        end
                    end)
                end
            else
                --是否需要修正位置
                if not self.__is_need_updatescrollview then
                    self.__is_no_update_scrollview = true
                end
                is_need_callback = true
            end
        else
            is_need_callback = false
        end

        --记录选中条目索引
        self.__selected_item_index = index

        --直接调用回调
        is_need_callback = self.__is_need_callback or is_need_callback
        if self.__select_callback and is_need_callback then
            self.__select_callback(self.__selected_item_index, selected_item:GetData())
        end
    end
end

--将索引为Index的条目移动到屏幕中心
function GUI.SpecialScrollView:MoveItemToScreenCenterByIndex(index)
    --获取中心点世界坐标
    self.__screen_center_point = NodeUnity.ConvertPosBetweenTwoContainer(self:GetNode(), self.__scissor_node:GetNode(), COCOPoint(0,0))  
    local pos_x, pos_y = self:__CalcPositionByIndex(index)
    local target_point = NodeUnity.ConvertPosBetweenTwoContainer(self.__move_node:GetNode(), self.__scissor_node:GetNode(), COCOPoint(pos_x, pos_y)) 
    self:LocatePos(self.__screen_center_point.x - target_point.x, self.__screen_center_point.y - target_point.y, true)
    
    self:__InitScrollViewItemStatus()
end

--检测索引为Index的条目是否位于屏幕中心，不是的话就移到中心
function GUI.SpecialScrollView:CheckItemIsOnScreenCenterByIndex(index)
    if self.__selected_item_index then
        --没有在移动
        if not self.__need_action_update then
            if self.__selected_item_index == index then
                GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_RESP,index)
            else
                local last_selected_item = self:GetItem(self.__selected_item_index) 
                if last_selected_item then
                    --反选上一个
                    last_selected_item:SetSelected(false)
                end
                
                local selected_item = self:GetItem(index, true)
                if selected_item then
                    local target_point = NodeUnity.ConvertPosBetweenTwoContainer(selected_item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))  
                    self.__need_action_update = true
                    self.__parent_node:SetTouchEnabled(false)
                    self.__touch_controller:ActionLocatePos(self.__screen_center_point.x - target_point.x, self.__screen_center_point.y - target_point.y, 0.3, function ()
                        self.__need_action_update = false
                        self.__parent_node:SetTouchEnabled(true)

                        --选择下一个
                        selected_item:SetSelected(true)

                        --记录选中条目索引
                        self.__selected_item_index = index
                        --选择条目过渡效果完成回调
                        if self.__select_item_effect_end_callback then
                            self.__select_item_effect_end_callback()
                        end
                    end)
                end
            end
        end
    end
end


--[[
--是否直接调用回调 is_need_callback
--是否变灰 is_turn_gray
--解锁副本的章数 lockChapterNum
--屏幕最大显示数量 max_num
]]--
--设置需要的数据 
function GUI.SpecialScrollView:SetNecessaryData(lockChapterNum,max_num,is_need_callback,is_turn_gray)
    self.__is_need_callback = is_need_callback
    self.__is_turn_gray = is_turn_gray
    self.__lock_chapter_num = lockChapterNum or self.__lock_chapter_num
    self.__touchsize_maxnum = max_num or self.__touchsize_maxnum
end

--设置是否无视情况都要修正条目位置
function GUI.SpecialScrollView:SetNeedUpdateScrollView(is_need_update)
    self.__is_need_updatescrollview = is_need_update
end

--设置透明度,缩放比例,缩放系数等数据
--[[
--最大比例 max_factor
--缩放系数 scale_factor
--透明系数 opacity_factor
]]--
function GUI.SpecialScrollView:SetChangeData(max_factor,scale_factor,opacity_factor)
    self.__max_factor = max_factor or 0.9
    self.__scale_factor = scale_factor or 0.3
    self.__opacity_factor = opacity_factor or 0.5
end

--章节通关效果
function GUI.SpecialScrollView:ChapterPassedAllEffect(chapter_index,fun)
    self.__chapter_index = chapter_index

    local selected_item = self:GetItem(chapter_index)
    local img_normal = selected_item:GetImgNormal()
    img_normal:SetColor(unpack(Macros.Color.GrayBlack))

    local last_selected_item = self:GetItem(self.__selected_item_index) 
    if last_selected_item then
        --反选上一个
        last_selected_item:SetSelected(false)
    end        

    local target_point = NodeUnity.ConvertPosBetweenTwoContainer(selected_item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))  
    self.__need_action_update = true
    self.__parent_node:SetTouchEnabled(false)
    self.__touch_controller:ActionLocatePos(self.__screen_center_point.x - target_point.x, self.__screen_center_point.y - target_point.y, 0.8, function ()
        self.__need_action_update = false

        --选择下一个
        selected_item:SetSelected(true)

        self.__selected_item_index = self.__chapter_index
        
        --记录章节打开记录
        CallModuleFunc(ModuleType.DUNGEON,"SetOpenChapterIndex",Macros.Game.FBType.MAIN_DUNGEON,self.__chapter_index)
        
        self.__chapter_index = nil
        self.__parent_node:SetTouchEnabled(true)        
        
        --选择条目过渡效果完成回调
        if self.__select_item_effect_end_callback then
            self.__select_item_effect_end_callback()
        end
        if fun then
            fun()
        end
    end)
end

--设置选择条目过渡效果完成回调
function GUI.SpecialScrollView:SetSelectItemEffectEndCallback(callback)
    self.__select_item_effect_end_callback = callback
end

--设置非无条件选中同一个条目
function GUI.SpecialScrollView:SetIsNotUnconditionalSelected(is_var)
    self.__is_not_unconditional_selected = is_var
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--列表滚动回调
function GUI.SpecialScrollView:__ScrollCallBack(force)
    GUI.ScrollView.__ScrollCallBack(self, true)

    self:__MyScrollCallBack()
end

function GUI.SpecialScrollView:__MyScrollCallBack()
    if not self.__screen_center_point then
        return
    end

    if self.__horizon then
        local __scrollX = self:GetScrollX()
        local IsMoveFront = __scrollX >=0   
        local indexMin
        local indexMax
        if IsMoveFront then
            indexMin = 1
            indexMax = math.ceil((self.__view_width - __scrollX)/self.__item_width) + 1
        else
            indexMin = math.floor( - __scrollX/self.__item_width)
            indexMax = math.ceil((self.__view_width - __scrollX)/self.__item_width) + 1
        end

        local item
        for i = indexMin, indexMax do
            item = self:GetItem(i)
            if item then
                local target_point = NodeUnity.ConvertPosBetweenTwoContainer(item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))
                
                local scroll = self.__max_factor - self.__scale_factor*math.min(1.5,(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2)))
                item:SetScaleFactor(scroll)                         
                
                self:Excursion( item , self.__item_width )

                item:SetZOrder(9 - 2*math.min(1.5,(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2))))   

                if self.__is_turn_gray then
                    local widget = item:GetChangeWidget()
                    local colorNum = math.min(1,1.2 *(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2))) 
                    widget:SetOpacity(255 * colorNum)
                end
            end         
        end
    else
        --TODO：处理垂直列表
    end 

    if self.__chapter_index then
        local before_selected_item = self:GetItem(self.__chapter_index)
        local img_normal = before_selected_item:GetImgNormal()
        local target_point = NodeUnity.ConvertPosBetweenTwoContainer(before_selected_item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_c)/2))  
        local colorNum = math.min(1,(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2)))
        img_normal:SetColor(unpack({255 - 211*colorNum,255 - 211*colorNum,255 - 211*colorNum})) 
    end
end

function GUI.SpecialScrollView:__SetTouchSize()
    self.__touchsize_maxnum = self.__touchsize_maxnum or 5
    local max_num = self.__touchsize_maxnum
    local org_view_data_num = self.__view_data_num

    self.__view_data_num = math.max(max_num, org_view_data_num)
    GUI.ScrollView.__SetTouchSize(self) 

    self.__view_data_num = org_view_data_num

    self.__touch_controller:AdjustForSpecialScrollView(self.__horizon)
end

function GUI.SpecialScrollView:__SetTouchUpCallback()
    self.__parent_node:SetTouchEnabled(false)
    if self.__horizon then      
        local function touchUpCallBack()
            local scrollIndex
            local __scrollX = self:GetScrollX()
            if  __scrollX >= 0 then
                scrollIndex = (math.floor(self.__touchsize_maxnum/2) + 1) - math.floor(__scrollX/self.__item_width)
            else
                scrollIndex = (math.floor(self.__touchsize_maxnum/2) + 1) - math.ceil(__scrollX/self.__item_width)
            end

            local select_changed = self:__IsSelectedChanged(scrollIndex)

            if select_changed then
                local last_selected_item = self:GetItem(self.__selected_item_index) 
                if last_selected_item then
                    --反选上一个
                    last_selected_item:SetSelected(false)
                end

                --选择下一个
                local selected_item = self:GetItem(scrollIndex, true)
                if selected_item then
                    selected_item:SetSelected(true)
                end
                              
                self.__selected_item_index = scrollIndex
                if self.__is_need_callback then
                    if self.__select_callback then
                        local data
                        if selected_item then
                            data = selected_item:GetData()
                        end
                        self.__select_callback(self.__selected_item_index, data)
                    end
                end 
            end
            self.__parent_node:SetTouchEnabled(true)      
            --选择条目过渡效果完成回调
            if self.__select_item_effect_end_callback then
                self.__select_item_effect_end_callback()
            end
        end
        local __scrollX = self:GetScrollX()     
        local leaveBehind = math.abs(__scrollX)%self.__item_width           
        if  __scrollX >= 0 then
            if leaveBehind >= self.__item_width/2 then
                self.__touch_controller:ActionLocatePos(self.__item_width - leaveBehind, 0, 0.3 * (self.__item_width - leaveBehind)/self.__item_width, touchUpCallBack)
            else
                --print("ldx","__scrollX__scrollX: ",__scrollX,leaveBehind,- leaveBehind)
                self.__touch_controller:ActionLocatePos(-leaveBehind, 0, 0.3 * leaveBehind/self.__item_width, touchUpCallBack)          
            end
        else
            if leaveBehind >= self.__item_width/2 then
                self.__touch_controller:ActionLocatePos( - self.__item_width + leaveBehind, 0, 0.3 * (self.__item_width - leaveBehind)/self.__item_width, touchUpCallBack)
            else
                self.__touch_controller:ActionLocatePos( leaveBehind, 0, 0.3 * leaveBehind/self.__item_width, touchUpCallBack)          
            end
        end             
    else

    end 
end

function GUI.SpecialScrollView:__SetFixPosCallback(scroll_x,off)
    --边界
    local border_limmitx = self.__item_width * math.floor(self.__touchsize_maxnum/2)
    --回弹偏移
    local bounce_x = 30 
    if off >= 0 then
        if scroll_x > (border_limmitx + bounce_x) then
            return (border_limmitx + bounce_x)
        end
    else        
        if scroll_x < (border_limmitx - (self.__lock_chapter_num -1)*self.__item_width - bounce_x) then
            return (border_limmitx - (self.__lock_chapter_num -1)*self.__item_width - bounce_x)
        end
    end 
    return scroll_x
end

--返回选择结果
function GUI.SpecialScrollView:__IsSelectedChanged(index)
    if self.__is_not_unconditional_selected then
        return GUI.ScrollView.__IsSelectedChanged(self,index)
    else
        return true
    end 
end

--初始化条目状态
function GUI.SpecialScrollView:__InitScrollViewItemStatus()
    local item
    for _, item in pairs(self.__item_map) do
        local target_point = NodeUnity.ConvertPosBetweenTwoContainer(item:GetNode(), self.__scissor_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))
        item:SetScaleFactor(self.__max_factor - self.__scale_factor*math.min(1.5,(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2))))                    
        item:SetZOrder(9 - 2*math.min(1.5,(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2))))       

        if self.__is_turn_gray then
            local widget = item:GetChangeWidget()
            local colorNum = math.min(1,1.2 *(math.abs(self.__screen_center_point.x - target_point.x)/(self.__item_width - self.__item_space_c/2)))     
            widget:SetOpacity(255 * colorNum)
        end
    end
end

--偏移
function GUI.SpecialScrollView:Excursion( item , w )
    if not item.GetExcursionPos or not item.GetExcursionNode then
        return
    end

    local item_pos = NodeUnity.ConvertPosBetweenTwoContainer(item:GetNode(), self.__parent_node:GetNode(), COCOPoint((self.__item_width - self.__item_space_c)/2, (self.__item_height - self.__item_space_r)/2))

    --条目内要这个东西拿需要偏移的容器
    -- GetExcursionNode
    local max_offset_y = item:GetExcursionPos()
    
    --条目内要这个东西拿需要的偏移量
    -- GetExcursionPos
    local base_offset_y = item:GetMinPos()
    local offset_y = base_offset_y + (max_offset_y - math.abs(item_pos.x / w * max_offset_y))
    
    local node = item:GetExcursionNode()
    node:SetPosition(node:GetPositionX(), offset_y + self.__item_height / 2)
end
