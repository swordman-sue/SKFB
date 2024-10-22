
--[[
%% @module: 战斗对象
%% @author: swordman sue
%% @created: 2016-07-18
--]]

Scene = Scene or {}

Scene.BattleObj = Scene.BattleObj or BaseClass(Scene.Obj)

local HPBarDefPos = COCOPoint(0, 230)

function Scene.BattleObj:__init()
    --身体动画信息
    self.__body_anim_play_info = {}
    self.__body_anim_play_info.anim_type = self.__vo.anim_type
    self.__body_anim_play_info.inner_layer = RenderObj.InnerLayer.BODY
    self.__body_anim_play_info.path = Resource.PathTool.GetModelPath(self.__vo.model_type, self.__vo.model_id)

    --用于BUFF飘字
    self.__buff_name_list = {}
    self.__in_buff_effect = nil
end

function Scene.BattleObj:__delete()
	self.__body_anim_play_info = nil
    self.__sp_ai_say_bg = nil

    GlobalEffectMgr:DelEffectsByTargetID(self.__vo.obj_id)        

    --移除待攻击近战碰撞对记录
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DelToAttackPNObjsRecord", self.__vo.obj_id, true)
end

function Scene.BattleObj:Update(now_time, elapse_time)
    if not self.__vo then
        return
    end
    
    Scene.Obj.Update(self, now_time, elapse_time)

    self.__s_beattacked_state.toggle = false

    self:__CorrectDirection()
end

function Scene.BattleObj:GetBodyAnimPlayInfo()
    return self.__body_anim_play_info
end

function Scene.BattleObj:SetLogicPos(posx, posy, update_to_model)
    local tmp_pos = Vector2D.New(posx, posy)
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", tmp_pos, nil, self.__vo.obj_id)  
    Scene.Obj.SetLogicPos(self, tmp_pos.x, tmp_pos.y, update_to_model)
end

function Scene.BattleObj:SetWorldPos(posx, posy, update_to_model)
    local tmp_pos = Vector2D.New(posx, posy)
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", tmp_pos, true, self.__vo.obj_id)  
    Scene.Obj.SetWorldPos(self, tmp_pos.x, tmp_pos.y, update_to_model)
end

function Scene.BattleObj:SetDir(var)
    if Scene.Obj.SetDir(self, var) then
        self.__model:SetDir(self.__dir, self.__body_anim_play_info.inner_layer)        
    end
end

--设置模型缩放比例
function Scene.BattleObj:SetScaleFactor(scale)
    Scene.Obj.SetScaleFactor(self, scale)

    if self.__sp_shadow then
        self.__sp_shadow:SetScaleFactor(scale)
    end
end

function Scene.BattleObj:SetVisible(var, force)
    if self.__vo.is_hide_mode and not force then
        return
    end
    
    Scene.Obj.SetVisible(self, var)

    if self.__sp_shadow then
        self.__sp_shadow:SetVisible(var)
    end

    GlobalEffectMgr:SetEffectsVisibleByTargetID(self.__vo.obj_id, var)
end

