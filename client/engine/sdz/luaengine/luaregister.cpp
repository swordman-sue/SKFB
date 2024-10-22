//
//  luaregister.cpp
//
//  Created by swordman sue
//  Copyright (c) ZLGame All rights reserved.
//

#include "luaregister.h"

#include "httpmanager/httpstruct.h"
#include "httpmanager/httpmanager.h"
#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"
#include "zqcommon/gamedef.h"

#include "cocos2d.h"
#include "luaengine.h"
#include "luatinker/lua_tinker.h"

#include "luamessageadapter.h"
#include "luaprofile.h"
#include "resmanager/installer.h"
#include "gameengine/gameengine.h"
#include "resmanager/resmanager.h"

#include "platform/sdktools.h"
#include "platform/systemtools.h"
#include "location/location.h"
#include "msgpush/msgpush.h"

#include "sound/soundmanager.h"
#include "sound/voicemanager.h"

#include "filesystem/fileversion.h"

#include "timer/timerquest.h"
#include "statemachine/gamestate.h"
#include "statemachine/gamestatemachine.h"

#include "netadapter/netadapter.h"

#include "view/struct.h"
#include "view/nodeunity.h"
#include "view/viewhelper.h"
#include "view/gamescene.h"
#include "view/gamelayer.h"
#include "view/scenemanager.h"
#include "view/skeletondatacache.h"
#include "view/font/fontmanager.h"
#include "view/font/stringconv.h"
#include "view/render/batchrenderer.h"
#include "view/action/actionmanager.h"

#include "view/action/actiontimelineex.h"

#include "view/widget/labelex.h"
#include "view/widget/spriteex.h"
#include "view/widget/scale9spriteex.h"
#include "view/widget/imagebutton.h"
#include "view/widget/imagesprite.h"
#include "view/widget/richtext.h"
#include "view/widget/edittext.h"
#include "view/widget/edittextex.h"
#include "view/widget/graphiclabel.h"
#include "view/widget/progresscontrol.h"
#include "view/widget/progress9control.h"
#include "view/widget/graphicrender.h"
#include "view/widget/skeletonanimation.h"
#include "view/widget/progresstimer.h"
#include "view/widget/clippingnodeex.h"
#include "view/widget/particlesprite.h"

#include "ui/UIWidget.h"
#include "view/uiwidget/uilayoutex.h"
#include "view/uiwidget/uiscrollviewex.h"
#include "view/uiwidget/uipageviewex.h"
#include "view/uiwidget/uilistviewex.h"
#include "view/uiwidget/uitouchgroupex.h"
#include "view/uiwidget/uibuttonex.h"
#include "view/uiwidget/uitextex.h"
#include "view/uiwidget/uiimageviewex.h"
#include "view/uiwidget/uiloadingbarex.h"
#include "view/uiwidget/uitextfieldex.h"
#include "view/uiwidget/uicheckboxex.h"
#include "view/uiwidget/uitextatlasex.h"
#include "view/uiwidget/uisliderex.h"
#include "view/widgetunity.h"

#include "cocos-ext.h"

#include "math/Vec3.h"

typedef cocos2d::Vec3 COCOPoint3D;

using namespace lua_tinker;

void CreateGlobalTable(lua_State *ls, const char* table_name)
{
	lua_getglobal(ls, table_name);
	if (!lua_istable(ls, -1))
	{
		lua_pushstring(ls, table_name);
		lua_newtable(ls);
		lua_settable(ls, LUA_GLOBALSINDEX);
	}
	lua_pop(ls, 1);
}

void SetGlobalTalbeValue(lua_State *ls, const char* table_name, const char* table_key, const char* table_value)
{
	lua_getglobal(ls, table_name);
	if (!lua_istable(ls, -1))
	{
		lua_pop(ls, 1);
		return;
	}

	lua_pushstring(ls, table_key);
	lua_setfield(ls, -2, table_value);
	lua_pop(ls, 1);
}

template<typename T>
void class_instance2Ex(lua_State *ls, T* v)
{
	push_meta(ls, lua_tinker::space_name::name());
	if (lua_istable(ls, -1))
	{
		const char* s_class_name = lua_tinker::class_name<T>::name();
		lua_pushstring(ls, s_class_name);
		new(lua_newuserdata(ls, sizeof(val2user2<T>))) val2user2<T>(v);
		lua_pushstring(ls, s_class_name);
		lua_gettable(ls, LUA_GLOBALSINDEX);
		if (lua_istable(ls, -1))
		{
			lua_setmetatable(ls, -2);
			lua_rawset(ls, -3);
		}
	}

	lua_pop(ls, 1);
}

