LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroTrainView = LogicGUI.TemplHeroTrainView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroTrainView:__init(parent)
	self:InitTemplate(GUI.TemplateRegistry.HERO_TRAIN_VIEW,parent,true)
end