function Scene.BattleObj:SetColor(r, g, b)
    self.__model:SetColor(r, g, b, self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:SetHighLight(var)
    self.__model:SetHighLight(var, self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:SetGray(var)
    self.__model:SetGray(var, self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:SetOpacity(var)
    self.__model:SetOpacity(var, self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:PlayAnimation(act_index, loop, time_scale)
    if time_scale == nil then
        time_scale = self:TopAnimTimeScale()
    end

    --身体
    self:__SetupAnimPlayInfo(self.__body_anim_play_info, act_index, loop, time_scale)
    self.__model:PlayAnimation(self.__body_anim_play_info)

    --其他
    --....
end

function Scene.BattleObj:SetAnimTimeScale(scale)
    self.__model:SetAnimTimeScale(scale, self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:GetAnimTimeScale()
    self.__model:GetAnimTimeScale(self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:PauseAnimation()
    self.__model:PauseAnimation(self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:ResumeAnimation()
    self.__model:ResumeAnimation(self.__body_anim_play_info.inner_layer)
end

function Scene.BattleObj:PauseActions()
    Scene.Obj.PauseActions(self)

    if self.__sp_ai_say_bg then
        CPPActionManager.PauseAction(self.__sp_ai_say_bg)
    end
end

function Scene.BattleObj:ResumeActions()
    Scene.Obj.ResumeActions(self)

    if self.__sp_ai_say_bg then
        CPPActionManager.ResumeAction(self.__sp_ai_say_bg)
    end
end

function Scene.BattleObj:ResumeExpand()
    if self:IsInState("attack") then
        self.__s_attack_state.sp_skill_logic:ResumeExpand()
    end
end

--设置属性
function Scene.BattleObj:SetAttr(type, val)
    if type == Macros.Game.BattleProp.HP then
        self.__vo.max_hp = val
    else
        self.__vo.battle_attr:SetAttr(type, val)
    end
end

--设置属性百分比
function Scene.BattleObj:SetAttrByFactor(type, factor)
    local org_attr = self:GetAttr(type)
    self:SetAttr(type, org_attr * factor)
end

--获取属性
function Scene.BattleObj:GetAttr(type, show_error)
    if type == Macros.Game.BattleProp.HP then
        return self.__vo.max_hp
    else
        return self.__vo.battle_attr:GetAttr(type, show_error)
    end
end

--设置血量
function Scene.BattleObj:SetHp(hp)
    if self:IsDead() then
        return
    end

    Scene.Obj.SetHp(self, hp)
end

--飘血特效
function Scene.BattleObj:ShowHpEffect(change_hp, damage_type)
    if not self:IsVisible() then
        return
    end

    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

    local hp_effect, hp_effect_follow_id
    local offset_y = 110

    --创建飘血特效
    if damage_type == Modules.BattleFormula.DamageType.DODGE then
        --闪避伤害：没有伤害数字，直接显示“闪避”字样图片
        hp_effect = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "battle_dodge", true))
        hp_effect:SetAnchorPoint(0.5, 0)
        GlobalScene:AddToRenderGroup(hp_effect, Scene.RenderQueue.GRQ_AIR_TOP)

        --绑定飘血特效
        hp_effect_follow_id = self.__model:BindFollowNode(hp_effect, RenderObj.FollowNodeType.NODE, Scene.RenderQueue.GRQ_AIR_TOP, RenderObj.FollowNodePosUpdateType.UPDATE_X, 0, offset_y)

    else
        if change_hp == 0 then
            return
        end
        
        local img_font_type = change_hp > 0 and GUI.ImgFont.ImgType.Num3 or GUI.ImgFont.ImgType.Num2

        local prefix_img_name
        local hp_img_name = change_hp > 0 and "battle_treat_num" or "battle_damage_num"

        if damage_type == Modules.BattleFormula.DamageType.NORMAL then
            --普通伤害：橙色数字
            --普通回复：绿色数字
            
        else
            --格挡伤害：蓝色伤害数字，数字前加“格挡”字样图片
            if damage_type == Modules.BattleFormula.DamageType.BLOCK then
                prefix_img_name = "battle_block"

            --暴击伤害：放大的红色伤害数字，数字前加红色“暴击”字样图片
            --暴击回复：放大的绿色数字，数字前加绿色“暴击”字样图片
            else
                img_font_type = GUI.ImgFont.ImgType.Num3
                prefix_img_name = change_hp > 0 and "battle_crit2" or "battle_crit"
                hp_img_name = change_hp > 0 and "battle_treat_num" or "battle_crit_num"
            end
        end

        offset_y = change_hp > 0 and 110 or 160

        local abs_change_hp = math.abs(change_hp)
        hp_effect = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, hp_img_name, true), img_font_type)
        hp_effect:SetAnchorPoint(0.5, 0)
        if prefix_img_name then
            hp_effect:SetPrefixEnabled(true, Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, prefix_img_name, true))
        end
        hp_effect:SetText(change_hp > 0 and "+"..abs_change_hp or "-"..abs_change_hp)
        GlobalScene:AddToRenderGroup(hp_effect, Scene.RenderQueue.GRQ_AIR_TOP)
    
        --绑定飘血特效
        hp_effect_follow_id = self.__model:BindFollowNode(hp_effect, RenderObj.FollowNodeType.OBJ, Scene.RenderQueue.GRQ_AIR_TOP, RenderObj.FollowNodePosUpdateType.UPDATE_X, 0, offset_y)
    end

    --处理飘血效果
    local hp_effect_node = self.__model:GetFollowNode(hp_effect_follow_id)
    ActionManager:GetInstance():BattleHPEffect(hp_effect_node, function()
        --解绑飘血特效    
        self.__model:UnbindFollowNode(hp_effect_follow_id)
    end)
end

--AI喊话特效
function Scene.BattleObj:ShowAISayEffect(content, last_time)
    if not content then
        return
    end

    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

    if self.__sp_ai_say_bg then
        self.__sp_ai_say_bg:RemoveSelf(true)
    end
    self.__sp_ai_say_bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "ai_say_bg"))
    self.__sp_ai_say_bg:SetPosition(0, 200)
    self.__sp_ai_say_bg:SetAnchorPoint(self.__dir == MathTool.HORZ_RIGHT and 0 or 1, 0)
    self.__sp_ai_say_bg:SetFlipX(self.__dir == MathTool.HORZ_RIGHT)
    self.__model:AddChild(self.__sp_ai_say_bg, RenderObj.InnerLayer.ALL_KIND_OF_TIPS)

    local lab_title = Util:RichText(content, Macros.TypefaceSize.headlineII, 270, nil, nil, Macros.Color.lilyBlack_hex, 4)             
    lab_title:SetAnchorPoint(0.5, 0.5)
    self.__sp_ai_say_bg:AddChild(lab_title)
    PosTool.Center(lab_title, 0, 0)

    ActionManager.GetInstance():AISayEffect(self.__sp_ai_say_bg, last_time, self.__dir, function()
        self.__sp_ai_say_bg = nil
    end)
