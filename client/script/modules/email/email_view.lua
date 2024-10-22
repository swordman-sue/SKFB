--[[
%% @module: 商店界面
%% @author: yj
%% @created: 2016-08-24
--]]


Modules = Modules or {} 


Modules.EmailView = Modules.EmailView or BaseClass(BaseView)


function Modules.EmailView:__init()
    self.__layout_name = "email"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
      self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EMAIL_SERVER_CELL)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EMAIL)    
end

function Modules.EmailView:__OpenCallback( )
   --标题
   Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("email_title"),Macros.TypefaceSize.popup,Macros.Color.btn ) 
   --关闭按钮  
   Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
        self:CloseManual()
   end)
   --关闭界面
  self:BindGlobalEvent(Macros.Event.EmailModule.CLOSE_EMAIL_VIEW, function()
      self:CloseManual()
  end)
   --邮件容器
   local Panel_ScrollView = self:GetWidget("Panel_ScrollView")
   local params = {
      item_class = Modules.TemplEmailCell,
      item_width = 569,
      item_height = 122,
      row = 3,
      col = 1,
      item_space_r = 10,
      view_width = 569,
      view_height = 372,
    }
    self.__email_scrollview = GUI.ScrollView.New(params)
    Panel_ScrollView:AddChild(self.__email_scrollview:GetNode())
    PosTool.Center(self.__email_scrollview)
    local email_list = self:CallModuleFunc("GetEmailList")
    self.__email_scrollview:SetDataList(email_list)
end 

function Modules.EmailView:AttemData(data)
    self.__email_scrollview:AttachItem(data,1,true)  
end

function Modules.EmailView:__Dispose()   
    if self.__email_scrollview then
        self.__email_scrollview:DeleteMe()
        self.__email_scrollview = nil
    end
end