void LuaRegister::Regist(lua_State *ls)
{
	int stack_top = lua_gettop(ls);
	
	//COCOSize
	class_add<COCOSize>(ls, "COCOSize");
	class_con<COCOSize>(ls, lua_tinker::constructor<COCOSize>);
	class_con<COCOSize>(ls, lua_tinker::constructor<COCOSize, float, float>);
	class_mem<COCOSize>(ls, "width", &COCOSize::width);
	class_mem<COCOSize>(ls, "height", &COCOSize::height);
	class_def<COCOSize>(ls, "Equals", &COCOSize::equals);

	//COCOPoint
	class_add<COCOPoint>(ls, "COCOPoint");
	class_con<COCOPoint>(ls, lua_tinker::constructor<COCOPoint>);
	class_con<COCOPoint>(ls, lua_tinker::constructor<COCOPoint, float, float>);
	class_mem<COCOPoint>(ls, "x", &COCOPoint::x);
	class_mem<COCOPoint>(ls, "y", &COCOPoint::y);
	class_def<COCOPoint>(ls, "GetLength", &COCOPoint::getLength);
	class_def<COCOPoint>(ls, "Normalize", &COCOPoint::normalize);
	class_def<COCOPoint>(ls, "Equals", &COCOPoint::equals);

	//vec3
	class_add<COCOPoint3D>(ls, "COCOPoint3D");
	class_con<COCOPoint3D>(ls, lua_tinker::constructor<COCOPoint3D>);
	class_con<COCOPoint3D>(ls, lua_tinker::constructor<COCOPoint3D, float, float, float>);
	class_mem<COCOPoint3D>(ls, "x", &COCOPoint3D::x);
	class_mem<COCOPoint3D>(ls, "y", &COCOPoint3D::y);
	class_mem<COCOPoint3D>(ls, "z", &COCOPoint3D::z);

    //Rect
    class_add<cocos2d::Rect>(ls, "COCORect");
    class_con<cocos2d::Rect>(ls, lua_tinker::constructor<cocos2d::Rect>);
	class_def<cocos2d::Rect>(ls, "Equals", &cocos2d::Rect::equals);
    class_mem<cocos2d::Rect>(ls, "origin", &cocos2d::Rect::origin);
    class_mem<cocos2d::Rect>(ls, "size", &cocos2d::Rect::size);

	//Color3
	class_add<cocos2d::Color3B>(ls, "Color3");
	class_con<cocos2d::Color3B>(ls, lua_tinker::constructor<cocos2d::Color3B>);
	class_mem<cocos2d::Color3B>(ls, "r", &cocos2d::Color3B::r);
	class_mem<cocos2d::Color3B>(ls, "g", &cocos2d::Color3B::g);
	class_mem<cocos2d::Color3B>(ls, "b", &cocos2d::Color3B::b);

	//Node
	class_add<cocos2d::Node>(ls, "Node");
	class_def<cocos2d::Node>(ls, "getDescription", &cocos2d::Node::getDescription);
	class_def<cocos2d::Node>(ls, "SetPosition", (void (cocos2d::Node::*)(float, float))&cocos2d::Node::setPosition);
	class_def<cocos2d::Node>(ls, "SetPosition3D", &cocos2d::Node::setPosition3D);
	class_def<cocos2d::Node>(ls, "GetPosition", (const cocos2d::Vec2& (cocos2d::Node::*)(void) const)&cocos2d::Node::getPosition);
	class_def<cocos2d::Node>(ls, "GetPosition3D", (cocos2d::Vec3 (cocos2d::Node::*)(void) const)&cocos2d::Node::getPosition3D);
	class_def<cocos2d::Node>(ls, "GetPositionX", &cocos2d::Node::getPositionX);
	class_def<cocos2d::Node>(ls, "GetPositionY", &cocos2d::Node::getPositionY);
	class_def<cocos2d::Node>(ls, "SetAnchorPoint", (void (cocos2d::Node::*)(float, float))&cocos2d::Node::SetAnchorPoint);
	class_def<cocos2d::Node>(ls, "GetAnchorPoint", &cocos2d::Node::getAnchorPoint);
	class_def<cocos2d::Node>(ls, "SetContentWidth", &cocos2d::Node::setContentWidth);
	class_def<cocos2d::Node>(ls, "SetContentHeight", &cocos2d::Node::setContentHeight);
	class_def<cocos2d::Node>(ls, "GetContentWidth", &cocos2d::Node::getContentWidth);
	class_def<cocos2d::Node>(ls, "GetContentHeight", &cocos2d::Node::getContentHeight);
	class_def<cocos2d::Node>(ls, "SetContentSize", (void (cocos2d::Node::*)(const cocos2d::Size&))&cocos2d::Node::setContentSize);
	class_def<cocos2d::Node>(ls, "SetContentSize", (void (cocos2d::Node::*)(float, float))&cocos2d::Node::SetContentSize);
	class_def<cocos2d::Node>(ls, "GetContentSize", &cocos2d::Node::getContentSize);
	class_def<cocos2d::Node>(ls, "SetScaleFactor", (void (cocos2d::Node::*)(float))&cocos2d::Node::setScale);
	class_def<cocos2d::Node>(ls, "GetScaleFactor", &cocos2d::Node::getScale);
	class_def<cocos2d::Node>(ls, "SetScaleXFactor", &cocos2d::Node::setScaleX);
	class_def<cocos2d::Node>(ls, "SetScaleYFactor", &cocos2d::Node::setScaleY);
	class_def<cocos2d::Node>(ls, "GetScaleXFactor", &cocos2d::Node::getScaleX);
	class_def<cocos2d::Node>(ls, "GetScaleYFactor", &cocos2d::Node::getScaleY);
	class_def<cocos2d::Node>(ls, "GetRotation", &cocos2d::Node::getRotation);
	class_def<cocos2d::Node>(ls, "SetRotation", &cocos2d::Node::setRotation);
	class_def<cocos2d::Node>(ls, "setRotation3D", &cocos2d::Node::setRotation3D);
	class_def<cocos2d::Node>(ls, "SetVisible", &cocos2d::Node::setVisible);
	class_def<cocos2d::Node>(ls, "IsVisible", &cocos2d::Node::isVisible);
	class_def<cocos2d::Node>(ls, "SetZOrder", &cocos2d::Node::setLocalZOrder);
	class_def<cocos2d::Node>(ls, "GetZOrder", &cocos2d::Node::getLocalZOrder);
	class_def<cocos2d::Node>(ls, "SetTag", &cocos2d::Node::setTag);
	class_def<cocos2d::Node>(ls, "GetTag", &cocos2d::Node::getTag);
	class_def<cocos2d::Node>(ls, "GetParent", (cocos2d::Node*(cocos2d::Node::*)())&cocos2d::Node::getParent);
	class_def<cocos2d::Node>(ls, "AddChild", (void(cocos2d::Node::*)(cocos2d::Node*))&cocos2d::Node::addChild);
	class_def<cocos2d::Node>(ls, "AddChild", (void(cocos2d::Node::*)(cocos2d::Node*, int))&cocos2d::Node::addChild);
	class_def<cocos2d::Node>(ls, "AddChild", (void(cocos2d::Node::*)(cocos2d::Node*, int, int))&cocos2d::Node::addChild);
	class_def<cocos2d::Node>(ls, "RemoveChild", (void(cocos2d::Node::*)(cocos2d::Node*))&cocos2d::Node::removeChild);
	class_def<cocos2d::Node>(ls, "RemoveChild", (void(cocos2d::Node::*)(cocos2d::Node*, bool))&cocos2d::Node::removeChild);
	class_def<cocos2d::Node>(ls, "RemoveSelf", &cocos2d::Node::removeFromParentAndCleanup);
	class_def<cocos2d::Node>(ls, "RemoveAllChild", &cocos2d::Node::removeAllChildrenWithCleanup);
	class_def<cocos2d::Node>(ls, "GetChildCount", &cocos2d::Node::getChildrenCount);
	class_def<cocos2d::Node>(ls, "GetChildMaxZOrder", &cocos2d::Node::getChildMaxZOrder);
	class_def<cocos2d::Node>(ls, "GetChildByTag", &cocos2d::Node::GetChildByTag);
	class_def<cocos2d::Node>(ls, "SetIgnoreAnchorPoint", &cocos2d::CCNode::ignoreAnchorPointForPosition);
	class_def<cocos2d::Node>(ls, "IsIgnoreAnchorPoint", &cocos2d::CCNode::isIgnoreAnchorPointForPosition);
	class_def<cocos2d::Node>(ls, "Visit", (void(cocos2d::Node::*)(void))&cocos2d::CCNode::visit);
	class_def<cocos2d::Node>(ls, "SetSkewX", &cocos2d::Node::setSkewX);
	class_def<cocos2d::Node>(ls, "SetSkewY", &cocos2d::Node::setSkewY);
	class_def<cocos2d::Node>(ls, "SetOpacity", &cocos2d::Node::setOpacity);
	class_def<cocos2d::Node>(ls, "SetCascadeColorEnabled", &cocos2d::CCNode::setCascadeColorEnabled);
	class_def<cocos2d::Node>(ls, "SetCascadeOpacityEnabled", &cocos2d::CCNode::setCascadeOpacityEnabled);
	class_def<cocos2d::Node>(ls, "RunAction", &cocos2d::Node::runAction);
	class_def<cocos2d::Node>(ls, "GetActionByTag", &cocos2d::Node::getActionByTag);
	class_static<cocos2d::Node>(ls, "Create", &cocos2d::CCNode::create);

	//Action
	class_add<cocos2d::Action>(ls, "Action");
	class_def<cocos2d::Action>(ls, "getDescription", &cocos2d::Action::getDescription);
	class_def<cocos2d::Action>(ls, "SetTag", &cocos2d::Action::setTag);
	
	//ActionTimeline
	class_add<ActionTimelineEx>(ls, "ActionTimelineEx");
	class_inh<ActionTimelineEx, cocos2d::Action>(ls);
	class_def<ActionTimelineEx>(ls, "Play", &ActionTimelineEx::play);
	class_def<ActionTimelineEx>(ls, "IsPlaying", &ActionTimelineEx::isPlaying);
	class_def<ActionTimelineEx>(ls, "Pause", &ActionTimelineEx::pause);
	class_def<ActionTimelineEx>(ls, "Resume", &ActionTimelineEx::resume);
	class_def<ActionTimelineEx>(ls, "SetTimeSpeed", &ActionTimelineEx::setTimeSpeed);
	class_def<ActionTimelineEx>(ls, "GetTimeSpeed", &ActionTimelineEx::getTimeSpeed);
	class_def<ActionTimelineEx>(ls, "SetAnimationEndListener", &ActionTimelineEx::SetAnimationEndListener);
	class_def<ActionTimelineEx>(ls, "ClearAnimationEndListener", &ActionTimelineEx::ClearAnimationEndListener);
	class_def<ActionTimelineEx>(ls, "GetStartFrame", &ActionTimelineEx::getStartFrame);
	class_def<ActionTimelineEx>(ls, "GetEndFrame", &ActionTimelineEx::getEndFrame);
	class_def<ActionTimelineEx>(ls, "GotoFrameAndPause", &ActionTimelineEx::gotoFrameAndPause);
	class_def<ActionTimelineEx>(ls, "GotoFrameAndPlay", (void(ActionTimelineEx::*)(int, int, bool))&ActionTimelineEx::gotoFrameAndPlay);
	class_def<ActionTimelineEx>(ls, "GetDuration", &ActionTimelineEx::getDuration);

	//ProgressTimer
	class_add<ProgressTimer>(ls, "ProgressTimer");
	class_inh<ProgressTimer, cocos2d::Node>(ls);
	class_enum<ProgressTimer>(ls, "TypeRadial", (int)cocos2d::ProgressTimer::Type::RADIAL);
	class_enum<ProgressTimer>(ls, "TypeBar", (int)cocos2d::ProgressTimer::Type::BAR);
	class_def<ProgressTimer>(ls, "SetType", &ProgressTimer::SetType);
	class_def<ProgressTimer>(ls, "GetType", &ProgressTimer::GetType);
	class_def<ProgressTimer>(ls, "GetPercentage", &ProgressTimer::GetPercentage);
	class_def<ProgressTimer>(ls, "SetPercentage", &ProgressTimer::SetPercentage);
	class_def<ProgressTimer>(ls, "SetReverseDirection", &ProgressTimer::SetReverseDirection);
	class_def<ProgressTimer>(ls, "IsReverseDirection", &ProgressTimer::IsReverseDirection);
	class_def<ProgressTimer>(ls, "SetOpacity", &ProgressTimer::SetOpacity);
	class_def<ProgressTimer>(ls, "GetOpacity", &ProgressTimer::GetOpacity);
	class_def<ProgressTimer>(ls, "SetColor", &ProgressTimer::SetColor);
	class_def<ProgressTimer>(ls, "SetMidpoint", &ProgressTimer::setMidpoint);
	class_static<ProgressTimer>(ls, "CreateWithImageLabel", &ProgressTimer::CreateWithImageLabel);
	class_static<ProgressTimer>(ls, "CreateWithGraphicRender", &ProgressTimer::CreateWithGraphicRender);

	//LabelEx
	class_add<LabelEx>(ls, "LabelEx");
	class_inh<LabelEx, cocos2d::Node>(ls);
	class_enum<LabelEx>(ls, "ALIGIN_TOP", (int)cocos2d::TextVAlignment::TOP);
	class_enum<LabelEx>(ls, "ALIGIN_MIDDLE", (int)cocos2d::TextVAlignment::CENTER);
	class_enum<LabelEx>(ls, "ALIGIN_BOTTOM", (int)cocos2d::TextVAlignment::BOTTOM);
	class_enum<LabelEx>(ls, "ALIGIN_LEFT", (int)cocos2d::TextHAlignment::LEFT);
	class_enum<LabelEx>(ls, "ALIGIN_CENTER", (int)cocos2d::TextHAlignment::CENTER);
	class_enum<LabelEx>(ls, "ALIGIN_RIGHT", (int)cocos2d::TextHAlignment::RIGHT);
	class_def<LabelEx>(ls, "SetString", &LabelEx::SetString);
	class_def<LabelEx>(ls, "GetString", &LabelEx::GetString);
	class_def<LabelEx>(ls, "SetFontName", &LabelEx::SetFontName);
	class_def<LabelEx>(ls, "SetFontSize", &LabelEx::SetFontSize);
	class_def<LabelEx>(ls, "SetColor", &LabelEx::SetColor);
	class_def<LabelEx>(ls, "SetStroke", &LabelEx::SetStroke);
	class_def<LabelEx>(ls, "SetStrokeWidth", &LabelEx::SetStrokeWidth);
	class_def<LabelEx>(ls, "SetStrokeColor", &LabelEx::SetStrokeColor);
	class_def<LabelEx>(ls, "SetStrokeColorEx", &LabelEx::SetStrokeColorEx);
	class_def<LabelEx>(ls, "SetShadow", &LabelEx::SetShadow);
	class_def<LabelEx>(ls, "SetShadowColor", &LabelEx::SetShadowColor);
	class_def<LabelEx>(ls, "SetLineWidth", &LabelEx::SetLineWidth);
	class_def<LabelEx>(ls, "SetHeightMax", &LabelEx::SetHeightMax);
	class_def<LabelEx>(ls, "SetTextWidthAndHeight", &LabelEx::SetTextWidthAndHeight);
	class_def<LabelEx>(ls, "GetWidth", &LabelEx::GetWidth);
	class_def<LabelEx>(ls, "GetHeight", &LabelEx::GetHeight);
	class_def<LabelEx>(ls, "SetFlipX", &LabelEx::SetFlipX);
	class_def<LabelEx>(ls, "SetFlipY", &LabelEx::SetFlipY);
	class_def<LabelEx>(ls, "IsFlipX", &LabelEx::IsFlipX);
	class_def<LabelEx>(ls, "IsFlipY", &LabelEx::IsFlipY);
	class_def<LabelEx>(ls, "SetTextHorizontalAlign", &LabelEx::SetTextHorizontalAlign);
	class_def<LabelEx>(ls, "SetTextVertiacalAlign", &LabelEx::SetTextVertiacalAlign);
	class_def<LabelEx>(ls, "GetContentWidth", &LabelEx::GetContentWidth);
	class_def<LabelEx>(ls, "GetContentHeight", &LabelEx::GetContentHeight);
	class_def<LabelEx>(ls, "SetLineSpacing", &LabelEx::setLineSpacing);
	class_def<LabelEx>(ls, "GetLineSpacing", &LabelEx::getLineSpacing);
	class_static<LabelEx>(ls, "Node", &LabelEx::Node);
	class_static<LabelEx>(ls, "CreateWithString", (LabelEx*(*)(const std::string&, const std::string&, float)) &LabelEx::CreateWithString);
	class_static<LabelEx>(ls, "SetShadowOpt", &LabelEx::SetShadowOpt);
	class_static<LabelEx>(ls, "SetStrokeOpt", &LabelEx::SetStrokeOpt);
	class_static<LabelEx>(ls, "SetStrokeOptEx", &LabelEx::SetStrokeOptEx);

	//SpriteEx
	class_add<SpriteEx>(ls, "SpriteEx");
	class_inh<SpriteEx, cocos2d::Node>(ls);
	class_def<SpriteEx>(ls, "SetImage", (bool (SpriteEx::*)(const std::string&))&SpriteEx::SetImage);
	class_def<SpriteEx>(ls, "SetImage", (bool(SpriteEx::*) (const ImageData&))&SpriteEx::SetImage);
	class_def<SpriteEx>(ls, "SetColor", &SpriteEx::SetColor);
	class_def<SpriteEx>(ls, "SetStretchSize", &SpriteEx::SetStretchSize);
	class_def<SpriteEx>(ls, "SetHighLight", &SpriteEx::SetHighLight);
	class_def<SpriteEx>(ls, "SetGray", &SpriteEx::SetGray);
	class_def<SpriteEx>(ls, "SetFlipX", &SpriteEx::SetFlipX);
	class_def<SpriteEx>(ls, "SetFlipY", &SpriteEx::SetFlipY);
	class_def<SpriteEx>(ls, "IsFlipX", &SpriteEx::isFlippedX);
	class_def<SpriteEx>(ls, "IsFlipY", &SpriteEx::isFlippedY);
	class_def<SpriteEx>(ls, "GetWidth", &SpriteEx::GetWidth);
	class_def<SpriteEx>(ls, "GetHeight", &SpriteEx::GetHeight);
	class_def<SpriteEx>(ls, "GetBright", &SpriteEx::GetBright);
	class_def<SpriteEx>(ls, "SetBright", &SpriteEx::SetBright);
	class_def<SpriteEx>(ls, "SetBlur", &SpriteEx::SetBlur);
	class_def<SpriteEx>(ls, "SetFeather", &SpriteEx::SetFeather);
	class_def<SpriteEx>(ls, "SetFeatherType", &SpriteEx::SetFeatherType);
	class_def<SpriteEx>(ls, "SetFeatherFocus", &SpriteEx::SetFeatherFocus);
	class_def<SpriteEx>(ls, "SetFeatherFocusFactor", &SpriteEx::SetFeatherFocusFactor);
	class_def<SpriteEx>(ls, "SetFeatherAttenuationThreshold", &SpriteEx::SetFeatherAttenuationThreshold);
	class_def<SpriteEx>(ls, "StretchTo", &SpriteEx::SetStretchSize);
	class_static<SpriteEx>(ls, "Node", &SpriteEx::Node);
	class_static<SpriteEx>(ls, "CreateWithImageFile", (SpriteEx*(*) (const std::string&))&SpriteEx::CreateWithImageFile);
	class_static<SpriteEx>(ls, "CreateWithImageFile", (SpriteEx*(*) (const ImageData&))&SpriteEx::CreateWithImageFile);

	//Scale9SpriteEx
	class_add<Scale9SpriteEx>(ls, "Scale9SpriteEx");
	class_inh<Scale9SpriteEx, cocos2d::Node>(ls);
	class_def<Scale9SpriteEx>(ls, "SetImage", (bool (Scale9SpriteEx::*)(const std::string&))&Scale9SpriteEx::SetImage);
	class_def<Scale9SpriteEx>(ls, "SetImage", (bool (Scale9SpriteEx::*) (const ImageData&))&Scale9SpriteEx::SetImage);
	class_def<Scale9SpriteEx>(ls, "SetColor", &Scale9SpriteEx::SetColor);
	class_def<Scale9SpriteEx>(ls, "SetHighLight", &Scale9SpriteEx::SetHighLight);
	class_def<Scale9SpriteEx>(ls, "SetGray", &Scale9SpriteEx::SetGray);
	class_def<Scale9SpriteEx>(ls, "SetFlipX", &Scale9SpriteEx::SetFlipX);
	class_def<Scale9SpriteEx>(ls, "SetFlipY", &Scale9SpriteEx::SetFlipY);
	class_def<Scale9SpriteEx>(ls, "IsFlipX", &Scale9SpriteEx::IsFlipX);
	class_def<Scale9SpriteEx>(ls, "IsFlipY", &Scale9SpriteEx::IsFlipY);
	class_def<Scale9SpriteEx>(ls, "GetWidth", &Scale9SpriteEx::GetWidth);
	class_def<Scale9SpriteEx>(ls, "GetHeight", &Scale9SpriteEx::GetHeight);
	class_def<Scale9SpriteEx>(ls, "GetOriginWidth", &Scale9SpriteEx::GetOriginWidth);
	class_def<Scale9SpriteEx>(ls, "GetOriginHeight", &Scale9SpriteEx::GetOriginHeight);
	class_def<Scale9SpriteEx>(ls, "SetBright", &Scale9SpriteEx::SetBright);
	class_def<Scale9SpriteEx>(ls, "GetBright", &Scale9SpriteEx::GetBright);
	class_def<Scale9SpriteEx>(ls, "SetInnerRect", (void (Scale9SpriteEx::*)(float, float, float, float))&Scale9SpriteEx::SetInnerRect);
	class_def<Scale9SpriteEx>(ls, "SetStretchSize", &Scale9SpriteEx::SetStretchSize);
	class_def<Scale9SpriteEx>(ls, "SetContentSize", &Scale9SpriteEx::SetContentSize);
	class_def<Scale9SpriteEx>(ls, "SetContentWidth", &Scale9SpriteEx::SetContentWidth);
	class_def<Scale9SpriteEx>(ls, "SetContentHeight", &Scale9SpriteEx::SetContentHeight);
	class_static<Scale9SpriteEx>(ls, "Node", &Scale9SpriteEx::Node);
	class_static<Scale9SpriteEx>(ls, "CreateWithImageFile", (Scale9SpriteEx*(*) (const std::string&))&Scale9SpriteEx::CreateWithImageFile);
	class_static<Scale9SpriteEx>(ls, "CreateWithImageFile", (Scale9SpriteEx*(*) (const ImageData&))&Scale9SpriteEx::CreateWithImageFile);

	//StencilContainer
	class_add<ClippingNodeEx>(ls, "ClippingNodeEx");
	class_inh<ClippingNodeEx, cocos2d::Node>(ls);
	class_def<ClippingNodeEx>(ls, "SetStencilMask", &ClippingNodeEx::SetStencilMask);
	class_def<ClippingNodeEx>(ls, "IsInverted", &ClippingNodeEx::IsInverted);
	class_def<ClippingNodeEx>(ls, "SetInverted", &ClippingNodeEx::SetInverted);
	class_def<ClippingNodeEx>(ls, "SetAlphaTestValue", &ClippingNodeEx::SetAlphaTestValue);
	class_def<ClippingNodeEx>(ls, "GetAlphaTestValue", &ClippingNodeEx::GetAlphaTestValue);
	class_static<ClippingNodeEx>(ls, "Node", &ClippingNodeEx::Node);

	//TextureResType
	class_add<cocos2d::ui::Widget::TextureResType>(ls, "TextureResType");
	class_enum<cocos2d::ui::Widget::TextureResType>(ls, "LOCAL", (int)cocos2d::ui::Widget::TextureResType::LOCAL);
	class_enum<cocos2d::ui::Widget::TextureResType>(ls, "PLIST", (int)cocos2d::ui::Widget::TextureResType::PLIST);

	//UIWidget
	class_add<cocos2d::ui::Widget>(ls, "Widget");
	class_inh<cocos2d::ui::Widget, cocos2d::Node>(ls);
	class_def<cocos2d::ui::Widget>(ls, "getDescription", &cocos2d::ui::Widget::getDescription);
	class_enum<cocos2d::ui::Widget>(ls, "BRIGHT_NORMAL", (int)cocos2d::ui::Widget::BrightStyle::NORMAL);
	class_enum<cocos2d::ui::Widget>(ls, "BRIGHT_HIGHLIGHT", (int)cocos2d::ui::Widget::BrightStyle::HIGHLIGHT);
	class_enum<cocos2d::ui::Widget>(ls, "SIZETYPE_ABSOLUTE", (int)cocos2d::ui::Widget::SizeType::ABSOLUTE);
	class_enum<cocos2d::ui::Widget>(ls, "SIZETYPE_PERCENT", (int)cocos2d::ui::Widget::SizeType::PERCENT);
	class_enum<cocos2d::ui::Widget>(ls, "POSTYPE_ABSOLUTE", (int)cocos2d::ui::Widget::PositionType::ABSOLUTE);
	class_enum<cocos2d::ui::Widget>(ls, "POSTYPE_PERCENT", (int)cocos2d::ui::Widget::PositionType::PERCENT);
	class_def<cocos2d::ui::Widget>(ls, "IsEnabled", &cocos2d::ui::Widget::isEnabled);
	class_def<cocos2d::ui::Widget>(ls, "SetEnabled", &cocos2d::ui::Widget::setEnabled);
	class_def<cocos2d::ui::Widget>(ls, "SetBright", &cocos2d::ui::Widget::setBright);
	class_def<cocos2d::ui::Widget>(ls, "IsBright", &cocos2d::ui::Widget::isBright);
	class_def<cocos2d::ui::Widget>(ls, "SetName", &cocos2d::ui::Widget::setName);
	class_def<cocos2d::ui::Widget>(ls, "GetName", &cocos2d::ui::Widget::getName);
	class_def<cocos2d::ui::Widget>(ls, "SetBrightStyle", &cocos2d::ui::Widget::SetBrightStyle);
	class_def<cocos2d::ui::Widget>(ls, "SetTouchEnabled", &cocos2d::ui::Widget::setTouchEnabled);
	class_def<cocos2d::ui::Widget>(ls, "IsTouchEnabled", &cocos2d::ui::Widget::isTouchEnabled);
	class_def<cocos2d::ui::Widget>(ls, "SetFoucs", &cocos2d::ui::Widget::setFocused);
	class_def<cocos2d::ui::Widget>(ls, "IsFocused", &cocos2d::ui::Widget::isFocused);
	class_def<cocos2d::ui::Widget>(ls, "AddChild", (void (cocos2d::ui::Widget::*)(cocos2d::Node*))&cocos2d::ui::Widget::addChild);
	class_def<cocos2d::ui::Widget>(ls, "AddChild", (void (cocos2d::ui::Widget::*)(cocos2d::Node*, int))&cocos2d::ui::Widget::addChild);
	class_def<cocos2d::ui::Widget>(ls, "AddChild", (void (cocos2d::ui::Widget::*)(cocos2d::Node*, int, int))&cocos2d::ui::Widget::addChild);
	class_def<cocos2d::ui::Widget>(ls, "RemoveChild", (void (cocos2d::ui::Widget::*)(cocos2d::Node*))&cocos2d::ui::Widget::removeNode);
	class_def<cocos2d::ui::Widget>(ls, "RemoveChild", (void (cocos2d::ui::Widget::*)(cocos2d::Node*, bool))&cocos2d::ui::Widget::removeNode);
	class_def<cocos2d::ui::Widget>(ls, "RemoveSelf", &cocos2d::ui::Widget::removeFromParentAndCleanup);
	class_def<cocos2d::ui::Widget>(ls, "SetColor", &cocos2d::ui::Widget::SetColor);
	class_def<cocos2d::ui::Widget>(ls, "SetFlipX", &cocos2d::ui::Widget::setFlippedX);
	class_def<cocos2d::ui::Widget>(ls, "IsFlipX", &cocos2d::ui::Widget::isFlippedX);
	class_def<cocos2d::ui::Widget>(ls, "SetFlipY", &cocos2d::ui::Widget::setFlippedY);
	class_def<cocos2d::ui::Widget>(ls, "IsFlipY", &cocos2d::ui::Widget::isFlippedY);
	class_def<cocos2d::ui::Widget>(ls, "SetSize", (void (cocos2d::ui::Widget::*)(const float, const float))&cocos2d::ui::Widget::setContentSize);
	class_def<cocos2d::ui::Widget>(ls, "GetSize", &cocos2d::ui::Widget::getSize);
	class_def<cocos2d::ui::Widget>(ls, "IgnoreContentAdaptWithSize", &cocos2d::ui::Widget::ignoreContentAdaptWithSize);
	class_def<cocos2d::ui::Widget>(ls, "IsIgnoreContentAdaptWithSize", &cocos2d::ui::Widget::isIgnoreContentAdaptWithSize);
	class_def<cocos2d::ui::Widget>(ls, "SetPropagateTouchEvents", &cocos2d::ui::Widget::setPropagateTouchEvents);
	class_def<cocos2d::ui::Widget>(ls, "IsPropagateTouchEvents", &cocos2d::ui::Widget::isPropagateTouchEvents);
	class_def<cocos2d::ui::Widget>(ls, "SetTouchSwallow", &cocos2d::ui::Widget::setSwallowTouches);
	class_def<cocos2d::ui::Widget>(ls, "IsSwallowTouches", &cocos2d::ui::Widget::isSwallowTouches);
	class_def<cocos2d::ui::Widget>(ls, "SetSizeType", (void (cocos2d::ui::Widget::*)(unsigned int))&cocos2d::ui::Widget::setSizeType);
	class_def<cocos2d::ui::Widget>(ls, "SetSizePercent", (void (cocos2d::ui::Widget::*)(const float, const float))&cocos2d::ui::Widget::setSizePercent);
	class_def<cocos2d::ui::Widget>(ls, "SetPositionType", (void (cocos2d::ui::Widget::*)(unsigned int))&cocos2d::ui::Widget::setPositionType);
	class_def<cocos2d::ui::Widget>(ls, "SetPositionPercent", (void (cocos2d::ui::Widget::*)(const float, const float))&cocos2d::ui::Widget::setPositionPercent);
	class_def<cocos2d::ui::Widget>(ls, "RefreshLayoutComponent", &cocos2d::ui::Widget::refreshLayoutComponent);
	class_def<cocos2d::ui::Widget>(ls, "SetTouchSoundInfo", &cocos2d::ui::Widget::setTouchSoundInfo);
	class_def<cocos2d::ui::Widget>(ls, "Clone", &cocos2d::ui::Widget::clone);
	class_def<cocos2d::ui::Widget>(ls, "IsHighlighted", &cocos2d::ui::Widget::isHighlighted);

	//UILayoutEx
	class_add<UILayoutEx>(ls, "UILayoutEx");
	class_inh<UILayoutEx, cocos2d::ui::Widget>(ls);
	class_enum<UILayoutEx>(ls, "ClippingTypeScissor", (int)cocos2d::ui::Layout::ClippingType::SCISSOR);
	class_enum<UILayoutEx>(ls, "ClippingTypeStencil", (int)cocos2d::ui::Layout::ClippingType::STENCIL);
	class_enum<UILayoutEx>(ls, "LayoutTypeAbsolute", (int)cocos2d::ui::Layout::Type::ABSOLUTE);
	class_enum<UILayoutEx>(ls, "LayoutTypeVertical", (int)cocos2d::ui::Layout::Type::VERTICAL);
	class_enum<UILayoutEx>(ls, "LayoutTypeHorizontal", (int)cocos2d::ui::Layout::Type::HORIZONTAL);
	class_enum<UILayoutEx>(ls, "LayoutTypeRelative", (int)cocos2d::ui::Layout::Type::RELATIVE);
	class_def<UILayoutEx>(ls, "SetBackGroundColor", &UILayoutEx::SetBackGroundColor);
	class_def<UILayoutEx>(ls, "SetBackGroundColorType", &UILayoutEx::SetBackGroundColorType);
	class_def<UILayoutEx>(ls, "SetBackGroundImageOpacity", &UILayoutEx::setBackGroundImageOpacity);
	class_def<UILayoutEx>(ls, "SetBackGroundColorOpacity", &UILayoutEx::setBackGroundColorOpacity);
	class_def<UILayoutEx>(ls, "SetBackGroundImageScale9Enabled", &UILayoutEx::setBackGroundImageScale9Enabled);
	class_def<UILayoutEx>(ls, "SetBackGroundImageCapInsets", &UILayoutEx::SetBackGroundImageCapInsets);
	class_def<UILayoutEx>(ls, "SetBackGroundImage", (void (UILayoutEx::*)(const char*, int))&UILayoutEx::SetBackGroundImage);
	class_def<UILayoutEx>(ls, "SetBackGroundImage", (void (UILayoutEx::*)(const ImageData&))&UILayoutEx::SetBackGroundImage);
	class_def<UILayoutEx>(ls, "SetEventListener", (void(UILayoutEx::*)(GameType::UINT32, GameType::UINT32, GameType::UINT32)) &UILayoutEx::SetEventListener);
	class_def<UILayoutEx>(ls, "SetEventListener", (void(UILayoutEx::*)(GameType::UINT32, GameType::UINT32, GameType::UINT32, GameType::UINT32)) &UILayoutEx::SetEventListener);
	class_def<UILayoutEx>(ls, "SetClippingSize", &UILayoutEx::SetClippingSize);
	class_def<UILayoutEx>(ls, "SetClippingType", &UILayoutEx::SetClippingType);
	class_def<UILayoutEx>(ls, "SetClippingEnable", &UILayoutEx::SetClippingEnable);
	class_def<UILayoutEx>(ls, "SetLayoutType", &UILayoutEx::SetLayoutType);
	class_def<UILayoutEx>(ls, "SetLuaUpdate", &UILayoutEx::SetLuaUpdate);
	class_static<UILayoutEx>(ls, "Create", &UILayoutEx::create);

	//UIScrollViewEx
	class_add<UIScrollViewEx>(ls, "UIScrollViewEx");
	class_inh<UIScrollViewEx, cocos2d::ui::Widget>(ls);
	class_enum<UIScrollViewEx>(ls, "ScrollDirNone", (int)cocos2d::ui::ScrollView::Direction::NONE);
	class_enum<UIScrollViewEx>(ls, "ScrollDirVertical", (int)cocos2d::ui::ScrollView::Direction::VERTICAL);
	class_enum<UIScrollViewEx>(ls, "ScrollDirHorizontal", (int)cocos2d::ui::ScrollView::Direction::HORIZONTAL);
	class_enum<UIScrollViewEx>(ls, "ScrollDirBoth", (int)cocos2d::ui::ScrollView::Direction::BOTH);
	class_def<UIScrollViewEx>(ls, "SetEventListener", &UIScrollViewEx::SetEventListener);
	class_def<UIScrollViewEx>(ls, "SetBounceEnabled", &UIScrollViewEx::setBounceEnabled);
	class_def<UIScrollViewEx>(ls, "SetInnerContainerSize", &UIScrollViewEx::SetInnerContainerSize);
	class_def<UIScrollViewEx>(ls, "GetInnerContainerSize", &UIScrollViewEx::getInnerContainerSize);
	class_def<UIScrollViewEx>(ls, "SetInnerContainerPosition", &UIScrollViewEx::setInnerContainerPosition);
	class_def<UIScrollViewEx>(ls, "GetInnerContainerPosition", &UIScrollViewEx::getInnerContainerPosition);
	class_def<UIScrollViewEx>(ls, "MoveInnerContainer", &UIScrollViewEx::MoveInnerContainer);
	class_def<UIScrollViewEx>(ls, "ScrollToTop", &UIScrollViewEx::ScrollToTop);
	class_def<UIScrollViewEx>(ls, "ScrollToBottom", &UIScrollViewEx::ScrollToBottom);
	class_def<UIScrollViewEx>(ls, "ScrollToLeft", &UIScrollViewEx::ScrollToLeft);
	class_def<UIScrollViewEx>(ls, "ScrollToRight", &UIScrollViewEx::ScrollToRight);
	class_def<UIScrollViewEx>(ls, "SetBackGroundColor", &UIScrollViewEx::SetBackGroundColor);
	class_def<UIScrollViewEx>(ls, "SetScrollBarEnabled", &UIScrollViewEx::SetScrollBarEnabled);
	class_def<UIScrollViewEx>(ls, "SetScrollDirection", &UIScrollViewEx::SetScrollDirection);
	class_def<UIScrollViewEx>(ls, "SetScrollHorizontalVolume", &UIScrollViewEx::SetScrollHorizontalVolume);
	class_def<UIScrollViewEx>(ls, "SetScrollVerticalVolume", &UIScrollViewEx::SetScrollVerticalVolume);
	class_def<UIScrollViewEx>(ls, "GetScrollPercentVertical", &UIScrollViewEx::GetScrollPercentVertical);
	class_def<UIScrollViewEx>(ls, "GetHorizontalScrollVolume", &UIScrollViewEx::GetHorizontalScrollVolume);
	class_def<UIScrollViewEx>(ls, "GetVerticalScrollVolume", &UIScrollViewEx::GetVerticalScrollVolume);
	class_def<UIScrollViewEx>(ls, "SetLayoutType", &UIScrollViewEx::SetLayoutType);
	class_def<UIScrollViewEx>(ls, "StartAutoScrollToDestination", &UIScrollViewEx::StartAutoScrollToDestination);
	class_static<UIScrollViewEx>(ls, "Create", &UIScrollViewEx::create);
	
	//UIListViewEx
	class_add<UIListViewEx>(ls, "UIListViewEx");
	class_inh<UIListViewEx, cocos2d::ui::Widget>(ls);
	class_enum<UIListViewEx>(ls, "ScrollDirNone", (int)cocos2d::ui::ScrollView::Direction::NONE);
	class_enum<UIListViewEx>(ls, "ScrollDirVertical", (int)cocos2d::ui::ScrollView::Direction::VERTICAL);
	class_enum<UIListViewEx>(ls, "ScrollDirHorizontal", (int)cocos2d::ui::ScrollView::Direction::HORIZONTAL);
	class_enum<UIListViewEx>(ls, "ScrollDirBoth", (int)cocos2d::ui::ScrollView::Direction::BOTH);
	class_def<UIListViewEx>(ls, "SetEventListener", (void(UIListViewEx::*)(GameType::UINT32))&UIListViewEx::AddEventListener);
	class_def<UIListViewEx>(ls, "AddCreateEventListener", (void(UIListViewEx::*)(GameType::UINT32))&UIListViewEx::AddCreateEventListener);
	class_def<UIListViewEx>(ls, "GetInnerContainerPos", &UIListViewEx::GetInnerContainerPos);
	class_def<UIListViewEx>(ls, "SetScrollHorizontalVolume", &UIListViewEx::scrollToPercentHorizontal);
	class_def<UIListViewEx>(ls, "SetScrollVerticalVolume", &UIListViewEx::scrollToPercentVertical);
	class_def<UIListViewEx>(ls, "SetBounceEnabled", &UIListViewEx::setBounceEnabled);
	class_def<UIListViewEx>(ls, "SetInnerContainerSize", &UIListViewEx::setInnerContainerSize);
	class_def<UIListViewEx>(ls, "GetInnerContainerSize", &UIListViewEx::getInnerContainerSize);
	class_def<UIListViewEx>(ls, "JumpToPercentVertical", &UIListViewEx::jumpToPercentVertical);
	class_def<UIListViewEx>(ls, "JumpToPercentHorizontal", &UIListViewEx::jumpToPercentHorizontal);
	class_def<UIListViewEx>(ls, "StartAutoScrollToDestination", &UIListViewEx::StartAutoScrollToDestination);
	class_def<UIListViewEx>(ls, "StartAutoScroll", &UIListViewEx::StartAutoScroll);
	class_def<UIListViewEx>(ls, "ScrollToTop", &UIListViewEx::jumpToTop);
	class_def<UIListViewEx>(ls, "ScrollToBottom", &UIListViewEx::jumpToBottom);
	class_def<UIListViewEx>(ls, "ScrollToLeft", &UIListViewEx::jumpToLeft);
	class_def<UIListViewEx>(ls, "ScrollToRight", &UIListViewEx::jumpToRight);
	class_def<UIListViewEx>(ls, "ForceDoLayout", &UIListViewEx::forceDoLayout);
	class_def<UIListViewEx>(ls, "SetItemModel", &UIListViewEx::setItemModel);
	class_def<UIListViewEx>(ls, "PushBackDefaultItem", &UIListViewEx::pushBackDefaultItem);
	class_def<UIListViewEx>(ls, "InsertDefaultItem", &UIListViewEx::insertDefaultItem);
	class_def<UIListViewEx>(ls, "PushBackCustomItem", &UIListViewEx::pushBackCustomItem);
	class_def<UIListViewEx>(ls, "RemoveLastItem", &UIListViewEx::removeLastItem);
	class_def<UIListViewEx>(ls, "InsertCustomItem", &UIListViewEx::insertCustomItem);
	class_def<UIListViewEx>(ls, "RemoveItem", &UIListViewEx::removeItem);
	class_def<UIListViewEx>(ls, "RemoveAllItems", &UIListViewEx::removeAllItems);
	class_def<UIListViewEx>(ls, "SetItemsMargin", &UIListViewEx::setItemsMargin);
	class_def<UIListViewEx>(ls, "GetCurSelectedIndex", &UIListViewEx::getCurSelectedIndex);
	class_def<UIListViewEx>(ls, "SortAllChildren", &UIListViewEx::sortAllChildren);
	class_def<UIListViewEx>(ls, "GetIndex", &UIListViewEx::getIndex);
	class_def<UIListViewEx>(ls, "GetItem", &UIListViewEx::getItem);
	class_def<UIListViewEx>(ls, "GetItemNum", &UIListViewEx::GetItemNum);
	class_def<UIListViewEx>(ls, "SetDirection", &UIListViewEx::SetDirection);
	class_def<UIListViewEx>(ls, "JumpToItem", &UIListViewEx::jumpToItem);
	class_def<UIListViewEx>(ls, "ScrollToItem", (void(cocos2d::ui::ListView::*)(ssize_t, const cocos2d::Vec2&, const cocos2d::Vec2&, float))&cocos2d::ui::ListView::scrollToItem);
	class_def<UIListViewEx>(ls, "GetScrollPercent", &UIListViewEx::GetScrollPercent);
	class_static<UIListViewEx>(ls, "Create", &UIListViewEx::create);
	
	//UIPageViewEx
	class_add<UIPageViewEx>(ls, "UIPageViewEx");
	class_inh<UIPageViewEx, cocos2d::ui::Widget>(ls);
	class_def<UIPageViewEx>(ls, "AddWidgetToPage", &UIPageViewEx::AddWidgetToPage);
	class_def<UIPageViewEx>(ls, "AddPage", &UIPageViewEx::AddPage);
	class_def<UIPageViewEx>(ls, "InsertPage", &UIPageViewEx::InsertPage);
	class_def<UIPageViewEx>(ls, "RemovePage", &UIPageViewEx::RemovePage);
	class_def<UIPageViewEx>(ls, "RemovePageAtIndex", &UIPageViewEx::RemovePageAtIndex);
	class_def<UIPageViewEx>(ls, "RemoveAllPages", &UIPageViewEx::RemoveAllPages);
	class_def<UIPageViewEx>(ls, "ScrollToPage", &UIPageViewEx::ScrollToPage);
	class_def<UIPageViewEx>(ls, "ScrollToPageByTime", &UIPageViewEx::ScrollToPageByTime);
	class_def<UIPageViewEx>(ls, "GetCurPageIndex", &UIPageViewEx::GetCurPageIndex);
	class_def<UIPageViewEx>(ls, "GetCurPage", &UIPageViewEx::GetCurPage);
	class_def<UIPageViewEx>(ls, "GetPage", &UIPageViewEx::GetPage);
	class_def<UIPageViewEx>(ls, "CreatePage", &UIPageViewEx::CreatePage);
	class_def<UIPageViewEx>(ls, "SetPageSize", (void(UIPageViewEx::*)(float, float))&UIPageViewEx::SetPageSize);
	class_def<UIPageViewEx>(ls, "GetPageNum", &UIPageViewEx::GetPageNum);
	class_def<UIPageViewEx>(ls, "SetEventListener", (void(UIPageViewEx::*)(GameType::UINT32))&UIPageViewEx::AddEventListener);
	class_def<UIPageViewEx>(ls, "ForceDoLayout", &UIListViewEx::forceDoLayout);
	class_def<UIPageViewEx>(ls, "SetInnerContainerPosition", &UIPageViewEx::setInnerContainerPosition);
	class_def<UIPageViewEx>(ls, "GetInnerContainerPosition", &UIPageViewEx::getInnerContainerPosition);
	class_static<UIPageViewEx>(ls, "Create", &UIPageViewEx::create);

	//UIButtonEx
	class_add<UIButtonEx>(ls, "UIButtonEx");
	class_inh<UIButtonEx, cocos2d::ui::Widget>(ls);
	class_def<UIButtonEx>(ls, "SetNormalImage", (void (UIButtonEx::*)(const std::string&, int))&UIButtonEx::SetNormalImage);
	class_def<UIButtonEx>(ls, "SetNormalImage", (void (UIButtonEx::*)(const ImageData&))&UIButtonEx::SetNormalImage);
	class_def<UIButtonEx>(ls, "SetSelectedImage", (void (UIButtonEx::*)(const std::string&, int))&UIButtonEx::SetSelectedImage);
	class_def<UIButtonEx>(ls, "SetSelectedImage", (void (UIButtonEx::*)(const ImageData&))&UIButtonEx::SetSelectedImage);
	class_def<UIButtonEx>(ls, "SetDisabledImage", (void (UIButtonEx::*)(const std::string&, int))&UIButtonEx::SetDisabledImage);
	class_def<UIButtonEx>(ls, "SetDisabledImage", (void (UIButtonEx::*)(const ImageData&))&UIButtonEx::SetDisabledImage);
	class_def<UIButtonEx>(ls, "SetScale9Enabled", &UIButtonEx::SetScale9Enabled);
	class_def<UIButtonEx>(ls, "IsScale9Enabled", &UIButtonEx::isScale9Enabled);
	class_def<UIButtonEx>(ls, "SetCapInsets", &UIButtonEx::SetCapInsets);
	class_def<UIButtonEx>(ls, "SetText", &UIButtonEx::SetText);
	class_def<UIButtonEx>(ls, "SetFontName", &UIButtonEx::SetFontName);
	class_def<UIButtonEx>(ls, "SetFontSize", &UIButtonEx::SetFontSize);
	class_def<UIButtonEx>(ls, "SetTextColor", &UIButtonEx::SetTextColor);
	class_def<UIButtonEx>(ls, "SetShadow", &UIButtonEx::SetShadow);
	class_def<UIButtonEx>(ls, "SetOutline", &UIButtonEx::SetOutline);
	class_def<UIButtonEx>(ls, "DisableEffect", &UIButtonEx::DisableEffect);
	class_def<UIButtonEx>(ls, "GetText", &UIButtonEx::GetText);
	class_def<UIButtonEx>(ls, "SetEnabled", &UIButtonEx::setEnabled);
	class_def<UIButtonEx>(ls, "IsEnable", &UIButtonEx::GetEnable);
	class_def<UIButtonEx>(ls, "SetEventListener", (void(cocos2d::ui::Widget::*)(GameType::UINT32, GameType::UINT32, GameType::UINT32)) &UIButtonEx::SetEventListener);
	class_def<UIButtonEx>(ls, "SetEventListener", (void(cocos2d::ui::Widget::*)(GameType::UINT32, GameType::UINT32, GameType::UINT32, GameType::UINT32)) &UIButtonEx::SetEventListener);
	class_def<UIButtonEx>(ls, "SetPressedActionEnabled", &UIButtonEx::setPressedActionEnabled);
	class_static<UIButtonEx>(ls, "Create", &UIButtonEx::create);
	class_static<UIButtonEx>(ls, "CreateWithImage", (UIButtonEx*(*)(const std::string&, int))&UIButtonEx::CreateWithImage);
	class_static<UIButtonEx>(ls, "CreateWithImage", (UIButtonEx*(*)(const ImageData&))&UIButtonEx::CreateWithImage);

	//UITextEx
	class_add<UITextEx>(ls, "UITextEx");
	class_inh<UITextEx, cocos2d::ui::Widget>(ls);
	class_enum<UITextEx>(ls, "ALIGIN_TOP", (int)cocos2d::TextVAlignment::TOP);
	class_enum<UITextEx>(ls, "ALIGIN_MIDDLE", (int)cocos2d::TextVAlignment::CENTER);
	class_enum<UITextEx>(ls, "ALIGIN_BOTTOM", (int)cocos2d::TextVAlignment::BOTTOM);
	class_enum<UITextEx>(ls, "ALIGIN_LEFT", (int)cocos2d::TextHAlignment::LEFT);
	class_enum<UITextEx>(ls, "ALIGIN_CENTER", (int)cocos2d::TextHAlignment::CENTER);
	class_enum<UITextEx>(ls, "ALIGIN_RIGHT", (int)cocos2d::TextHAlignment::RIGHT);
	class_def<UITextEx>(ls, "SetText", &UITextEx::SetText);
	class_def<UITextEx>(ls, "SetFontName", &UITextEx::SetFontName);
	class_def<UITextEx>(ls, "SetFontSize", &UITextEx::SetFontSize);
	class_def<UITextEx>(ls, "SetTextColor", &UITextEx::SetTextColor);
	class_def<UITextEx>(ls, "GetText", &UITextEx::GetText);
	class_def<UITextEx>(ls, "GetLength", &UITextEx::GetLength);
	class_def<UITextEx>(ls, "SetStroke", &UITextEx::SetStroke);
	class_def<UITextEx>(ls, "SetStrokeColor", &UITextEx::SetStrokeColor);
	class_def<UITextEx>(ls, "SetShadow", &UITextEx::SetShadow);
	class_def<UITextEx>(ls, "SetShadowColor", &UITextEx::SetShadowColor);
	class_def<UITextEx>(ls, "SetTextHorizontalAlign", &UITextEx::SetTextHorizontalAlign);
	class_def<UITextEx>(ls, "SetTextVertiacalAlign", &UITextEx::SetTextVertiacalAlign);
	class_def<UITextEx>(ls, "SetTextAreaSize", &UITextEx::SetTextAreaSize);
	class_def<UITextEx>(ls, "DisableEffect", &UITextEx::DisableEffect);
	class_def<UITextEx>(ls, "SetEventListener", &UITextEx::AddEventListener);
	class_static<UITextEx>(ls, "Create", &UITextEx::create);
	class_static<UITextEx>(ls, "CreateWithString", &UITextEx::CreateWithString);

	//UIImageViewEx
	class_add<UIImageViewEx>(ls, "UIImageViewEx");
	class_inh<UIImageViewEx, cocos2d::ui::Widget>(ls);
	class_def<UIImageViewEx>(ls, "SetImage", (void (UIImageViewEx::*)(const std::string&, int))&UIImageViewEx::SetImage);
	class_def<UIImageViewEx>(ls, "SetImage", (void (UIImageViewEx::*)(const ImageData&))&UIImageViewEx::SetImage);
	class_def<UIImageViewEx>(ls, "SetImageRect", &UIImageViewEx::SetImageRect);
	class_def<UIImageViewEx>(ls, "SetScale9Enabled", &UIImageViewEx::SetScale9Enabled);
	class_def<UIImageViewEx>(ls, "SetCapInsets", &UIImageViewEx::SetCapInsets);
	class_def<UIImageViewEx>(ls, "SetEventListener", &UIImageViewEx::AddEventListener);
	class_def<UIImageViewEx>(ls, "SetGray", &UIImageViewEx::SetGray);
	class_def<UIImageViewEx>(ls, "GetVirtualRenderer", &UIImageViewEx::getVirtualRenderer);
	class_def<UIImageViewEx>(ls, "SetAliasTexParameters", &UIImageViewEx::SetAliasTexParameters);
	class_static<UIImageViewEx>(ls, "Create", &UIImageViewEx::create);
	class_static<UIImageViewEx>(ls, "CreateWithImage", (UIImageViewEx*(*)(const std::string&, int))&UIImageViewEx::CreateWithImage);
	class_static<UIImageViewEx>(ls, "CreateWithImage", (UIImageViewEx*(*)(const ImageData&))&UIImageViewEx::CreateWithImage);

	//UILoadingBarEx
	class_add<UILoadingBarEx>(ls, "UILoadingBarEx");
	class_inh<UILoadingBarEx, cocos2d::ui::Widget>(ls);
	class_enum<UILoadingBarEx>(ls, "DIR_LEFT", (int)cocos2d::ui::LoadingBar::Direction::LEFT);
	class_enum<UILoadingBarEx>(ls, "DIR_RIGHT", (int)cocos2d::ui::LoadingBar::Direction::RIGHT);
	class_def<UILoadingBarEx>(ls, "SetImage", (void (UILoadingBarEx::*)(const std::string&))&UILoadingBarEx::SetImage);
	class_def<UILoadingBarEx>(ls, "SetImage", (void (UILoadingBarEx::*)(const ImageData&))&UILoadingBarEx::SetImage);
	class_def<UILoadingBarEx>(ls, "SetScale9Enabled", &UILoadingBarEx::SetScale9Enabled);
	class_def<UILoadingBarEx>(ls, "SetCapInsets", &UILoadingBarEx::SetCapInsets);
	class_def<UILoadingBarEx>(ls, "SetDirection", &UILoadingBarEx::SetDirection);
	class_def<UILoadingBarEx>(ls, "SetPercent", &UILoadingBarEx::SetPercent);
	class_def<UILoadingBarEx>(ls, "GetPercent", &UILoadingBarEx::GetPercent);
	class_def<UILoadingBarEx>(ls, "SetShowMinPercent", &UILoadingBarEx::SetShowMinPercent);
	class_static<UILoadingBarEx>(ls, "Create", &UILoadingBarEx::create);
	class_static<UILoadingBarEx>(ls, "CreateWithImage", (UILoadingBarEx*(*)(const std::string&))&UILoadingBarEx::CreateWithImage);
	class_static<UILoadingBarEx>(ls, "CreateWithImage", (UILoadingBarEx*(*)(const ImageData&))&UILoadingBarEx::CreateWithImage);

	//UITextFieldEx
	class_add<UITextFieldEx>(ls, "UITextFieldEx");
	class_inh<UITextFieldEx, cocos2d::ui::Widget>(ls);
	class_enum<UITextFieldEx>(ls, "ALIGIN_TOP", (int)cocos2d::TextVAlignment::TOP);
	class_enum<UITextFieldEx>(ls, "ALIGIN_MIDDLE", (int)cocos2d::TextVAlignment::CENTER);
	class_enum<UITextFieldEx>(ls, "ALIGIN_BOTTOM", (int)cocos2d::TextVAlignment::BOTTOM);
	class_enum<UITextFieldEx>(ls, "ALIGIN_LEFT", (int)cocos2d::TextHAlignment::LEFT);
	class_enum<UITextFieldEx>(ls, "ALIGIN_CENTER", (int)cocos2d::TextHAlignment::CENTER);
	class_enum<UITextFieldEx>(ls, "ALIGIN_RIGHT", (int)cocos2d::TextHAlignment::RIGHT);
	class_def<UITextFieldEx>(ls, "SetText", &UITextFieldEx::SetText);
	class_def<UITextFieldEx>(ls, "SetFontName", &UITextFieldEx::SetFontName);
	class_def<UITextFieldEx>(ls, "SetFontSize", &UITextFieldEx::SetFontSize);
	class_def<UITextFieldEx>(ls, "SetTextColor", &UITextFieldEx::SetTextColor);
	class_def<UITextFieldEx>(ls, "GetText", &UITextFieldEx::GetText);
	class_def<UITextFieldEx>(ls, "SetPlaceHolderText", &UITextFieldEx::SetPlaceHolderText);
	class_def<UITextFieldEx>(ls, "SetPlaceHolderColor", &UITextFieldEx::SetPlaceHolderColor);
	class_def<UITextFieldEx>(ls, "SetFilterStr", &UITextFieldEx::SetFilterStr);
	class_def<UITextFieldEx>(ls, "SetTouchSize", &UITextFieldEx::SetTouchSize);
	class_def<UITextFieldEx>(ls, "SetMaxLength", &UITextFieldEx::SetMaxLength);
	class_def<UITextFieldEx>(ls, "GetMaxLength", &UITextFieldEx::getMaxLength);
	class_def<UITextFieldEx>(ls, "SetPasswordMode", &UITextFieldEx::SetPasswordMode);
	class_def<UITextFieldEx>(ls, "SetTextHorizontalAlign", &UITextFieldEx::SetTextHorizontalAlign);
	class_def<UITextFieldEx>(ls, "SetTextVertiacalAlign", &UITextFieldEx::SetTextVertiacalAlign);
	class_def<UITextFieldEx>(ls, "SetTouchAreaEnabled", &UITextFieldEx::setTouchAreaEnabled);
	class_def<UITextFieldEx>(ls, "SetMaxLengthEnabled", &UITextFieldEx::setMaxLengthEnabled);
	class_def<UITextFieldEx>(ls, "IsMaxLengthEnabled", &UITextFieldEx::isMaxLengthEnabled);
	class_def<UITextFieldEx>(ls, "SetOnlyVisibleTouchArea", &UITextFieldEx::SetOnlyVisibleTouchArea);
	class_def<UITextFieldEx>(ls, "SetEventListener", &UITextFieldEx::SetEventListener);
	class_static<UITextFieldEx>(ls, "Create", &UITextFieldEx::create);
	class_static<UITextFieldEx>(ls, "CreateWithString", (UITextFieldEx*(*)(const std::string&, const std::string&, int))&UITextFieldEx::CreateWithString);

	//UICheckBoxEx
	class_add<UICheckBoxEx>(ls, "UICheckBoxEx");
	class_inh<UICheckBoxEx, cocos2d::ui::Widget>(ls);
	class_def<UICheckBoxEx>(ls, "SetSelectedState", &UICheckBoxEx::SetSelectedState);
	class_def<UICheckBoxEx>(ls, "GetSelectedState", &UICheckBoxEx::GetSelectedState);
	class_def<UICheckBoxEx>(ls, "SetEventListener", &UICheckBoxEx::SetEventListener);
	class_def<UICheckBoxEx>(ls, "SetBackGroundImage", (void (UICheckBoxEx::*)(const std::string&))&UICheckBoxEx::SetBackGroundImage);
	class_def<UICheckBoxEx>(ls, "SetBackGroundImage", (void (UICheckBoxEx::*)(const ImageData&))&UICheckBoxEx::SetBackGroundImage);
	class_def<UICheckBoxEx>(ls, "SetBackGroundSelectImage", (void (UICheckBoxEx::*)(const std::string&))&UICheckBoxEx::SetBackGroundSelectImage);
	class_def<UICheckBoxEx>(ls, "SetBackGroundSelectImage", (void (UICheckBoxEx::*)(const ImageData&))&UICheckBoxEx::SetBackGroundSelectImage);
	class_def<UICheckBoxEx>(ls, "SetBackGroundDisableImage", (void (UICheckBoxEx::*)(const std::string&))&UICheckBoxEx::SetBackGroundDisableImage);
	class_def<UICheckBoxEx>(ls, "SetBackGroundDisableImage", (void (UICheckBoxEx::*)(const ImageData&))&UICheckBoxEx::SetBackGroundDisableImage);
	class_def<UICheckBoxEx>(ls, "SetCrossImage", (void (UICheckBoxEx::*)(const std::string&))&UICheckBoxEx::SetCrossImage);
	class_def<UICheckBoxEx>(ls, "SetCrossImage", (void (UICheckBoxEx::*)(const ImageData&))&UICheckBoxEx::SetCrossImage);
	class_def<UICheckBoxEx>(ls, "SetCrossDisableImage", (void (UICheckBoxEx::*)(const std::string&))&UICheckBoxEx::SetCrossDisableImage);
	class_def<UICheckBoxEx>(ls, "SetCrossDisableImage", (void (UICheckBoxEx::*)(const ImageData&))&UICheckBoxEx::SetCrossDisableImage);
	class_def<UICheckBoxEx>(ls, "SetGray", &UICheckBoxEx::SetGray);
	class_static<UICheckBoxEx>(ls, "Create", &UICheckBoxEx::create);
	class_static<UICheckBoxEx>(ls, "CreateWithImage", (UICheckBoxEx*(*)(const std::string&, const std::string&))&UICheckBoxEx::CreateWithImage);
	class_static<UICheckBoxEx>(ls, "CreateWithImage", (UICheckBoxEx*(*)(const ImageData&, const ImageData&))&UICheckBoxEx::CreateWithImage);

	//UILabelAtlasEx
	class_add<UITextAtlasEx>(ls, "UITextAtlasEx");
	class_inh<UITextAtlasEx, cocos2d::ui::Widget>(ls);
	class_def<UITextAtlasEx>(ls, "SetProperty", &UITextAtlasEx::SetProperty);
	class_def<UITextAtlasEx>(ls, "SetString", &UITextAtlasEx::SetString);
	class_def<UITextAtlasEx>(ls, "GetString", &UITextAtlasEx::GetString);
	class_static<UITextAtlasEx>(ls, "Create", &UITextAtlasEx::create);
	class_static<UITextAtlasEx>(ls, "CreateWithImage", (UITextAtlasEx*(*) (const char*, const char*, int)) &UITextAtlasEx::CreateWithImage);
	class_static<UITextAtlasEx>(ls, "CreateWithImage", (UITextAtlasEx*(*) (const char*, const char*, int, int, int)) &UITextAtlasEx::CreateWithImage);

	//UISliderEx
	class_add<UISliderEx>(ls, "UISliderEx");
	class_inh<UISliderEx, cocos2d::ui::Widget>(ls);
	class_def<UISliderEx>(ls, "SetBarImage", (void (UISliderEx::*)(const std::string&))&UISliderEx::SetBarImage);
	class_def<UISliderEx>(ls, "SetBarImage", (void (UISliderEx::*)(const ImageData&))&UISliderEx::SetBarImage);
	class_def<UISliderEx>(ls, "SetBarProgressImage", (void (UISliderEx::*)(const std::string&))&UISliderEx::SetBarProgressImage);
	class_def<UISliderEx>(ls, "SetBarProgressImage", (void (UISliderEx::*)(const ImageData&))&UISliderEx::SetBarProgressImage);
	class_def<UISliderEx>(ls, "SetBarBallImage", (void (UISliderEx::*)(const std::string&))&UISliderEx::SetBarBallImage);
	class_def<UISliderEx>(ls, "SetBarBallImage", (void (UISliderEx::*)(const ImageData&))&UISliderEx::SetBarBallImage);
	class_def<UISliderEx>(ls, "SetBarBallPressedImage", (void (UISliderEx::*)(const std::string&))&UISliderEx::SetBarBallPressedImage);
	class_def<UISliderEx>(ls, "SetBarBallPressedImage", (void (UISliderEx::*)(const ImageData&))&UISliderEx::SetBarBallPressedImage);
	class_def<UISliderEx>(ls, "SetBarBallDisabledImage", (void (UISliderEx::*)(const std::string&))&UISliderEx::SetBarBallDisabledImage);
	class_def<UISliderEx>(ls, "SetBarBallDisabledImage", (void (UISliderEx::*)(const ImageData&))&UISliderEx::SetBarBallDisabledImage);
	class_def<UISliderEx>(ls, "SetPercent", &UISliderEx::SetPercent);
	class_def<UISliderEx>(ls, "GetPercent", &UISliderEx::GetPercent);
	class_def<UISliderEx>(ls, "SetScale9Enabled", &UISliderEx::SetScale9Enabled);
	class_def<UISliderEx>(ls, "SetCapInsets", &UISliderEx::SetCapInsets);
	class_def<UISliderEx>(ls, "SetEventListener", &UISliderEx::SetEventListener);
	class_static<UISliderEx>(ls, "Create", &UISliderEx::create);
	class_static<UISliderEx>(ls, "CreateWithImage", (UISliderEx*(*)(const std::string&, const std::string&))&UISliderEx::CreateWithImage);
	class_static<UISliderEx>(ls, "CreateWithImage", (UISliderEx*(*)(const ImageData&, const ImageData&))&UISliderEx::CreateWithImage);

	//WidgetUnity
	class_add<WidgetUnity>(ls, "CPPWidgetUnity");
	class_static<WidgetUnity>(ls, "GetWidgetByName", &WidgetUnity::GetWidgetByName);

	//SkeletonDataCahce
	class_add<SkeletonDataCache>(ls, "CPPSkeletonDataCahce");
	class_def<SkeletonDataCache>(ls, "LoadSkeletonDataAsync", &SkeletonDataCache::LoadSkeletonDataAsync);
	class_def<SkeletonDataCache>(ls, "LoadSsdzSkeletonDataAsync", (void(SkeletonDataCache::*)(const std::string&, GameType::UINT32))&SkeletonDataCache::LoadSsdzSkeletonDataAsync);
	class_def<SkeletonDataCache>(ls, "IsSkeletonDataInCache", &SkeletonDataCache::IsSkeletonDataInCache);
	class_def<SkeletonDataCache>(ls, "IsSSdzDataInCache", &SkeletonDataCache::IsSSdzDataInCache);
	class_instance<SkeletonDataCache>(ls, &SkeletonDataCache::GetInstance);

	//SkeletonAnimationEx
	class_add<SkeletonAnimationEx>(ls, "SkeletonAnimationEx");
	class_inh<SkeletonAnimationEx, cocos2d::Node>(ls);
	class_def<SkeletonAnimationEx>(ls, "SetAnimation", &SkeletonAnimationEx::SetAnimation);
	class_def<SkeletonAnimationEx>(ls, "SetMix", &SkeletonAnimationEx::SetMix);
	class_def<SkeletonAnimationEx>(ls, "SetTimeScale", &SkeletonAnimationEx::SetTimeScale);
	class_def<SkeletonAnimationEx>(ls, "SetAnimationListener", &SkeletonAnimationEx::SetAnimationListener);
	class_def<SkeletonAnimationEx>(ls, "ClearAnimationListener", &SkeletonAnimationEx::ClearAnimationListener);
	class_def<SkeletonAnimationEx>(ls, "AddAnimation", &SkeletonAnimationEx::AddAnimation);
	class_def<SkeletonAnimationEx>(ls, "SetSkin", &SkeletonAnimationEx::SetSkin);
	class_def<SkeletonAnimationEx>(ls, "SetToSetupPose", &SkeletonAnimationEx::SetToSetupPose);
	class_def<SkeletonAnimationEx>(ls, "SetAttachment", &SkeletonAnimationEx::SetAttachment);
	class_def<SkeletonAnimationEx>(ls, "GetHeight", &SkeletonAnimationEx::GetHeight);
	class_def<SkeletonAnimationEx>(ls, "GetWidth", &SkeletonAnimationEx::GetWidth);
	class_def<SkeletonAnimationEx>(ls, "GetOriginX", &SkeletonAnimationEx::GetOriginX);
	class_def<SkeletonAnimationEx>(ls, "GetOriginY", &SkeletonAnimationEx::GetOriginY);
	class_def<SkeletonAnimationEx>(ls, "SetColor", &SkeletonAnimationEx::SetColor);
	class_def<SkeletonAnimationEx>(ls, "SetHighLight", &SkeletonAnimationEx::SetHighLight);
	class_def<SkeletonAnimationEx>(ls, "SetGray", &SkeletonAnimationEx::SetGray);
	class_def<SkeletonAnimationEx>(ls, "ResetSkeleton", (void(SkeletonAnimationEx::*)(const char*, const char*))&SkeletonAnimationEx::ResetSkeleton);
	class_def<SkeletonAnimationEx>(ls, "ResetSkeleton", (void(SkeletonAnimationEx::*)(const char*))&SkeletonAnimationEx::ResetSkeleton);
	class_def<SkeletonAnimationEx>(ls, "SetFlipX", &SkeletonAnimationEx::SetFlipX);
	class_def<SkeletonAnimationEx>(ls, "SetFlipY", &SkeletonAnimationEx::SetFlipY);
	class_def<SkeletonAnimationEx>(ls, "IsFlipX", &SkeletonAnimationEx::IsFlipX);
	class_def<SkeletonAnimationEx>(ls, "IsFlipY", &SkeletonAnimationEx::IsFlipY);
	class_def<SkeletonAnimationEx>(ls, "SetRunAnimationEx", &SkeletonAnimationEx::SetRunAnimationEx);
	class_def<SkeletonAnimationEx>(ls, "StopAnimation", &SkeletonAnimationEx::StopAnimation);
	class_def<SkeletonAnimationEx>(ls, "ResumeAnimation", &SkeletonAnimationEx::ResumeAnimation);
	class_def<SkeletonAnimationEx>(ls, "IsAnimationExist", &SkeletonAnimationEx::IsAnimationExist);
	class_def<SkeletonAnimationEx>(ls, "GetPlayAnimationName", &SkeletonAnimationEx::GetPlayAnimationName);
	class_def<SkeletonAnimationEx>(ls, "GetPlayAnimationTime", &SkeletonAnimationEx::GetPlayAnimationTime);
	class_def<SkeletonAnimationEx>(ls, "SetToPlayTime", &SkeletonAnimationEx::SetToPlayTime);
	class_def<SkeletonAnimationEx>(ls, "GetNodeBySlotName", &SkeletonAnimationEx::GetNodeBySlotName);
	class_def<SkeletonAnimationEx>(ls, "SetSlotNodeOffSet", (void (SkeletonAnimationEx::*)(float, float))&SkeletonAnimationEx::SetSlotNodeOffSet);
	class_static<SkeletonAnimationEx>(ls, "CreateWithFile", (SkeletonAnimationEx* (*)(const char*, const char*))&SkeletonAnimationEx::CreateWithFile);
	class_static<SkeletonAnimationEx>(ls, "CreateWithFile", (SkeletonAnimationEx* (*)(const char*))&SkeletonAnimationEx::CreateWithFile);
	class_static<SkeletonAnimationEx>(ls, "SetSkeletonFps", (void(*)(float))SkeletonAnimationEx::SetSkeletonFps);

	//GameLib
	module_addEx(ls, "CPPGameLib");
	defEx(ls, "LuaLog", &cocos2d::LuaLog);
	defEx(ls, "LuaLogInfo", &LogUtil::LuaLogInfo);
	defEx(ls, "LuaLogError", &LogUtil::LuaLogError);
	defEx(ls, "Md5", (const std::string(*)(const std::string&))&ZQ::StrUtil::Md5);
	defEx(ls, "UrlEncode", &ZQ::StrUtil::UrlEncode);
	defEx(ls, "Base64Encode", &ZQ::StrUtil::Base64Encode);
	defEx(ls, "Rand", &rand);

	//GameEngine
	class_add<GameEngine>(ls, "CPPGameEngine");
	class_def<GameEngine>(ls, "GetNowTime", &GameEngine::GetNowTime);
	class_def<GameEngine>(ls, "GetNowTimeMilli", &GameEngine::GetNowTimeMilli);
	class_def<GameEngine>(ls, "CloseApp", &GameEngine::CloseApp);
	class_def<GameEngine>(ls, "Restart", &GameEngine::Restart);
	class_def<GameEngine>(ls, "SwitchLanguageAndRestart", &GameEngine::SwitchLanguageAndRestart);
	class_instance<GameEngine>(ls, &GameEngine::GetInstance);

	//FileVersion
	class_add<FileVersion>(ls, "CPPFileVersion");
	class_def<FileVersion>(ls, "GetFileSize", &FileVersion::GetFileSize);
	class_def<FileVersion>(ls, "IsDownLoadFileVersionNew", &FileVersion::IsDownLoadFileVersionNew);
	class_def<FileVersion>(ls, "GetRemoteAppVersion", &FileVersion::GetRemoteAppVersion);
	class_def<FileVersion>(ls, "GetRemoteSvnVersion", &FileVersion::GetRemoteSvnVersion);
	class_def<FileVersion>(ls, "GetLocalVersionMd5", &FileVersion::GetLocalVersionMd5);
	class_def<FileVersion>(ls, "GetFileSize", &FileVersion::GetFileSize);
	class_enum<FileVersion>(ls, "VS_INVALID", FileVersion::VS_INVALID);
	class_enum<FileVersion>(ls, "VS_INIT_NEW", FileVersion::VS_INIT_NEW);
	class_enum<FileVersion>(ls, "VS_INIT_EX_NEW", FileVersion::VS_INIT_EX_NEW);
	class_enum<FileVersion>(ls, "VS_DOWNLOAD_NEW", FileVersion::VS_DOWNLOAD_NEW);
	class_enum<FileVersion>(ls, "VS_SERVER_NEW", FileVersion::VS_SERVER_NEW);
	class_enum<FileVersion>(ls, "VS_NOT_EXIST", FileVersion::VS_NOT_EXIST);
	class_instance<FileVersion>(ls, &FileVersion::GetInstance);

	//ResManager
	class_add<ResManager>(ls, "CPPResManager");
	class_def<ResManager>(ls, "SetPlatAndResURL", &ResManager::SetPlatAndCdnURL);
	class_def<ResManager>(ls, "SetGameURL", &ResManager::SetGameURL);
	class_def<ResManager>(ls, "SetConnectTimeout", &ResManager::SetConnectTimeout);
	class_def<ResManager>(ls, "SetKeepAlive", &ResManager::SetKeepAlive);
	class_def<ResManager>(ls, "CallPlatApi", &ResManager::CallPlatApi);
	class_def<ResManager>(ls, "CallGameApi", &ResManager::CallGameApi);
	class_def<ResManager>(ls, "CallDirectApi", &ResManager::CallDirectApi);
	class_def<ResManager>(ls, "CallDirectApiEx", &ResManager::CallDirectApiEx);
	class_def<ResManager>(ls, "UpdateFileList", (bool(ResManager::*)(lua_tinker::table, ZQ::UINT32, ZQ::UINT32, ZQ::INT32))&ResManager::UpdateFileList);
	class_def<ResManager>(ls, "UpdateFileList", (bool(ResManager::*)(lua_tinker::table, ZQ::UINT32, ZQ::INT32, lua_tinker::table, lua_tinker::table))&ResManager::UpdateFileList);
	class_def<ResManager>(ls, "CancelUpdateFileList", &ResManager::CancelUpdateFileList);
	class_def<ResManager>(ls, "GetSetting", &ResManager::GetSetting);
	class_def<ResManager>(ls, "SetSetting", &ResManager::SetSetting);
	class_def<ResManager>(ls, "GetFileVersionStatus", &ResManager::GetFileVersionStatus);
	class_def<ResManager>(ls, "GetFileSize", &ResManager::GetFileSize);
	class_def<ResManager>(ls, "GetUpdateFileList", &ResManager::GetUpdateFileList);
	class_def<ResManager>(ls, "SendFile2Server", (bool(ResManager::*)(const std::string& url, const std::string& filePath, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out))&ResManager::SendFile2Server);
	class_def<ResManager>(ls, "SendFile2Server", (bool(ResManager::*)(const std::string& url, const char* fileData, unsigned int fileSize, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out))&ResManager::SendFile2Server);
	class_instance<ResManager>(ls, &ResManager::GetInstance);

	//ImageData
	class_add<ImageData>(ls, "CPPImageData");
	class_con<ImageData>(ls, lua_tinker::constructor<ImageData>);
	class_mem<ImageData>(ls, "x", &ImageData::x);
	class_mem<ImageData>(ls, "y", &ImageData::y);
	class_mem<ImageData>(ls, "width", &ImageData::width);
	class_mem<ImageData>(ls, "height", &ImageData::height);
	class_mem<ImageData>(ls, "offset_x", &ImageData::offset_x);
	class_mem<ImageData>(ls, "offset_y", &ImageData::offset_y);
	class_mem<ImageData>(ls, "image_path", &ImageData::image_path);
	class_mem<ImageData>(ls, "clip_image_name", &ImageData::clip_image_name);
	class_mem<ImageData>(ls, "scale_size", &ImageData::scale_size);

	//Installer
	class_add<Installer>(ls, "CPPInstaller");
	class_static<Installer>(ls, "GetNewPackInfo", &Installer::GetNewPackInfoForLua);
	class_static<Installer>(ls, "CheckHasDownload", &Installer::CheckHasDownload);
	class_static<Installer>(ls, "DownloadNewPack", &Installer::DownloadNewPackForLua);
	class_static<Installer>(ls, "CheckDownloadProgress", &Installer::CheckDownloadProgressForLua);
	class_static<Installer>(ls, "Install", &Installer::Install);
	class_static<Installer>(ls, "Cancle", &Installer::Cancle);
	class_static<Installer>(ls, "GotoDownloadUrl", &Installer::GotoDownloadUrl);

	//HttpProgressInfo
	class_add<HttpProgressInfo>(ls, "CPPHttpProgressInfo");
	class_mem<HttpProgressInfo>(ls, "ret", &HttpProgressInfo::ret);
	class_mem<HttpProgressInfo>(ls, "total_len", &HttpProgressInfo::total_len);
	class_mem<HttpProgressInfo>(ls, "cur_len", &HttpProgressInfo::cur_len);

	//MessageMem
	class_add<MessageMem>(ls, "CPPMessageMem");
	class_con<MessageMem>(ls, constructorEx<MessageMem>);
	class_def<MessageMem>(ls, "Destroy", &MessageMem::Destroy);

	//HttpManager
	class_add<HttpManager>(ls, "CPPHttpManager");
	class_def<HttpManager>(ls, "GetProgress", (HttpProgressInfo(HttpManager::*)(ZQ::UINT32))&HttpManager::GetProgress);
	class_def<HttpManager>(ls, "GetResourceConnNum", &HttpManager::GetResourceConnNum);
	class_instance<HttpManager>(ls, &HttpManager::GetInstance);

	//SoundManager
	class_add<SoundManager>(ls, "CPPSoundManager");
	class_def<SoundManager>(ls, "PreloadBGMusic", &SoundManager::PreloadBGMusic);
	class_def<SoundManager>(ls, "PlayBGMusic", &SoundManager::PlayBGMusic);
	class_def<SoundManager>(ls, "StopBGMusic", &SoundManager::StopBGMusic);
	class_def<SoundManager>(ls, "PauseBGMusic", &SoundManager::PauseBGMusic);
	class_def<SoundManager>(ls, "ResumeBGMusic", &SoundManager::ResumeBGMusic);
	class_def<SoundManager>(ls, "RewindBGMusic", &SoundManager::RewindBGMusic);
	class_def<SoundManager>(ls, "IsBGMusicPlaying", &SoundManager::IsBGMusicPlaying);
	class_def<SoundManager>(ls, "GetBGMusicVolume", &SoundManager::GetBGMusicVolume);
	class_def<SoundManager>(ls, "SetBGMusicVolume", &SoundManager::SetBGMusicVolume);
	class_def<SoundManager>(ls, "PlayEffect", &SoundManager::PlayEffect);
	class_def<SoundManager>(ls, "PauseEffect", &SoundManager::PauseEffect);
	class_def<SoundManager>(ls, "PauseAllEffects", &SoundManager::PauseAllEffects);
	class_def<SoundManager>(ls, "ResumeEffect", &SoundManager::ResumeEffect);
	class_def<SoundManager>(ls, "ResumeAllEffects", &SoundManager::ResumeAllEffects);
	class_def<SoundManager>(ls, "StopEffect", &SoundManager::StopEffect);
	class_def<SoundManager>(ls, "StopAllEffects", &SoundManager::StopAllEffects);
	class_def<SoundManager>(ls, "PreloadEffect", &SoundManager::PreloadEffect);
	class_def<SoundManager>(ls, "UnloadEffect", &SoundManager::UnloadEffect);
	class_def<SoundManager>(ls, "GetEffectsVolume", &SoundManager::GetEffectsVolume);
	class_def<SoundManager>(ls, "SetEffectsVolume", &SoundManager::SetEffectsVolume);
	class_def<SoundManager>(ls, "Release", &SoundManager::Release);
	class_def<SoundManager>(ls, "Stop", &SoundManager::Stop);
	class_instance<SoundManager>(ls, &SoundManager::GetInstance);

	//VoiceManager
	class_add<VoiceManager>(ls, "CPPVoiceManager");
	class_def<VoiceManager>(ls, "StartRecord", &VoiceManager::StartRecord);
	class_def<VoiceManager>(ls, "StopRecord", &VoiceManager::StopRecord);
	class_def<VoiceManager>(ls, "StartPlay", &VoiceManager::StartPlay);
	class_def<VoiceManager>(ls, "StopPlay", &VoiceManager::StopPlay);
	class_def<VoiceManager>(ls, "SaveMsgAsFile", &VoiceManager::SaveMsgAsFile);
	class_def<VoiceManager>(ls, "ReleaseVoice", &VoiceManager::ReleaseVoice);
	class_def<VoiceManager>(ls, "DeleteFileByVoiceId", &VoiceManager::DeleteFileByVoiceId);
	class_instance<VoiceManager>(ls, &VoiceManager::GetInstance);

	//Location
	class_add<Location>(ls, "CPPLocation");
	class_def<Location>(ls, "StartLocation", &Location::StartLocation);
	class_def<Location>(ls, "StopLocation", &Location::StopLocation);
	class_def<Location>(ls, "SetLocationTimeout", &Location::SetLocationTimeout);
	class_instance<Location>(ls, &Location::GetInstance);

	//MsgPush
	class_add<MsgPush>(ls, "CPPMsgPush");
	class_def<MsgPush>(ls, "StartMsgPush", &MsgPush::StartMsgPush);
	class_def<MsgPush>(ls, "StopMsgPush", &MsgPush::StopMsgPush);
	class_def<MsgPush>(ls, "ResumeMsgPush", &MsgPush::ResumeMsgPush);
	class_def<MsgPush>(ls, "GetApiKey", &MsgPush::GetApiKey);
	class_def<MsgPush>(ls, "SetTag", &MsgPush::SetTag);
	class_def<MsgPush>(ls, "GetAppInfo", &MsgPush::GetAppInfo);
	class_def<MsgPush>(ls, "SetTimerTask", &MsgPush::SetTimerTask);
	class_def<MsgPush>(ls, "GetActivityStatus", &MsgPush::GetActivityStatus);
	class_def<MsgPush>(ls, "CancelTimerTask", &MsgPush::CancelTimerTask);
	class_def<MsgPush>(ls, "SetAlias", &MsgPush::SetAlias);
	class_def<MsgPush>(ls, "DelegateSocket", &MsgPush::DelegateSocket);
	class_instance<MsgPush>(ls, &MsgPush::GetInstance);

	//SdkTools
	class_add<SdkTools>(ls, "CPPSdkTools");
	class_static<SdkTools>(ls, "GetSdkName", &SdkTools::GetSdkName);
	class_static<SdkTools>(ls, "GetChannelId", &SdkTools::GetChannelId);
	class_static<SdkTools>(ls, "GetChannelIdEx", &SdkTools::GetChannelIdEx);
	class_static<SdkTools>(ls, "GetAppId", &SdkTools::GetAppId);
	class_static<SdkTools>(ls, "GetAppKey", &SdkTools::GetAppKey);
	class_static<SdkTools>(ls, "GetSdkVersion", &SdkTools::GetSdkVersion);
	class_static<SdkTools>(ls, "GetStartImagePath", &SdkTools::GetStartImagePath);
	class_static<SdkTools>(ls, "GetPhpPayOrderUrl", &SdkTools::GetPhpTDOrderUrl);
	class_static<SdkTools>(ls, "GetPhpPayNotifyUrl", &SdkTools::GetPhpTDNotifyUrl);
	class_static<SdkTools>(ls, "GetLoginImagePath", &SdkTools::GetLoginImagePath);
	class_static<SdkTools>(ls, "IsThirdAccountPlat", &SdkTools::IsThirdAccountPlat);
	class_static<SdkTools>(ls, "GetUpdateUrlParams", &SdkTools::GetUpdateUrlParams);
	class_static<SdkTools>(ls, "IsInited", &SdkTools::IsInited);
	class_static<SdkTools>(ls, "CheckUpdate", &SdkTools::CheckUpdate);
	class_static<SdkTools>(ls, "LoginPanel", &SdkTools::LoginPanel);
	class_static<SdkTools>(ls, "LoginPannelWithParam", &SdkTools::LoginPannelWithParam);
	class_static<SdkTools>(ls, "Logout", &SdkTools::Logout);
	class_static<SdkTools>(ls, "SwitchAccount", &SdkTools::SwitchAccount);
	class_static<SdkTools>(ls, "Pay", &SdkTools::TD);
	class_static<SdkTools>(ls, "ToUserCenter", &SdkTools::ToUserCenter);
	class_static<SdkTools>(ls, "ExitSdk", &SdkTools::ExitSdk);
	class_static<SdkTools>(ls, "GotoBrowser", &SdkTools::GotoBrowser);
	class_static<SdkTools>(ls, "ShowDialog", (void(*)(const char *, const char *, const char *, const char *, ZQ::UINT32, ZQ::UINT32))&SdkTools::ShowDialog);
	class_static<SdkTools>(ls, "CommonHandle", &SdkTools::CommonHandle);
	class_static<SdkTools>(ls, "KeyDataHandle", &SdkTools::KeyDataHandle);
	class_static<SdkTools>(ls, "SetLuaMemoryWarnCallback", &SdkTools::SetLuaMemoryWarnCallback);
	class_static<SdkTools>(ls, "SetLuaInitCallback", &SdkTools::SetLuaInitCallback);
	class_static<SdkTools>(ls, "SetLuaLoginCallback", &SdkTools::SetLuaLoginCallback);
	class_static<SdkTools>(ls, "SetLuaPayCallback", &SdkTools::SetLuaTDCallback);
	class_static<SdkTools>(ls, "SetLuaCommonHandleCallback", &SdkTools::SetLuaCommonHandleCallback);
	class_static<SdkTools>(ls, "OpenFile", &SdkTools::OpenFile);
	class_static<SdkTools>(ls, "Install", &SdkTools::Install);
	class_static<SdkTools>(ls, "GetKeyDataChannelId", &SdkTools::GetKeyDataChannelId);

	//SystemTools
	class_add<SystemTools>(ls, "CPPSystemTools");
	class_enum<SystemTools>(ls, "NETWORK_NONE", SystemTools::NETWORK_NONE);
	class_enum<SystemTools>(ls, "NETWORK_WIFI", SystemTools::NETWORK_WIFI);
	class_enum<SystemTools>(ls, "NETWORK_MOBILE_2G", SystemTools::NETWORK_MOBILE_2G);
    class_enum<SystemTools>(ls, "NETWORK_MOBILE_3G", SystemTools::NETWORK_MOBILE_3G);
    class_enum<SystemTools>(ls, "NETWORK_MOBILE_4G", SystemTools::NETWORK_MOBILE_4G);
	class_enum<SystemTools>(ls, "NETWORK_UNKNOW", SystemTools::NETWORK_UNKNOW);
	class_static<SystemTools>(ls, "GetSystemType", &SystemTools::GetSystemType);
	class_static<SystemTools>(ls, "GetAppPath", &SystemTools::GetAppPath);
	class_static<SystemTools>(ls, "GetAppPackPath", &SystemTools::GetAppPackPath);
	class_static<SystemTools>(ls, "GetPackageName", &SystemTools::GetPackageName);
	class_static<SystemTools>(ls, "GetAppName", &SystemTools::GetAppName);
	class_static<SystemTools>(ls, "GetDeviceId", &SystemTools::GetDeviceId);
	class_static<SystemTools>(ls, "GetImsi", &SystemTools::GetImsi);
	class_static<SystemTools>(ls, "GetAppVersion", &SystemTools::GetAppVersion);
	class_static<SystemTools>(ls, "GetAppVersionCode", &SystemTools::GetAppVersionCode);
	class_static<SystemTools>(ls, "GetDeviceModel", &SystemTools::GetDeviceModel);
	class_static<SystemTools>(ls, "IsOpenLocationService", &SystemTools::IsOpenLocationService);
	class_static<SystemTools>(ls, "GetMemorySize", &SystemTools::GetMemorySize);
	class_static<SystemTools>(ls, "GetNetWorkState", &SystemTools::GetNetWorkState);
	class_static<SystemTools>(ls, "GetIPAddress", &SystemTools::GetIPAddress);
	class_static<SystemTools>(ls, "GetMacAddress", &SystemTools::GetMacAddress);
	class_static<SystemTools>(ls, "IsApptorePackage", &SystemTools::IsApptorePackage);
	class_static<SystemTools>(ls, "GetIdentifierForAdvertisers", &SystemTools::GetIdentifierForAdvertisers);
	class_static<SystemTools>(ls, "GetIdentifierForVendor", &SystemTools::GetIdentifierForVendor);
	class_static<SystemTools>(ls, "IsAppInstalled", &SystemTools::IsAppInstalled);
	class_static<SystemTools>(ls, "IsNetworkAvailable", &SystemTools::IsNetworkAvailable);
	class_static<SystemTools>(ls, "CheckNetWork", &SystemTools::CheckNetWork);
	class_static<SystemTools>(ls, "SetNetworkStateChangeNotifyFunc", &SystemTools::SetNetworkStateChangeNotifyFunc);
	class_static<SystemTools>(ls, "GetCountryCode", &SystemTools::GetCountryCode);
	class_static<SystemTools>(ls, "GetCurrencyCode", &SystemTools::GetCurrencyCode);
	class_static<SystemTools>(ls, "ConvertUygur", &SystemTools::ConvertUygur);

	//TimerQuest
	class_add<TimerQuest>(ls, "CPPTimerQuest");
	class_con<TimerQuest>(ls, lua_tinker::constructor<TimerQuest>);
	class_def<TimerQuest>(ls, "Update", &TimerQuest::Update);
	class_def<TimerQuest>(ls, "AddDelayQuest", &TimerQuest::AddDelayQuest);
	class_def<TimerQuest>(ls, "AddPeriodQuest", &TimerQuest::AddPeriodQuest);
	class_def<TimerQuest>(ls, "AddRunTimesQuest", &TimerQuest::AddRunTimesQuest);
	class_def<TimerQuest>(ls, "CancelQuest", &TimerQuest::CancelQuest);
	class_def<TimerQuest>(ls, "PauseQuest", &TimerQuest::PauseQuest);
	class_def<TimerQuest>(ls, "ResumeQuest", &TimerQuest::ResumeQuest);
	class_def<TimerQuest>(ls, "Pause", &TimerQuest::Pause);
	class_def<TimerQuest>(ls, "Resume", &TimerQuest::Resume);
	class_def<TimerQuest>(ls, "Stop", &TimerQuest::Stop);

	//ViewHelper
	class_add<ViewHelper>(ls, "CPPViewHelper");
	class_static<ViewHelper>(ls, "LoadImageResource", &ViewHelper::LoadImageResource);
	class_static<ViewHelper>(ls, "ReleaseImageResource", &ViewHelper::ReleaseImageResource);
	class_static<ViewHelper>(ls, "LoadAsyncImageResource", (void(*)(const std::string&, ZQ::UINT32))&ViewHelper::LoadAsyncImageResource);
	class_static<ViewHelper>(ls, "GetImageSuffix", &ViewHelper::GetImageSuffix);
	class_static<ViewHelper>(ls, "UnzipImageFile", &ViewHelper::UnzipImageFile);
	class_static<ViewHelper>(ls, "GetSoundSuffix", &ViewHelper::GetSoundSuffix);
	class_static<ViewHelper>(ls, "PrintAllTextureCache", &ViewHelper::PrintAllTextureCache);
	class_static<ViewHelper>(ls, "ReleaseUnusedImageResource", &ViewHelper::ReleaseUnusedImageResource);
	class_static<ViewHelper>(ls, "ReleaseUnusedImageExcept", &ViewHelper::ReleaseUnusedImageExcept);
	class_static<ViewHelper>(ls, "GetCachedTextureBytes", &ViewHelper::GetCachedTextureBytes);
	class_static<ViewHelper>(ls, "RecordCurTextureInfo", &ViewHelper::RecordCurTextureInfo);
	class_static<ViewHelper>(ls, "PrintTextureChange", &ViewHelper::PrintTextureChange);
	class_static<ViewHelper>(ls, "GetResoursePath", &ViewHelper::GetResoursePath);
	class_static<ViewHelper>(ls, "GetMusicSuffix", &ViewHelper::GetMusicSuffix);
	class_static<ViewHelper>(ls, "GetSkeletonSuffix", &ViewHelper::GetSkeletonSuffix);
	class_static<ViewHelper>(ls, "IsImageExistInCache", &ViewHelper::IsImageExistInCache);
	class_static<ViewHelper>(ls, "LoadWidgetImageResource", &ViewHelper::LoadWidgetImageResource);
	class_static<ViewHelper>(ls, "ReleaseWidgetImageResource", &ViewHelper::ReleaseWidgetImageResource);
	class_static<ViewHelper>(ls, "CreateWidgetFromJsonFile", &ViewHelper::CreateWidgetFromJsonFile);
	class_static<ViewHelper>(ls, "CreateWidgetFromZJsonFile", &ViewHelper::CreateWidgetFromZJsonFile);
	class_static<ViewHelper>(ls, "CreateWidgetFromBinaryFile", &ViewHelper::CreateWidgetFromBinaryFile);
	class_static<ViewHelper>(ls, "CreateWidgetFromFlatBuffersFile", &ViewHelper::CreateWidgetFromFlatBuffersFile);
	class_static<ViewHelper>(ls, "CreateWidgetFromFlatBuffersFileAsync", &ViewHelper::CreateWidgetFromFlatBuffersFileAsync);
	class_static<ViewHelper>(ls, "CreateActionTimelineFromFlatBuffersFile", &ViewHelper::CreateActionTimelineFromFlatBuffersFile);
	class_static<ViewHelper>(ls, "CreateActionTimelineFromFlatBuffersFileAsync", &ViewHelper::CreateActionTimelineFromFlatBuffersFileAsync);
	class_static<ViewHelper>(ls, "GetDFFWidget", &ViewHelper::GetDFFWidget);
	class_static<ViewHelper>(ls, "GetDFFAction", &ViewHelper::GetDFFAction);

	//ActionManager
	class_add<ActionManager>(ls, "CPPActionManager");
	class_static<ActionManager>(ls, "MoveBy", (void(*)(cocos2d::Node*, float, float, float, ZQ::UINT32))&ActionManager::MoveBy);
	class_static<ActionManager>(ls, "MoveBy", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::MoveBy);
	class_static<ActionManager>(ls, "MoveTo", (void(*)(cocos2d::Node*, float, float, float, ZQ::UINT32))&ActionManager::MoveTo);
	class_static<ActionManager>(ls, "MoveTo", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::MoveTo);
	class_static<ActionManager>(ls, "JumpBy", (void(*)(cocos2d::Node*, float, float, float, float, ZQ::UINT32, ZQ::UINT32))&ActionManager::JumpBy);
	class_static<ActionManager>(ls, "JumpBy", (void(*)(cocos2d::Node*, float, float, float, float, ZQ::UINT32))&ActionManager::JumpBy);
	class_static<ActionManager>(ls, "JumpTo", (void(*)(cocos2d::Node*, float, float, float, float, ZQ::UINT32, ZQ::UINT32))&ActionManager::JumpTo);
	class_static<ActionManager>(ls, "JumpTo", (void(*)(cocos2d::Node*, float, float, float, float, GameType::UINT32))&ActionManager::JumpTo);
	class_static<ActionManager>(ls, "Jump", (void(*)(cocos2d::Node*, float, float, float, GameType::UINT32, GameType::UINT32))&ActionManager::Jump);
	class_static<ActionManager>(ls, "Jump", (void(*)(cocos2d::Node*, float, float, float, GameType::UINT32))&ActionManager::Jump);
	class_static<ActionManager>(ls, "RotateBy", (void(*)(cocos2d::Node*, float, float, GameType::UINT32))&ActionManager::RotateBy);
	class_static<ActionManager>(ls, "RotateBy", (void(*)(cocos2d::Node*, float, float))&ActionManager::RotateBy);
	class_static<ActionManager>(ls, "RotateTo", (void(*)(cocos2d::Node*, float, float, GameType::UINT32))&ActionManager::RotateTo);
	class_static<ActionManager>(ls, "RotateTo", (void(*)(cocos2d::Node*, float, float))&ActionManager::RotateTo);
	class_static<ActionManager>(ls, "FadeIn", (void(*)(cocos2d::Node*, float, GameType::UINT32))&ActionManager::FadeIn);
	class_static<ActionManager>(ls, "FadeIn", (void(*)(cocos2d::Node*, float))&ActionManager::FadeIn);
	class_static<ActionManager>(ls, "FadeOut", (void(*)(cocos2d::Node*, float, GameType::UINT32))&ActionManager::FadeOut);
	class_static<ActionManager>(ls, "FadeOut", (void(*)(cocos2d::Node*, float))&ActionManager::FadeOut);
	class_static<ActionManager>(ls, "FadeTo", (void(*)(cocos2d::Node*, GLubyte, float))&ActionManager::FadeTo);
	class_static<ActionManager>(ls, "FadeTo", (void(*)(cocos2d::Node*, GLubyte, float, GameType::UINT32))&ActionManager::FadeTo);
	class_static<ActionManager>(ls, "ScaleTo", (void(*)(cocos2d::Node*, float, float))&ActionManager::ScaleTo);
	class_static<ActionManager>(ls, "ScaleTo", (void(*)(cocos2d::Node*, float, float, GameType::UINT32))&ActionManager::ScaleTo);
	class_static<ActionManager>(ls, "ScaleToXY", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::ScaleToXY);
	class_static<ActionManager>(ls, "ScaleToXY", (void(*)(cocos2d::Node*, float, float, float, ZQ::UINT32))&ActionManager::ScaleToXY);
	class_static<ActionManager>(ls, "SkewByXY", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::SkewByXY);
	class_static<ActionManager>(ls, "SkewByXY", (void(*)(cocos2d::Node*, float, float, float, ZQ::UINT32))&ActionManager::SkewByXY);
	class_static<ActionManager>(ls, "SkewToXY", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::SkewToXY);
	class_static<ActionManager>(ls, "SkewToXY", (void(*)(cocos2d::Node*, float, float, float, ZQ::UINT32))&ActionManager::SkewToXY);
	class_static<ActionManager>(ls, "DelayTo", (void(*)(cocos2d::Node*, float, ZQ::UINT32))&ActionManager::DelayTo);
	class_static<ActionManager>(ls, "Rollover", (void(*)(cocos2d::Node*, cocos2d::Node*, float, ZQ::UINT32))&ActionManager::Rollover);
	class_static<ActionManager>(ls, "ShakeLoop", (void(*)(cocos2d::Node*, float, float, float))&ActionManager::ShakeLoop);
	class_static<ActionManager>(ls, "ShakeLoop", (void(*)(cocos2d::Node*, float, float, float, GameType::UINT32))&ActionManager::ShakeLoop);
	class_static<ActionManager>(ls, "ProgressTo", (void(*)(cocos2d::Node*, float, float))&ActionManager::ProgressTo);
	class_static<ActionManager>(ls, "ProgressTo", (void(*)(cocos2d::Node*, float, float, ZQ::UINT32))&ActionManager::ProgressTo);
	class_static<ActionManager>(ls, "StopAllActions", (void(*)(cocos2d::Node*))&ActionManager::StopAllActions);
	class_static<ActionManager>(ls, "PauseAction", &ActionManager::PauseAction);
	class_static<ActionManager>(ls, "ResumeAction", &ActionManager::ResumeAction);
	class_static<ActionManager>(ls, "BezierBy", &ActionManager::BezierBy);
	class_static<ActionManager>(ls, "BezierTo", &ActionManager::BezierTo);

	//SceneManager
	class_add<SceneManager>(ls, "CPPSceneManager");
	class_def<SceneManager>(ls, "GetResolutionPolicy", &SceneManager::GetResolutionPolicy);
	class_def<SceneManager>(ls, "GetDesignSize", &SceneManager::GetDesignSize);
	class_def<SceneManager>(ls, "GetScreenSize", &SceneManager::GetScreenSize);
	class_def<SceneManager>(ls, "GetVisibleRect", &SceneManager::GetVisibleRect);
	class_def<SceneManager>(ls, "GetSafeAreaRect", &SceneManager::GetSafeAreaRect);
	class_def<SceneManager>(ls, "RunWithScene", &SceneManager::RunWithScene);
	class_def<SceneManager>(ls, "ReplaceScene", &SceneManager::ReplaceScene);
	class_def<SceneManager>(ls, "PushScene", &SceneManager::PushScene);
	class_def<SceneManager>(ls, "PopScene", &SceneManager::PopScene);
	class_def<SceneManager>(ls, "GetRunningScene", &SceneManager::GetRunningScene);
	class_def<SceneManager>(ls, "ShowMessage", &SceneManager::ShowMessage);
	class_def<SceneManager>(ls, "SetEnableTouchEvent", &SceneManager::SetEnableTouchEvent);
	class_def<SceneManager>(ls, "SetLuaBackKey", &SceneManager::SetLuaBackKey);
	class_def<SceneManager>(ls, "SetLuaMenuKey", &SceneManager::SetLuaMenuKey);
	class_def<SceneManager>(ls, "SetKeypadEnabled", &SceneManager::SetKeypadEnabled);
	class_def<SceneManager>(ls, "SetGameFrame", &SceneManager::SetGameFrame);
	class_def<SceneManager>(ls, "GetGameFrame", &SceneManager::GetGameFrame);
	class_def<SceneManager>(ls, "SetLuaTouchDown", &SceneManager::SetLuaTouchDown);
	class_def<SceneManager>(ls, "SetLuaTouchMove", &SceneManager::SetLuaTouchMove);
	class_def<SceneManager>(ls, "SetLuaTouchUp", &SceneManager::SetLuaTouchUp);
	class_def<SceneManager>(ls, "SetLuaTouchCancel", &SceneManager::SetLuaTouchCancel);
	class_def<SceneManager>(ls, "SetTouchSwallow", &SceneManager::SetTouchSwallow);
	class_def<SceneManager>(ls, "SetDeltaSpeedTime", &SceneManager::SetDeltaSpeedTime);
	class_def<SceneManager>(ls, "GetDeltaSpeedTime", &SceneManager::GetDeltaSpeedTime);
	class_enum<SceneManager>(ls, "SRP_EXACT_FIT", (int)ResolutionPolicy::EXACT_FIT);
	class_enum<SceneManager>(ls, "SRP_NO_BORDER", (int)ResolutionPolicy::NO_BORDER);
	class_enum<SceneManager>(ls, "SRP_SHOW_ALL", (int)ResolutionPolicy::SHOW_ALL);
	class_enum<SceneManager>(ls, "SRP_FIXED_HEIGHT", (int)ResolutionPolicy::FIXED_HEIGHT);
	class_enum<SceneManager>(ls, "SRP_FIXED_WIDTH", (int)ResolutionPolicy::FIXED_WIDTH);
	class_enum<SceneManager>(ls, "SRP_UNKNOWN", (int)ResolutionPolicy::UNKNOWN);
	class_enum<SceneManager>(ls, "SCREEN_DESIGN_WIDTH_DEF", (int)GameType::SCREEN_DESIGN_WIDTH_DEF);
	class_enum<SceneManager>(ls, "SCREEN_DESIGN_HEIGHT_DEF", (int)GameType::SCREEN_DESIGN_HEIGHT_DEF);
	class_instance<SceneManager>(ls, &SceneManager::GetInstance);

	//NodeUnity
	class_add<NodeUnity>(ls, "CPPNodeUnity");
	class_static<NodeUnity>(ls, "ConvertToUISpace", &NodeUnity::ConvertToUISpace);
	class_static<NodeUnity>(ls, "ConvertToNodePosition", &NodeUnity::ConvertToNodePosition);
	class_static<NodeUnity>(ls, "ConvertToWorldPosition", &NodeUnity::ConvertToWorldPosition);
	class_static<NodeUnity>(ls, "GetWorldPosition", &NodeUnity::GetWorldPosition);
	class_static<NodeUnity>(ls, "Retain", &NodeUnity::Retain);
	class_static<NodeUnity>(ls, "Release", &NodeUnity::Release);
	class_static<NodeUnity>(ls, "SetGrayShader", &NodeUnity::SetGrayShader);
	class_static<NodeUnity>(ls, "SetBrightValue", &NodeUnity::SetBrightValue);
	class_static<NodeUnity>(ls, "SetBrightShader", &NodeUnity::SetBrightShader);
	class_static<NodeUnity>(ls, "GetBrightValue", &NodeUnity::GetBrightValue);
	class_static<NodeUnity>(ls, "Resume", &NodeUnity::Resume);
	class_static<NodeUnity>(ls, "Pause", &NodeUnity::Pause);
	class_static<NodeUnity>(ls, "IsNodeEqual", &NodeUnity::IsNodeEqual);

	//GameScene
	class_add<GameScene>(ls, "CPPGameScene");
	class_inh<GameScene, cocos2d::Node>(ls);
	class_def<GameScene>(ls, "AddLayer", (void (GameScene::*) (GameLayer*))&GameScene::AddLayer);
	class_def<GameScene>(ls, "AddLayer", (void (GameScene::*) (GameLayer*, ZQ::INT32, ZQ::INT32))&GameScene::AddLayer);
	class_def<GameScene>(ls, "RemoveLayer", (void (GameScene::*) (GameLayer*, bool))&GameScene::RemoveLayer);
	class_def<GameScene>(ls, "RemoveLayer", (void (GameScene::*) (ZQ::INT32, bool))&GameScene::RemoveLayer);
	class_def<GameScene>(ls, "RemoveAllLayer", &GameScene::RemoveAllLayer);
	class_def<GameScene>(ls, "IsObjectActive", &GameScene::IsObjectActive);
	class_static<GameScene>(ls, "Node", &GameScene::Node);

	//GameLayer
	class_add<GameLayer>(ls, "CPPGameLayer");
	class_inh<GameLayer, cocos2d::Node>(ls);
	class_def<GameLayer>(ls, "SetVisible", &GameLayer::SetVisible);
	class_def<GameLayer>(ls, "SetTouchEnabled", &GameLayer::setTouchEnabled);
	class_def<GameLayer>(ls, "SetTouchRect", &GameLayer::SetTouchRect);
	class_def<GameLayer>(ls, "SetPosition", &GameLayer::SetPosition);
	class_def<GameLayer>(ls, "SetAnchorPoint", &GameLayer::SetAnchorPoint);
	class_def<GameLayer>(ls, "SetIsScissor", &GameLayer::SetIsScissor);
	class_def<GameLayer>(ls, "GetIsScissor", &GameLayer::GetIsScissor);
	class_def<GameLayer>(ls, "SetScissorSection", &GameLayer::SetScissorSection);
	class_def<GameLayer>(ls, "SetScissorSectionByLayer", &GameLayer::SetScissorSectionByLayer);
	class_def<GameLayer>(ls, "AddChild", (void(GameLayer::*)(cocos2d::Node*))&GameLayer::AddChild);
	class_def<GameLayer>(ls, "AddChild", (void(GameLayer::*)(cocos2d::Node*, ZQ::INT32, ZQ::INT32))&GameLayer::AddChild);
	class_def<GameLayer>(ls, "RemoveChild", (void(GameLayer::*)(ZQ::INT32, bool))&GameLayer::RemoveChild);
	class_def<GameLayer>(ls, "RemoveChild", (void(GameLayer::*)(cocos2d::Node*, bool))&GameLayer::RemoveChild);
	class_def<GameLayer>(ls, "RemoveAllChild", &GameLayer::RemoveAllChild);
	class_def<GameLayer>(ls, "RemoveSelf", &GameLayer::RemoveSelf);
	class_def<GameLayer>(ls, "SortAllChildByPos", &GameLayer::SortAllChildByPos);
	class_def<GameLayer>(ls, "SetLuaTouchDown", &GameLayer::SetLuaTouchDown);
	class_def<GameLayer>(ls, "SetLuaTouchMove", &GameLayer::SetLuaTouchMove);
	class_def<GameLayer>(ls, "SetLuaTouchUp", &GameLayer::SetLuaTouchUp);
	class_def<GameLayer>(ls, "SetEventListener", &GameLayer::SetLuaCallBack);
	class_def<GameLayer>(ls, "SetZOrder", &GameLayer::SetZOrder);
	class_def<GameLayer>(ls, "SetScaleFactor", &GameLayer::SetScaleFactor);
	class_def<GameLayer>(ls, "SetLuaUpdate", &GameLayer::SetLuaUpdate);
	class_def<GameLayer>(ls, "SetTouchSwallow", &GameLayer::SetSwallow);
	class_def<GameLayer>(ls, "GetWidth", &GameLayer::GetWidth);
	class_def<GameLayer>(ls, "GetHeight", &GameLayer::GetHeight);
	class_def<GameLayer>(ls, "SetTouchPriority", &GameLayer::SetTouchPriority);
	class_def<GameLayer>(ls, "GetTouchPriority", &GameLayer::GetTouchPriority);
	class_def<GameLayer>(ls, "SetOnListener", &GameLayer::SetOnListener);
	class_static<GameLayer>(ls, "Node", &GameLayer::Node);

	//ImageSprite
	class_add<ImageSprite>(ls, "CPPImageSprite");
	class_inh<ImageSprite, cocos2d::Node>(ls);
	class_def<ImageSprite>(ls, "SetPosition", &ImageSprite::SetPosition);
	class_def<ImageSprite>(ls, "AddSpriteState", &ImageSprite::AddSpriteState);
	class_def<ImageSprite>(ls, "AddSpriteStateImage", &ImageSprite::AddSpriteStateImage);
	class_def<ImageSprite>(ls, "AddSpriteStatePath", &ImageSprite::AddSpriteStatePath);
	class_def<ImageSprite>(ls, "AddSpriteStateData", &ImageSprite::AddSpriteStateData);
	class_def<ImageSprite>(ls, "AddSpriteStateIndex", &ImageSprite::AddSpriteStateIndex);
	class_def<ImageSprite>(ls, "ClearSpriteStateIndex", &ImageSprite::ClearSpriteStateIndex);
	class_def<ImageSprite>(ls, "IsSpriteStateExist", &ImageSprite::IsSpriteStateExist);
	class_def<ImageSprite>(ls, "SetSpriteState", &ImageSprite::SetSpriteState);
	class_def<ImageSprite>(ls, "SetFlipX", &ImageSprite::SetFlipX);
	class_def<ImageSprite>(ls, "SetFlipY", &ImageSprite::SetFlipY);
	class_def<ImageSprite>(ls, "SetVisible", &ImageSprite::SetVisible);
	class_def<ImageSprite>(ls, "SetZOrder", &ImageSprite::SetZOrder);
	class_def<ImageSprite>(ls, "SetLuaPlayOnceFun", &ImageSprite::SetLuaPlayOnceFun);
	class_def<ImageSprite>(ls, "SetAnchorPoint", &ImageSprite::SetAnchorPoint);
	class_def<ImageSprite>(ls, "IsPlayOnce", &ImageSprite::IsPlayOnce);
	class_def<ImageSprite>(ls, "GetFrameIndex", &ImageSprite::GetFrameIndex);
	class_def<ImageSprite>(ls, "SetFrameIndex", &ImageSprite::SetFrameIndex);
	class_def<ImageSprite>(ls, "SetScaleFactor", &ImageSprite::setScale);
	class_def<ImageSprite>(ls, "SetScaleXFactor", &ImageSprite::setScaleX);
	class_def<ImageSprite>(ls, "SetScaleYFactor", &ImageSprite::setScaleY);
	class_def<ImageSprite>(ls, "SetHighLight", &ImageSprite::SetHighLight);
	class_def<ImageSprite>(ls, "SetGray", &ImageSprite::SetGray);
	class_def<ImageSprite>(ls, "SetShadow", &ImageSprite::SetShadow);
	class_def<ImageSprite>(ls, "AddShadowOffset", &ImageSprite::AddShadowOffset);
	class_def<ImageSprite>(ls, "SetShadowInfo", &ImageSprite::SetShadowInfo);
	class_def<ImageSprite>(ls, "GetSpriteState", &ImageSprite::GetSpriteState);
	class_def<ImageSprite>(ls, "GetWidth", &ImageSprite::GetWidth);
	class_def<ImageSprite>(ls, "GetHeight", &ImageSprite::GetHeight);
	class_def<ImageSprite>(ls, "ResetUpdateDelay", &ImageSprite::ResetUpdateDelay);
	class_static<ImageSprite>(ls, "Node", &ImageSprite::Node);
	class_static<ImageSprite>(ls, "CreateWithImage", (ImageSprite* (*)(const std::string&, ZQ::INT32, ZQ::INT32, float))&ImageSprite::CreateWithImage);
	class_static<ImageSprite>(ls, "CreateWithImagePath", (ImageSprite* (*)(const std::string&, ZQ::INT32, ZQ::INT32, float))&ImageSprite::CreateWithImagePath);
	class_static<ImageSprite>(ls, "CreateWithImageData", (ImageSprite* (*)(const ImageData&, ZQ::INT32, ZQ::INT32, float))&ImageSprite::CreateWithImageData);

	//RichText
	class_add<RichText>(ls, "CPPRichText");
	class_inh<RichText, cocos2d::Node>(ls);
	class_def<RichText>(ls, "SetText", &RichText::SetText);
	class_def<RichText>(ls, "GetText", &RichText::GetText);
	class_def<RichText>(ls, "SetFontName", &RichText::SetFontName);
	class_def<RichText>(ls, "SetFontSize", &RichText::SetFontSize);
	class_def<RichText>(ls, "SetTextWidth", &RichText::SetTextWidth);
	class_def<RichText>(ls, "SetTextHeight", &RichText::SetTextHeight);
	class_def<RichText>(ls, "IsHeightFixed", &RichText::IsHeightFixed);
	class_def<RichText>(ls, "ScrollToStart", &RichText::ScrollToStart);
	class_def<RichText>(ls, "ScrollToBottom", &RichText::ScrollToBottom);
	class_def<RichText>(ls, "SetImageData", &RichText::SetImageData);
	class_def<RichText>(ls, "GetContentWidth", &RichText::GetContentWidth);
	class_def<RichText>(ls, "GetContentHeight", &RichText::GetContentHeight);
	class_def<RichText>(ls, "SetLuaTouchLink", &RichText::SetLuaTouchLink);
	class_def<RichText>(ls, "GetFontSpacing", &RichText::GetFontSpacing);
	class_def<RichText>(ls, "SetFontSpacing", &RichText::SetFontSpacing);
	class_def<RichText>(ls, "SetHorizonFontSpace", &RichText::SetHorizonFontSpace);
	class_def<RichText>(ls, "GetHorizonFontSpcae", &RichText::GetHorizonFontSpcae);
	class_def<RichText>(ls, "SetTouchSwallow", &RichText::SetSwallow);
	class_def<RichText>(ls, "SetTouchEnabled", &RichText::SetTouchEnabled);
	class_def<RichText>(ls, "GetRowNum", &RichText::GetRowNum);
	class_def<RichText>(ls, "SetReadingOrder", &RichText::SetReadingOrder);
	class_static<RichText>(ls, "Node", &RichText::Node);
	class_static<RichText>(ls, "CreateRichText", (RichText*(*)(int, int, const std::string&, int))&RichText::CreateRichText);
	class_static<RichText>(ls, "SetDefaultOutlineColor", (void(*)(unsigned int))&RichText::SetDefaultOutlineColor);
	class_static<RichText>(ls, "SetDefaultShadowColor", (void(*)(unsigned int))&RichText::SetDefaultShadowColor);
	class_static<RichText>(ls, "SetNewlineChar", (void(*)(const std::string&))&RichText::SetNewlineChar);
	class_static<RichText>(ls, "GetNewlineChar", (std::string*(*)(void(*)))&RichText::GetNewlineChar);
	class_enum<RichText>(ls, "RTReadingOrderLTR", (int)RichItemParser::ReadingOrder::RO_LTR);
	class_enum<RichText>(ls, "RTReadingOrderRTL", (int)RichItemParser::ReadingOrder::RO_RTL);

	//EditText
	class_add<EditTextEx>(ls, "CPPEditText");
	class_inh<EditTextEx, cocos2d::Node>(ls);
	class_def<EditTextEx>(ls, "SetMaxLength", &EditTextEx::SetMaxLength);
	class_def<EditTextEx>(ls, "SetMaxShowLength", &EditTextEx::SetMaxShowLength);
	class_def<EditTextEx>(ls, "SetPosition", &EditTextEx::SetPosition);
	class_def<EditTextEx>(ls, "SetText", &EditTextEx::SetText);
	class_def<EditTextEx>(ls, "SetFontName", &EditTextEx::SetFontName);
	class_def<EditTextEx>(ls, "SetFontSize", &EditTextEx::SetFontSize);
	class_def<EditTextEx>(ls, "SetContentSize", &EditTextEx::SetContentSize);
	class_def<EditTextEx>(ls, "GetText", &EditTextEx::GetText);
	class_def<EditTextEx>(ls, "SetColor", &EditTextEx::SetColor);
	class_def<EditTextEx>(ls, "SetOpacity", &EditTextEx::SetOpacity);
	class_def<EditTextEx>(ls, "SetHolderText", &EditTextEx::SetHolderText);
	class_def<EditTextEx>(ls, "SetPlaceHolderColor", &EditTextEx::SetPlaceHolderColor);
	class_def<EditTextEx>(ls, "SetPasswordMode", &EditTextEx::SetPasswordMode);
	class_def<EditTextEx>(ls, "SetKeyBoardWillShowCallBack", &EditTextEx::SetKeyBoardWillShowCallBack);
	class_def<EditTextEx>(ls, "SetKeyBoardDidShowCallBack", &EditTextEx::SetKeyBoardDidShowCallBack);
	class_def<EditTextEx>(ls, "SetKeyBoardWillHideCallBack", &EditTextEx::SetKeyBoardWillHideCallBack);
	class_def<EditTextEx>(ls, "SetKeyBoardDidHideCallBack", &EditTextEx::SetKeyBoardDidHideCallBack);
	class_def<EditTextEx>(ls, "SetAnchorPoint", &EditTextEx::SetAnchorPoint);
	class_def<EditTextEx>(ls, "SetFilterWords", &EditTextEx::SetFilterWords);
	class_def<EditTextEx>(ls, "SetTextChangedCallBack", &EditTextEx::SetTextChangedCallBack);
	class_def<EditTextEx>(ls, "SetTouchSwallow", &EditTextEx::SetSwallow);
	class_def<EditTextEx>(ls, "DetachWithIME", &EditTextEx::detachWithIME);
	class_def<EditTextEx>(ls, "SetExpand", &EditTextEx::SetExpand);
	class_static<EditTextEx>(ls, "Node", &EditTextEx::Node);
	class_static<EditTextEx>(ls, "CreateEditText", (EditTextEx*(*)(const std::string&, float, float, const std::string&, float))&EditTextEx::CreateEditText);

	//GraphicLabel
	class_add<GraphicLabel>(ls, "CPPGraphicLabel");
	class_inh<GraphicLabel, cocos2d::CCNode>(ls);
	class_def<GraphicLabel>(ls, "DrawSolidRect", &GraphicLabel::DrawSolidRect);
	class_def<GraphicLabel>(ls, "DrawSolidCircle", &GraphicLabel::DrawSolidCircle);
	class_def<GraphicLabel>(ls, "DrawSolidTriangle", &GraphicLabel::DrawSolidTriangle);
	class_def<GraphicLabel>(ls, "DrawLine", &GraphicLabel::DrawLine);
	class_def<GraphicLabel>(ls, "DrawRect", &GraphicLabel::DrawRect);
	class_def<GraphicLabel>(ls, "DrawCircle", &GraphicLabel::DrawCircle);
	class_def<GraphicLabel>(ls, "SetPosition", &GraphicLabel::SetPosition);
	class_def<GraphicLabel>(ls, "SetAnchorPoint", &GraphicLabel::SetAnchorPoint);
	class_def<GraphicLabel>(ls, "SetOpacity", &GraphicLabel::setOpacity);
	class_def<GraphicLabel>(ls, "SetScaleFactor", &GraphicLabel::SetScaleFactor);
	class_def<GraphicLabel>(ls, "SetVisible", &GraphicLabel::SetVisible);
	class_def<GraphicLabel>(ls, "Clear", &GraphicLabel::Clear);
	class_static<GraphicLabel>(ls, "Node", &GraphicLabel::Node);

	//ProgressControl
	class_add<ProgressControl>(ls, "CPPProgressControl");
	class_inh<ProgressControl, cocos2d::Node>(ls);
	class_def<ProgressControl>(ls, "SetProgressMax", &ProgressControl::SetProgressMax);
	class_def<ProgressControl>(ls, "AddProgress", &ProgressControl::AddProgress);
	class_def<ProgressControl>(ls, "SetProgress", &ProgressControl::SetProgress);
	class_def<ProgressControl>(ls, "SetProgressImage", (bool(ProgressControl::*)(const std::string&))&ProgressControl::SetProgressImage);
	class_def<ProgressControl>(ls, "SetProgressImage", (bool(ProgressControl::*)(const ImageData&))&ProgressControl::SetProgressImage);
	class_def<ProgressControl>(ls, "SetBkImage", (bool(ProgressControl::*)(const std::string&))&ProgressControl::SetBkImage);
	class_def<ProgressControl>(ls, "SetBkImage", (bool(ProgressControl::*)(const ImageData&))&ProgressControl::SetBkImage);
	class_def<ProgressControl>(ls, "InitWithImage", (bool(ProgressControl::*)(const std::string&, const std::string&))&ProgressControl::InitWithImage);
	class_def<ProgressControl>(ls, "InitWithImage", (bool(ProgressControl::*)(const ImageData&, const ImageData&))&ProgressControl::InitWithImage);
	class_static<ProgressControl>(ls, "Node", &ProgressControl::Node);
	class_static<ProgressControl>(ls, "CreateWithImage", (ProgressControl*(*)(const std::string&, const std::string&))&ProgressControl::CreateWithImage);
	class_static<ProgressControl>(ls, "CreateWithImage", (ProgressControl*(*)(const ImageData&, const ImageData&))&ProgressControl::CreateWithImage);

	//Progress9Control
	class_add<Progress9Control>(ls, "CPPProgress9Control");
	class_inh<Progress9Control, cocos2d::Node>(ls);
	class_def<Progress9Control>(ls, "SetProgressMax", &Progress9Control::SetProgressMax);
	class_def<Progress9Control>(ls, "AddProgress", &Progress9Control::AddProgress);
	class_def<Progress9Control>(ls, "SetProgress", &Progress9Control::SetProgress);
	class_def<Progress9Control>(ls, "SetProgressImage", (bool(Progress9Control::*)(const std::string&))&Progress9Control::SetProgressImage);
	class_def<Progress9Control>(ls, "SetProgressImage", (bool(Progress9Control::*)(const ImageData&))&Progress9Control::SetProgressImage);
	class_static<Progress9Control>(ls, "Node", &Progress9Control::Node);
	class_static<Progress9Control>(ls, "CreateWithImage", (Progress9Control*(*)(const std::string&))&Progress9Control::CreateWithImage);
	class_static<Progress9Control>(ls, "CreateWithImage", (Progress9Control*(*)(const ImageData&))&Progress9Control::CreateWithImage);

	//GraphicRender
	class_add<GraphicRender>(ls, "CPPGraphicRender");
	class_inh<GraphicRender, cocos2d::Node>(ls);
	class_def<GraphicRender>(ls, "GetImageLabel", &GraphicRender::GetImageLabel);
	class_def<GraphicRender>(ls, "Begin", &GraphicRender::Begin);
	class_def<GraphicRender>(ls, "BeginWithClear", (void(GraphicRender::*)(float, float, float, float))&GraphicRender::BeginWithClear);
	class_def<GraphicRender>(ls, "End", &GraphicRender::End);
	class_def<GraphicRender>(ls, "SetAutoDraw", &GraphicRender::SetAutoDraw);
	class_def<GraphicRender>(ls, "IsAutoDraw", &GraphicRender::IsAutoDraw);
	class_def<GraphicRender>(ls, "SaveToFile", &GraphicRender::SaveToFile);
	class_static<GraphicRender>(ls, "Create", &GraphicRender::Create);
	class_static<GraphicRender>(ls, "CaptureScreen", &GraphicRender::CaptureScreen);

	//FontManager
	class_add<FontManager>(ls, "CPPFontManager");
	class_def<FontManager>(ls, "registerFont", &FontManager::registerFont);
	class_def<FontManager>(ls, "setFontSizeRange", &FontManager::setFontSizeRange);
	class_def<FontManager>(ls, "clearAllFonts", &FontManager::clearAllFonts);
	class_def<FontManager>(ls, "registerFilterChar", &FontManager::registerFilterChar);
	class_instance<FontManager>(ls, &FontManager::sharedFontManager);

	//BatchRendererManager
	class_add<BatchRendererManager>(ls, "CPPBatchRendererManager");
	class_def<BatchRendererManager>(ls, "setDirty", &BatchRendererManager::setDirty);
	class_instance<BatchRendererManager>(ls, &BatchRendererManager::sharedRendererManager);

	//FaceConvHelper
	class_add<FaceConvHelper>(ls, "CPPFaceConvHelper");
	class_def<FaceConvHelper>(ls, "registerFace", &FaceConvHelper::registerFace);
	class_def<FaceConvHelper>(ls, "convertToFaceStr", &FaceConvHelper::convertToFaceStr);
	class_instance<FaceConvHelper>(ls, &FaceConvHelper::sharedFaceConvHelper);

	//StringConvHelper
	class_add<StringConvHelper>(ls, "CPPStringConvHelper");
	class_static<StringConvHelper>(ls, "unicode_at", &StringConvHelper::unicode_at);
	class_static<StringConvHelper>(ls, "unicode_sub", &StringConvHelper::unicode_sub);
	class_static<StringConvHelper>(ls, "unicode_length", &StringConvHelper::unicode_length);

	//GameState
	class_add<GameState>(ls, "CPPGameState");
	class_def<GameState>(ls, "GetName", &GameState::GetName);
	class_def<GameState>(ls, "GetComposeType", &GameState::GetComposeType);
	class_def<GameState>(ls, "AddOutStateName", &GameState::AddOutStateName);
	class_def<GameState>(ls, "RemoveOutStateName", &GameState::RemoveOutStateName);
	class_def<GameState>(ls, "SetStateCanReEnter", &GameState::SetStateCanReEnter);
	class_def<GameState>(ls, "SetCallbackAsMultiFuctions", &GameState::SetCallbackAsMultiFuctions);

	//GameStateMachine
	class_add<GameStateMachine>(ls, "CPPGameStateMachine");
	class_con<GameStateMachine>(ls, lua_tinker::constructor<GameStateMachine>);
	class_def<GameStateMachine>(ls, "Update", &GameStateMachine::Update);
	class_def<GameStateMachine>(ls, "ChangeState", (bool(GameStateMachine::*)(const std::string&, bool))&GameStateMachine::ChangeState);
	class_def<GameStateMachine>(ls, "ChangeState", (bool(GameStateMachine::*)(GameType::UINT16, bool))&GameStateMachine::ChangeState);
	class_def<GameStateMachine>(ls, "ChangeStateForce", (bool(GameStateMachine::*)(const std::string))&GameStateMachine::ChangeStateForce);
	class_def<GameStateMachine>(ls, "ChangeStateForce", (bool(GameStateMachine::*)(GameType::UINT16))&GameStateMachine::ChangeStateForce);
	class_def<GameStateMachine>(ls, "IsInState", (bool(GameStateMachine::*)(const std::string&))&GameStateMachine::IsInState);
	class_def<GameStateMachine>(ls, "IsInState", (bool(GameStateMachine::*)(GameType::UINT16))&GameStateMachine::IsInState);
	class_def<GameStateMachine>(ls, "GetSinkRunState", (std::string(GameStateMachine::*)(const std::string&))&GameStateMachine::GetSinkRunState);
	class_def<GameStateMachine>(ls, "GetSinkRunState", (GameType::UINT16(GameStateMachine::*)(GameType::UINT16))&GameStateMachine::GetSinkRunState);
	class_def<GameStateMachine>(ls, "GetSinkPreviousState", (std::string(GameStateMachine::*)(const std::string&))&GameStateMachine::GetSinkPreviousState);
	class_def<GameStateMachine>(ls, "CreateSink", (bool(GameStateMachine::*)(const std::string&))&GameStateMachine::CreateSink);
	class_def<GameStateMachine>(ls, "CreateSink", (bool(GameStateMachine::*)(GameType::UINT16))&GameStateMachine::CreateSink);
	class_def<GameStateMachine>(ls, "CreateComposeState", (GameState*(GameStateMachine::*)(const std::string&, const std::string&))&GameStateMachine::CreateComposeState);
	class_def<GameStateMachine>(ls, "CreateComposeState", (GameState*(GameStateMachine::*)(GameType::UINT16, GameType::UINT16))&GameStateMachine::CreateComposeState);
	class_def<GameStateMachine>(ls, "CreateSubState", (GameState*(GameStateMachine::*)(const std::string&, const std::string&, const std::string&))&GameStateMachine::CreateSubState);
	class_def<GameStateMachine>(ls, "CreateSubState", (GameState*(GameStateMachine::*)(GameType::UINT16, GameType::UINT16, GameType::UINT16))&GameStateMachine::CreateSubState);
	class_def<GameStateMachine>(ls, "CreateNormalState", (GameState*(GameStateMachine::*)(const std::string&, const std::string&))&GameStateMachine::CreateNormalState);
	class_def<GameStateMachine>(ls, "CreateNormalState", (GameState*(GameStateMachine::*)(GameType::UINT16, GameType::UINT16))&GameStateMachine::CreateNormalState);

	//ParticleSprite
	class_add<ParticleSprite>(ls, "CPPParticleSprite");
	class_inh<ParticleSprite, cocos2d::Node>(ls);
	class_def<ParticleSprite>(ls, "SetPlayEndListener", &ParticleSprite::SetPlayEndListener);
	class_def<ParticleSprite>(ls, "SetCycle", &ParticleSprite::SetCycle);
	class_def<ParticleSprite>(ls, "SetScale", &ParticleSprite::SetScale);
	class_def<ParticleSprite>(ls, "SetFlipX", &ParticleSprite::SetFlipX);
	class_def<ParticleSprite>(ls, "SetFlipY", &ParticleSprite::SetFlipY);
	class_def<ParticleSprite>(ls, "SetEnabled", &ParticleSprite::SetEnabled);
	class_def<ParticleSprite>(ls, "SetMansualUpdate", &ParticleSprite::SetMansualUpdate);
	class_def<ParticleSprite>(ls, "SetTimeScale", &ParticleSprite::SetTimeScale);
	class_def<ParticleSprite>(ls, "GetTimeScale", &ParticleSprite::GetTimeScale);
	class_def<ParticleSprite>(ls, "SetFramePreSecond", &ParticleSprite::SetFramePreSecond);
	class_def<ParticleSprite>(ls, "GetSinceStartTime", &ParticleSprite::GetSinceStartTime);
	class_def<ParticleSprite>(ls, "GetContentSize", &ParticleSprite::GetContentSize);
	class_def<ParticleSprite>(ls, "MakeGrey", &ParticleSprite::MakeGrey);
	class_static<ParticleSprite>(ls, "Create", (ParticleSprite*(*)(void))&ParticleSprite::Create);
	class_static<ParticleSprite>(ls, "Create", (ParticleSprite*(*)(const char*))&ParticleSprite::Create);
	class_static<ParticleSprite>(ls, "CreateAsync", ParticleSprite::CreateAsync);
	class_static<ParticleSprite>(ls, "PreloadTemplates", &ParticleSprite::PreloadTemplates);
	class_static<ParticleSprite>(ls, "DeleteTemplates", &ParticleSprite::DeleteTemplates);

	//LuaProfileAgent
	class_add<LuaProfileAgent>(ls, "CPPLuaProfileAgent");
	class_enum<LuaProfileAgent>(ls, "LPT_UNKNOWN", (int)LuaProfile::LPT_UNKNOWN);
	class_enum<LuaProfileAgent>(ls, "LPT_PERFORMANCE_MONITOR", (int)LuaProfile::LPT_PERFORMANCE_MONITOR);
	class_enum<LuaProfileAgent>(ls, "LPT_LATEST_CALLFUNC_MONITOR", (int)LuaProfile::LPT_LATEST_CALLFUNC_MONITOR);
	class_def<LuaProfileAgent>(ls, "Start", &LuaProfileAgent::Start);
	class_def<LuaProfileAgent>(ls, "Stop", &LuaProfileAgent::Stop);
	class_instance<LuaProfileAgent>(ls, &LuaProfileAgent::GetInstance);
}