end

--技能BUFF飘字
function Scene.BattleObj:ShowBuffNameEffect(buff_name, say_info)
    if not buff_name then return end

    if not self:IsVisible() then
        return
    end    

    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

    table.insert( self.__buff_name_list , buff_name )

    local function Piece( ... )
        self.__in_buff_effect = true

        local color_title = Macros.GradientColor( self.__buff_name_list[1][2] )
        local lab_title = Util:RichText( string.format(color_title, self.__buff_name_list[1][1] ) , Macros.TypefaceSize.headlineII )             
        lab_title:SetAnchorPoint(0.5, 0.5)
        self.__model:AddChild(lab_title , 100 )
        PosTool.Center(lab_title, 0 , 200 )

        ActionManager.GetInstance():BattleSkillNameEffect( lab_title , function ( ... )
            lab_title:RemoveSelf(true)
            lab_title = nil
        end)

        table.remove( self.__buff_name_list , 1 )

        --假装是个定时器
        CPPActionManager.DelayTo( lab_title , 0.7 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
            if self.__buff_name_list[1] then
                Piece()
            end
            self.__in_buff_effect = nil
        end ) )

    end

    if not self.__in_buff_effect then
        Piece()
    end
end

--QTE圆圈特效
function Scene.BattleObj:ShowQTEEffect()
    --默认放大系数
    local def_scale_factor = 1.2  
    --参数列表
    local pa_list = ConfigAdapter.Common.GetQTEGradeParameter()
    --外圈大小
    local out_circle_scale_factor = def_scale_factor * pa_list[1]
    --目标大小
    local target_scale = def_scale_factor * (1 - MathTool.TransProbCoeffPercentage(pa_list[4]))

    local effect
    --QTE圆圈特效(TODO QTE圆圈特效)
    for i=1,2 do
        self["__qte_match_effect_id"..i],effect = GlobalEffectMgr:CreateSceneTargetEffect({id = 100104 + i,loop = true,scale = i == 1 and def_scale_factor or out_circle_scale_factor}, self.__vo.obj_id, RenderObj.InnerLayer.RIDER_DOWN)  
        PosTool.Center(effect)  
        effect:setRotation3D(COCOPoint3D(-60,0,0))
    end
    local func
    local is_zoomin
    local zoom = function ()   
        is_zoomin = not is_zoomin    
        if is_zoomin then
            CPPActionManager.ScaleTo(effect, pa_list[2], target_scale, func) 
        else
            --玩家不点击，无评分
            self:ShowQTEGradeEffect(true)
        end     
    end
    func = GlobalCallbackMgr:AddCallBackFunc(zoom) 
    zoom()
    self.__is_touch_qte_grade = false
    self.__is_in_turning_qte_camera = false
