--
-- @brief 聊天界面(表情)
-- @author: yjg
-- @date: 2018年12月25日16:15:23
--

Modules = Modules or {}

Modules.TemplChatFaceLayer = Modules.TemplChatFaceLayer or BaseClass(UILayer)

function Modules.TemplChatFaceLayer:__init(callback , node)
    self.FaceList = {}
    self.INDEX = 7
    self.callback = callback

    --九宫格
    local dialogueBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_pack_tips_2"))
    dialogueBG:SetStretchSize(337.00,145.00)
    dialogueBG:SetAnchorPoint(0,0)
    dialogueBG:SetPosition(234.16,177.13)
    dialogueBG:SetInnerRect(10/44,10/44,10/44,10/44)
    self:AddChild(dialogueBG)

    --设置列表
    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize(337.00,145.00)
    self.lst_list:SetPosition(234.16,177.13)
    self.lst_list:SetAnchorPoint(0,0)
    self:AddChild(self.lst_list)

    --创建表情
    for i = 1 , 20 do
        table.insert(self.FaceList,string.format("x_%02d", i))
    end

    self:AddCell()
end

function Modules.TemplChatFaceLayer:__delete()
end

function Modules.TemplChatFaceLayer:AddCell(data)
    local dice_data = {}
    for i,v in ipairs(self.FaceList) do
        --找到图集
        dice_data[i] = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT_EMOTION_ICON, v)
    end

    --行数
    local num = math.ceil(#dice_data / self.INDEX)

    for i=1,num do
        local layou = Util:Layout()
        layou:SetContentSize(337 ,50)
        layou:SetAnchorPoint(0.5,0.5)
        layou:SetTouchEnabled(true)
        local layou_sp = {}
        layou_sp.width = layou:GetContentSize().width
        layou_sp.height = layou:GetContentSize().height

        local x = 24
        for j=1,self.INDEX do
            if i * j <= #dice_data then
                local biaoQingKuang = Util:Layout()
                biaoQingKuang:SetContentSize(24,24)
                biaoQingKuang:SetAnchorPoint(0.5,0.5)
                biaoQingKuang:SetTouchEnabled(true)
                biaoQingKuang:SetPosition(x,layou_sp.height/2)
                x = x + 24

                local biaoQingKuang_sp = {}
                biaoQingKuang_sp.width = biaoQingKuang:GetContentSize().width
                biaoQingKuang_sp.height = biaoQingKuang:GetContentSize().height
                layou:AddChild(biaoQingKuang)
                WidgetUnity.SetWidgetListener(biaoQingKuang,nil,nil,function()
                    self.callback( string.format("x_%02d", (self.INDEX * (i-1) ) + j ) )
                end,nil)

                local img = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT_EMOTION_ICON, string.format("x_%02d", (self.INDEX * (i-1) ) + j ) )
                x = x + dice_data[1].width 
                local sp = SpriteEx.CreateWithImageFile( img )
                sp:SetPosition(biaoQingKuang_sp.width/2 , biaoQingKuang_sp.height/2)
                biaoQingKuang:AddChild(sp)
            end
        end

        self.lst_list:PushBackCustomItem(layou)
    end
end
