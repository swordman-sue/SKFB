#ifndef IMAGE_SPRITE_H
#define IMAGE_SPRITE_H

#include "cocos2d.h"
#include <map>
#include <deque>
#include "zqcommon/typedef.h"
#include "view/struct.h"

class ImageData;

#define DEFAULT_SHADOW_MAX		8		//��Ӱ֡��
#define DEFAULT_SHADOW_TIME		1.0		//��Ӱʱ��

//ԭ��ƫ��
typedef struct origin_offset
{
	float offset_x;
	float offset_y;
	float scale_size;
	origin_offset():offset_x(0.0), offset_y(0.0), scale_size(1.0){}
}ORIGINOFFSET;

//���鶯���ṹ
typedef struct sprite_state 
{
	ZQ::INT32 frame_count;												//����֡��
	float delay;														//ÿ֡���
	cocos2d::Vector<cocos2d::SpriteFrame*> frame_array;										//��������
	std::vector<ORIGINOFFSET> offset_array;								//����֡ƫ������
	sprite_state():frame_count(0),delay(0.0){} 
}SPRITESTATE;

//��Ӱ֡�Ķ���
typedef struct shadow_infomation
{
	float					x, y;			//����
	ZQ::UINT32				sprite_state;	//����״̬
	ZQ::UINT32				index;			//֡
	cocos2d::SpriteFrame	*frame;			//����֡
	bool					flip;			//�Ƿ�ת
}SHADOWINFO;

//��Ϸ�еľ���
class ImageSprite : public cocos2d::Sprite
{
public:
	enum SpriteEffect
	{
		EFFECT_NONE = 0,		//��Ч��
		EFFECT_HIGHLIGHT,		//����Ч��
		EFFECT_GREY,			//����Ч��
		EFFECT_SHADOW,			//��ӰЧ��
	};

	~ImageSprite();

	//  �����յľ���
	static ImageSprite* Node();

	// 	ʹ��ͼƬ��������
	// 	In: image_file ͼƬ·�� path ͼƬ�ļ���·�� ImageData �ü�ͼ����
	//	In: state_index ״̬��ʶ����һ����������и���״̬ �������� վ����) 
	// 	In: frame_count ֡��
	// 	In��ÿ֡���ż��ʱ�� 1.0Ϊһ��
	//������ͼƬ��ȡ
	static ImageSprite* CreateWithImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	//���ļ��ж�ȡ
	static ImageSprite* CreateWithImagePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	//��ƴͼ��ȡ
	static ImageSprite* CreateWithImageData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);

	//	Ϊ��������һ��״̬
	// 	In: image_file ͼƬ·�� image_data ͼƬ�������� path ͼƬ�ļ�·�� 
	//	In: state_index ״̬��ʶ����һ����������и���״̬ �������� վ����) 
	// 	In: frame_count ֡��
	// 	In��ÿ֡���ż��ʱ�� 1.0Ϊһ��
	bool AddSpriteStateImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStatePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStateData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);

	//����һ����״̬��ʹ��ƴͼ����״̬֡
	bool AddSpriteState(ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStateIndex(const ImageData &image_data, ZQ::INT32 state_index);

	//	���þ����״̬
	//	In: state_index ״̬��ʶ����һ����������и���״̬ �������� վ����) 
	void SetSpriteState(ZQ::INT32 state_index);
	//	��ȡ�����״̬
	ZQ::INT32 GetSpriteState();

	// ���õ�ǰ�Ĳ���֡
	void SetFrameIndex(unsigned int frame_index);

	//	��ȡ��ǰ�Ĳ���֡
	int GetFrameIndex(){return m_frame_index;}

	virtual void setPosition(const cocos2d::Vec2& pos);

	//���þ����λ��
	//In: x, y �ؼ�������
	void SetPosition(float x, float y);

	//��ȡ�Ƿ񲥷Ź�һ��
	bool IsPlayOnce(){return m_is_play_once;}

	//�����Ƿ�ˮƽ��ת
	//In: flip �Ƿ�ˮƽ��ת
	void SetFlipX(bool flip);

	//�����Ƿ�ˮƽ��ת
	//In: flip �Ƿ�����ת
	void SetFlipY(bool flip);

	//�����Ƿ�ɼ�
	//visible ���鶯���Ƿ�ɼ�
	void SetVisible(bool visible);

	//����ZOrder
	//In:order �ؼ���������ı��
	void SetZOrder(ZQ::INT32 order);
	
	//���ü���Ч��
	void SetHighLight(bool high_light);
	void SetBright(float value);

	//���ñ䰵Ч��
	void SetGray(bool is_grey);

	//���ù�ӰЧ��
	void SetShadow(bool shadow);
	//���ù�ӰЧ����Ӱ���Ⱥͼ��
	//shadow_time ��Ӱʱ��
	//frame_count ��Ӱ֡��
	void SetShadowInfo(float shadow_time, float frame_count);
	//���ӹ�Ӱƫ����
	void AddShadowOffset(float x, float y);

	//�����Ƿ��Զ�����
	void SetUpdate(bool is_update){ m_is_update = is_update;}

	//���ø���
	virtual void visit();

	//���ö�λ��
	void SetAnchorPoint(float x, float y);

	//�������ű���
	virtual void setScale(float scale_size);
	virtual void setScaleX(float x_scale_size);
	virtual void setScaleY(float y_scale_size);
	void ResetStateScaleFactor(float state_scale);

	//��ȡ���
	float GetWidth();
	float GetHeight();

	//���ò�����һ���Ļص�
	void SetLuaPlayOnceFun(ZQ::UINT32 lua_fun_id){m_lua_playonce_fun_id = lua_fun_id;}

	//��ѯ�����״̬�Ƿ����
	bool IsSpriteStateExist(ZQ::INT32 state_index);

	// ���ĳһ��״̬�µĶ���֡
	void ClearSpriteStateIndex(ZQ::INT32 state_index);

	// ���趯��Ƶ��
	void ResetUpdateDelay(ZQ::INT32 state_index, float delay_time);

	virtual void onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

protected:
	void Update(float time);
	void TrackShadow(float time);

protected:
	std::map<ZQ::INT32, SPRITESTATE>		m_sprite_map;				//��Ÿ���״̬������ӳ���
	ZQ::INT32								m_state;					//�����״̬
	ZQ::UINT32								m_frame_index;				//��ǰ����֡
	SPRITESTATE								m_sprite_date;				//����״̬����
	bool									m_is_play_once;				//�Ƿ񲥷Ź�һ��
	bool									m_is_update;				//�Ƿ��Զ�����
	bool									m_is_filp_x;				//�Ƿ�ˮƽ��ת
	bool									m_is_flip_y;				//�Ƿ�����ת
	ZQ::UINT32								m_lua_playonce_fun_id;		//����һ�����õĺ���
	float									m_x, m_y;					//��ǰλ��
	float									m_dx, m_dy;					//ƫ��λ��
	SpriteEffect							m_effect;					//��Ч
	std::deque<SHADOWINFO>					m_shadow_queue;				//��Ӱ�б�
	float									m_shadow_max;				//��Ӱ֡����
	float									m_shadow_time;				//��Ӱ֡�ܶ�
	float									m_scale_x;					//x�����ű���
	float									m_scale_y;					//y�����ű���
	float									m_state_scale_size;			//��ǰ״̬������֡
	bool									m_highlight_enable;
	float									m_highlight_value;
	cocos2d::GLProgram*						m_shader_program;

private:
	ImageSprite();
	void ResetOffsetPos();
};

#endif