end

--QTE评分特效
function Scene.BattleObj:ShowQTEGradeEffect(is_invalid)
    if self.__is_touch_qte_grade then
        return
    end
    self.__is_touch_qte_grade = true
    --内圈
    local in_circle_effect = GlobalEffectMgr:GetEffect(self.__qte_match_effect_id1)
    --先停外圈特效
    local out_circle_effect = GlobalEffectMgr:GetEffect(self.__qte_match_effect_id2)
    if out_circle_effect then
        CPPActionManager.StopAllActions(out_circle_effect)
    end
    local ai,grade_info
    --处理评分
    local handle_grade = function ()
        --无评分
        if is_invalid or not grade_info then
            --重置连击数
            GlobalVOMgr:ResetQTEComboHitTimes()
            if ai then
                ai:TriggerAngerOrJointQTE()
            end
        else
            --伤害特效
            -- local effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({res_name = grade_info.harm_effect,loop = false}, self.__vo.obj_id, RenderObj.InnerLayer.HP_EFFECT)  
            -- PosTool.CenterBottom(effect,0,200)                     
            --技能
            local skill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "battle_skillpower", true)
            local skill_front = Util:Sprite(skill_sp)
            skill_front:SetVisible(false)
            GlobalScene:AddToRenderGroup(skill_front, Scene.RenderQueue.GRQ_BUILD_AND_PLAYER)
            --转坐标
            local new_container_node = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_BUILD_AND_PLAYER)
            local new_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__model:GetNode(), new_container_node:GetNode(), COCOPoint(365,305))
            skill_front:SetPosition(new_pos.x,new_pos.y)
            --伤害系数
            local harm_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "battle_qte_combo_"..grade_info.harm_percent, true)
            local harm_front = Util:Sprite(harm_sp)
            harm_front:SetVisible(false)
            GlobalScene:AddToRenderGroup(harm_front, Scene.RenderQueue.GRQ_BUILD_AND_PLAYER)
            new_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__model:GetNode(), new_container_node:GetNode(), COCOPoint(370,220))
            harm_front:SetPosition(new_pos.x,new_pos.y)

            ActionManager.GetInstance():BattleQTEHarmEffect(skill_front,harm_front)

            if self.__change_status_delay_timer then
                GlobalTimerQuest:CancelQuest(self.__change_status_delay_timer)
                self.__change_status_delay_timer = nil
            end

            self.__change_status_delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
                function ()
                    --退出QTE状态
                    if ai then
                        ai:TriggerAngerOrJointQTE()
                    end
                end),1.5)           
        end

        --QTE圆圈特效(TODO QTE圆圈特效)
        for i=1,2 do
            if self["__qte_match_effect_id"..i] then
                GlobalEffectMgr:DelEffect(self["__qte_match_effect_id"..i])
            end
        end
    end
    --ai
    ai = GlobalAIMgr:GetAI(self.__ai_key)
    grade_info = ConfigAdapter.Battle.GetBattleQTEGradeInfo(not is_invalid and out_circle_effect and out_circle_effect:GetScaleFactor() or 0,in_circle_effect and in_circle_effect:GetScaleFactor() or 1)
    if grade_info then
        --评分特效     
        local effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({id = grade_info.grade_effectid,loop = false,scale = 2}, self.__vo.obj_id, RenderObj.InnerLayer.ALL_KIND_OF_TIPS)  
        PosTool.CenterBottom(effect,0,0)
        --评分文字
        local grade_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "battle_qte_grade_type_"..grade_info.grade_type, true)
        local grade_front = Util:Sprite(grade_sp)
        self.__model:AddChild(grade_front, RenderObj.InnerLayer.HP_EFFECT, RenderObj.InnerLayer.HP_EFFECT)
        grade_front:SetPosition(0,250)
        ActionManager.GetInstance():BattleQTEGradeEffect(grade_front)      
    end

    if self.__qte_pause_timer then
        GlobalTimerQuest:CancelQuest(self.__qte_pause_timer)
        self.__qte_pause_timer = nil
    end
    --触发QTE后的暂停时长
    local qte_pause_time = ConfigAdapter.Common.GetQTEPauseTime() or 1
    self.__qte_pause_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
        function ()
            handle_grade()
        end),qte_pause_time)
    --光聚特效
    local effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({res_name = "nuqi_qishou2", loop = false, scale = 1/self:GetScaleFactor(), time_scale = 1}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_DOWN)  
    PosTool.Center(effect, 0, 60)

    return true
