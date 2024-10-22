
--[[
%% @module: 通用打折
--]]

LogicGUI = LogicGUI or {}

LogicGUI.LconDiscount = LogicGUI.LconDiscount or BaseClass(UILayer)

function LogicGUI.LconDiscount:__init( index,str )

  local imgdata = nil
  local color = nil
  if math.floor(index) > 6 then
      imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_shopTips_blue")
      color = Macros.Color.blue_discount
  else
      imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_shopTips_red")
      color = Macros.Color.red_discount
  end

  local tips = Util:Sprite( imgdata )
  tips:SetAnchorPoint( 0.5 , 0.5 )
  self:AddChild(tips)
  PosTool.Center( tips )

  local text = Util:Label( str and str or CPPGameLib.GetString("common_break" , index ) , Macros.TypefaceSize.normal , nil , nil , nil , color )
  text:SetAnchorPoint( 0.5 , 0.5 )
  tips:AddChild(text)
  PosTool.Center( text )

  CPPActionManager.RotateTo(text,0, -30 )
end

function LogicGUI.LconDiscount:__delete()

end

function LogicGUI.LconDiscount:SetData(  )

end
