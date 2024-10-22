#ifndef GAME_LAYER_H
#define GAME_LAYER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

#define LAYER_TOUCH_PRIORITY	-99

// ��ť�°��ص����� 
// out: ����ʱ����Ļ����
typedef void (cocos2d::Object::*LAYER_CALLBACK)(float, float);

class GameLayer : public cocos2d::Layer
{
public:
	enum ScissorType{
		NONE_SCISSOR = 0,			//�޲ü���
		SCREEN_SCISSOR,				//�������Ļ�ü�
		LAYER_SCISSOR				//����ڲ�ü�				
	};

	~GameLayer();

	//����һ���ղ�
	static GameLayer* Node();

	//�����Ƿ���ʾ
	//In: visible �Ƿ���ʾ����
	void SetVisible(bool visible);

	//��������
	//In: x y�ؼ�λ�� �����½Ƕ�λ
	void SetPosition(float x, float y);

	//���ò�Ĵ�����
	void SetTouchRect(float x, float y, float width, float height);

	//���òü��Ƿ���Ч
	void SetIsScissor(bool is_scissor){m_is_scirrsor = is_scissor;}
	bool GetIsScissor() {return m_is_scirrsor;}
	//���òü���
	void SetScissorSection(float x, float y, float width, float height);
	//���òü���By����߼�����
	void SetScissorSectionByLayer(float x, float y, float width, float height);

	//��ӿؼ�������
	//In: child��������Ŀؼ�
	//order:��Ļ���˳��order��ĻᵲסorderС��
	//tag:��ı�ʾ��
	void AddChild(cocos2d::Node *child);
	void AddChild(cocos2d::Node *child, ZQ::INT32 order, ZQ::INT32 tag);

	//�Ƴ��ӿؼ�
	//tag���ӿؼ��ı�ʶ
	//is_clean: �Ƿ����
	void RemoveChild(ZQ::INT32 tag, bool is_clean = true);
	void RemoveChild(cocos2d::Node *node, bool is_clean = true);

	//�Ƴ�ȫ�����ӿؼ�
	void RemoveAllChild(bool is_clean = true);

	//�Ӹ������Ƴ��Լ�
	void RemoveSelf(bool is_clean = true);

	//����������ӽڵ�����������
	void SortAllChildByPos(ZQ::UINT8 max_scene_width_power, ZQ::UINT8 max_scene_height_power);

	//����ZOrder
	//In:order �ؼ���������ı��
	void SetZOrder(ZQ::INT32 order);

	//���ñ���
	void SetScaleFactor(float factor);

	//���ڵ�ÿ֡�Զ����ã����Ա��������ӽڵ㲢����
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

	void Update(float dt);

	//���븸���c�r�{��
	virtual void onEnter();
	virtual void onExit();

	//���ö�λ��
	void SetAnchorPoint(float x, float y);

	//�����Ƿ����ɴ�����Ϣ
	void SetSwallow(bool swallow);
	void SetTouchCallBack(cocos2d::Ref *listener, LAYER_CALLBACK down_callback_fun, LAYER_CALLBACK move_callback_fun, LAYER_CALLBACK m_up_callback_fun);

	//  Lua�汾�Ĵ����ص�����
	void SetLuaTouchDown(ZQ::UINT32 down_call_fun_id){m_lua_down_fun_id = down_call_fun_id;}
	void SetLuaTouchMove(ZQ::UINT32 move_call_fun_id){m_lua_move_fun_id = move_call_fun_id;}
	void SetLuaTouchUp(ZQ::UINT32 up_call_fun_id){m_lua_up_fun_id = up_call_fun_id;}
	void SetLuaCallBack(ZQ::UINT32 down_call_fun_id, ZQ::UINT32 move_call_fun_id, ZQ::UINT32 up_call_fun_id);
	void SetLuaUpdate(ZQ::UINT32 update_fun_id, float interval = 0.0f);

	//�Ƿ�����Ч������
	bool IsValidTouchPos(cocos2d::Touch* touch);

	//�Ƿ�����Ч������
	bool IsValidDrawRect(cocos2d::Rect *rect);
	void SetOnListener(ZQ::UINT32 listener_id){ m_listener_id = listener_id; };

	//��ȡ��Ŀ��
	float GetWidth();
	float GetHeight();

	//���ô����ȼ�
	void SetTouchPriority(ZQ::INT32 priority);
	//��ȡ�����ȼ�
	ZQ::INT32 GetTouchPriority();

	//�Ƿ��пɼ����ڵ�
	bool HasVisibleParents();

protected:
	void onBeforeVisitScissor();
	void onAfterVisitScissor();
	//�̳���CCLayer, �������¼�
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

protected:
	GameLayer();
	bool IsListenerTouch();
	bool IsContainTouchLayer();
	inline ZQ::INT32 GetZOrderByPos(
		const cocos2d::Vec2& pos, ZQ::INT16 max_scene_width, ZQ::INT16 max_scene_height, ZQ::UINT8 max_scene_width_power)
	{
		float x = MIN(pos.x, max_scene_width); 
		x = MAX(-max_scene_width, x);
		float y = MIN(pos.y, max_scene_height); 
		y = MAX(-max_scene_height, y);
		ZQ::INT32 zorder = ((max_scene_height - (int)y) << max_scene_width_power) + (max_scene_width - (int)x);
		return zorder;
	}

private:
	cocos2d::Point		m_pre_point;			//�������϶�����һ��λ��
	ZQ::UINT32			m_lua_down_fun_id;		//������ͣ�����ڲ���ʱ�Ļص�
	ZQ::UINT32			m_lua_move_fun_id;		//�������ڲ��ϻ����Ļص�
	ZQ::UINT32			m_lua_up_fun_id;		//�������뿪��ʱ�Ļص�
	ZQ::UINT32			m_update_fun_id;		//lua���õĸ��º���
	ZQ::UINT32			m_listener_id;			//lua���õĸ��º���
	cocos2d::Rect		m_touch_rect;			//������
	ScissorType			m_scissor_type;			//�ü�������
	float				m_scissor_x;			//��Ĳü���
	float				m_scissor_y;			
	float				m_scissor_width;
	float				m_scissor_height;
	cocos2d::Rect m_scissor_rect;
	GLboolean m_gl_is_scissor;
	cocos2d::CustomCommand _beforeVisitCmdScissor;
	cocos2d::CustomCommand _afterVisitCmdScissor;

	bool				m_swallow;					//�Ƿ�������Ϣ
	bool				m_is_scirrsor;				//�ü����Ƿ���Ч
	cocos2d::Ref*		m_listener;					//�����¼�����
	LAYER_CALLBACK		m_down_callback_fun;		//�°���Ļص�����
	LAYER_CALLBACK		m_move_callback_fun;		//�ƶ�ʱ�Ļص�����
	LAYER_CALLBACK		m_up_callback_fun;			//��ס�ɿ���Ļص�����
};

#endif