end

--能否累计怒气
function Scene.BattleObj:IsAngerAdditive()
    return false
end

--怒气处理
function Scene.BattleObj:DoAngerChange(change_anger)    
end

--每场战役开启时的技能重置
function Scene.BattleObj:ResetAISkillForBattleStart()
    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        ai:ResetSkillForBattleStart()
    end
end

--怒气是否充足
function Scene.BattleObj:IsAngerEnough(is_plus)
end

--设置怒气/合击技能释放开关
function Scene.BattleObj:TriggerAngerOrJointSkill(is_plus)
    if not self:IsAngerEnough(is_plus) then
        return
    end

    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        return ai:TriggerAngerOrJointSkill(is_plus)
    end
end

--重置怒气/合击技能释放开关
function Scene.BattleObj:ShutdownAngerOrJointSkill(is_plus)
    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        ai:ShutdownAngerOrJointSkill(is_plus)
    end
end

--检测怒气/合击技能开关
function Scene.BattleObj:IsAngerOrJointSkillTriggered()
    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        return ai:IsAngerOrJointSkillTriggered()
    end
end

--设置自动战斗(自动释放怒气/合击)
function Scene.BattleObj:SetAutoFight(var)
    self.__auto_fight = var

    if not self.__auto_fight then
        self:ShutdownAngerOrJointSkill()
        self:ShutdownAngerOrJointSkill(true)
    end
end

--是否自动战斗
function Scene.BattleObj:IsAutoFight()
    return self.__auto_fight 
end

--设置出生状态
function Scene.BattleObj:SetInBirth(var)
    self.__in_birth = var
end

--获取出生状态
function Scene.BattleObj:GetInBirth()
    return self.__in_birth
end

--战斗翻倍提示效果
function Scene.BattleObj:ShowDoubleEffect(...)
end

--设置是否进入镜头
function Scene.BattleObj:SetEnteredBattleRegion(var)
    self.__entered_camera = var
end

--获取是否进入镜头
function Scene.BattleObj:GetEnteredBattleRegion()
    return self.__entered_camera
end

--获取模型高度
function Scene.BattleObj:GetModelHeight()
    return self.__model:GetModelHeight(self.__body_anim_play_info.inner_layer)
end

--改变血条位置
function Scene.BattleObj:ChangeHpBarPosition(delta_x, delta_y)
    if self.__hp_bar then
        self.__hp_bar:SetPosition(HPBarDefPos.x + delta_x, HPBarDefPos.y + delta_y)
    end
end

--是否在转换QTE镜头
function Scene.BattleObj:IsInTurningQTECamera()
    return self.__is_in_turning_qte_camera
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:__LoadVO()
    Scene.Obj.__LoadVO(self)
end

--添加脚底烟尘特效
function Scene.BattleObj:__AddFogEffect(effect_config_id, flip_dir)
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

    local obj_dir = flip_dir and -self:GetDir() or self:GetDir()
    local fog_effect_id = GlobalEffectMgr:CreateScenePosEffect({id = effect_config_id, loop = true, dir = obj_dir}, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    local fog_follow_id = self.__model:BindFollowNode(fog_effect_id, RenderObj.FollowNodeType.EFFECT_ID, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    return fog_follow_id
end

--移除脚底烟尘特效
function Scene.BattleObj:__DelFogEffect(fog_follow_id)
    self.__model:UnbindFollowNode(fog_follow_id)
end

--纠正方向
function Scene.BattleObj:__CorrectDirection()
    if self:IsInState("sp_move") or self:GetInBirth() then
        return
    end

    if self:IsHero() or self:IsHeroPVP() or
        self:IsMonster() or self:IsMonsterPVP() then
        if self.__vo.dir ~= self.__dir then
            self:SetDir(self.__vo.dir)
        end
    end
end

--播放精灵祝福特效
function Scene.BattleObj:__PlaySpriteBlessEffect()
    --精灵祝福特效
    if self.__vo.bless_sprite_info and not self.__sp_sprite_half_head_name then  
        --虚影名字
        local res_name = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "sprite_bless_name_"..self.__vo.bless_sprite_info.id, true)
        self.__sp_sprite_half_head_name = Util:Sprite(res_name)    
        self.__sp_sprite_half_head_name:SetScaleFactor(self.__vo.model_scale_inver)   
        self:ChangeSpriteBlessHalfHeadPosition(0, 0)
        self.__model:AddChild(self.__sp_sprite_half_head_name, RenderObj.InnerLayer.HP_BAR, RenderObj.InnerLayer.HP_BAR)
        --虚影
        res_name = Resource.PathTool.GetHalfHeadPath(self.__vo.bless_sprite_info.id)
        local sp_sprite_half_head = Util:Sprite(res_name)
        sp_sprite_half_head:SetScaleFactor(0.3)        
        sp_sprite_half_head:SetAnchorPoint(0.5,0)
        self.__sp_sprite_half_head_name:AddChild(sp_sprite_half_head,-1)
        PosTool.CenterBottom(sp_sprite_half_head)
        --延迟播放  
        CPPActionManager.DelayTo(self.__sp_sprite_half_head_name,1,GlobalCallbackMgr:AddCallBackFunc(function ()
            CPPActionManager.FadeOut(self.__sp_sprite_half_head_name,1.5)
        end))    
        local effect_id,effect
        --精灵下特效
        effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({res_name = "UI_zhandou_jinglingchuchangzhufu",loop = false,time_scale = 0.5,scale = self.__vo.model_scale_inver}, self.__vo.obj_id, RenderObj.InnerLayer.SHADOW)  
        PosTool.Center(effect,0,100) 
        --精灵上特效
        effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({res_name = "UI_zhandou_jinglingchuchangzhufu1",loop = false,time_scale = 0.5,scale = self.__vo.model_scale_inver}, self.__vo.obj_id,RenderObj.InnerLayer.BODY_EFFECT_UP)  
        PosTool.Center(effect,0,100)   
        --延迟播放  
        CPPActionManager.DelayTo(sp_sprite_half_head,1.5,GlobalCallbackMgr:AddCallBackFunc(function ()
            local spirit_config = CPPGameLib.GetConfig("Spirit",self.__vo.bless_sprite_info.id,false)
            --精灵下特效
            if spirit_config and spirit_config.down_bless_effect_id and not self.__down_bless_spirit_effectid then
                self.__down_bless_spirit_effectid,effect = GlobalEffectMgr:CreateSceneTargetEffect({id = spirit_config.down_bless_effect_id,loop = true}, self.__vo.obj_id, RenderObj.InnerLayer.SHADOW)  
                PosTool.CenterBottom(effect) 
            end
            --精灵上特效
            if spirit_config and spirit_config.bless_effect_id and not self.__bless_spirit_effectid then
                self.__bless_spirit_effectid,effect = GlobalEffectMgr:CreateSceneTargetEffect({id = spirit_config.bless_effect_id,loop = true}, self.__vo.obj_id,RenderObj.InnerLayer.BODY_EFFECT_UP)  
                PosTool.CenterBottom(effect)   
            end
        end))           
    end 
end

--播放精灵祝福特效
function Scene.BattleObj:__ShowSpriteBlessHalfHead()
    if self.__sp_sprite_half_head_name then
        self.__sp_sprite_half_head_name:SetOpacity(255)
        CPPActionManager.DelayTo(self.__sp_sprite_half_head_name,0.5,GlobalCallbackMgr:AddCallBackFunc(function ()
            CPPActionManager.FadeOut(self.__sp_sprite_half_head_name,1.5)
        end)) 
    end
end
