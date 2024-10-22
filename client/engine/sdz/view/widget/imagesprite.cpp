#include "imagesprite.h"
#include "luaengine/luaengine.h"
#include "view/struct.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/logutil.h"
#include "view/viewhelper.h"

using namespace cocos2d;

ImageSprite::ImageSprite()
	: m_is_update(true), m_lua_playonce_fun_id(0),m_x(0),m_y(0), m_dx(0), m_dy(0),m_is_filp_x(false),m_is_flip_y(false)
	, m_effect(EFFECT_NONE), m_shadow_max(DEFAULT_SHADOW_MAX), m_shadow_time(DEFAULT_SHADOW_TIME)
	, m_state_scale_size(1.0), m_state(-1), m_frame_index(0), m_scale_x(1.0), m_scale_y(1.0)
	, m_highlight_enable(false), m_highlight_value(1.6f), m_shader_program(nullptr)
{}

ImageSprite::~ImageSprite()
{
	std::map<ZQ::INT32, SPRITESTATE>::iterator iter = m_sprite_map.begin();

	while(iter != m_sprite_map.end())
	{
		iter->second.frame_array.clear();
		++iter;
		++iter;
	}

	m_sprite_map.clear();
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_playonce_fun_id);
}

//ʹ��ͼƬ��������
ImageSprite* ImageSprite::CreateWithImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	if("" == image_file)
	{	return NULL;}
	
	if(0 == frame_count)
	{
		frame_count = 1;
	}

	ImageSprite* image_sprite = new ImageSprite();
	if(image_sprite && image_sprite->init()
		&& image_sprite->AddSpriteStateImage(image_file, state_index, frame_count, delay))
	{
		image_sprite->autorelease();
		image_sprite->SetSpriteState(state_index);
		return image_sprite;
	}
	else
	{
		CC_SAFE_DELETE(image_sprite);
	}

	return NULL;
}

//ʹ��ͼƬ�ļ�·����������
ImageSprite* ImageSprite::CreateWithImagePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	if("" == path)
	{
		return NULL;
	}

	if(0 == frame_count)
	{
		frame_count = 1;
	}

	ImageSprite* image_sprite = new ImageSprite();
	if(image_sprite && image_sprite->init()
		&& image_sprite->AddSpriteStatePath(path, state_index, frame_count, delay))
	{
		image_sprite->autorelease();
		image_sprite->SetSpriteState(state_index);
		return image_sprite;
	}
	else
	{
		CC_SAFE_DELETE(image_sprite);
	}

	return NULL;
}

ImageSprite* ImageSprite::CreateWithImageData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	if("" == image_data.image_path)
	{
		return NULL;
	}

	if(0 == frame_count)
	{
		frame_count = 1;
	}

	ImageSprite* image_sprite = new ImageSprite();
	if(image_sprite && image_sprite->init()
		&& image_sprite->AddSpriteStateData(image_data, state_index, frame_count, delay))
	{
		image_sprite->autorelease();
		image_sprite->SetSpriteState(state_index);
		return image_sprite;
	}
	else
	{
		CC_SAFE_DELETE(image_sprite);
	}

	return NULL;
}

//����һ����֡��״̬ �����м���֡��ͨ��AddSpriteStateIndexʵ��
bool ImageSprite::AddSpriteState(ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	if(!IsSpriteStateExist(state_index))
	{
		Vector<cocos2d::SpriteFrame*> frame_array;

		//������������״̬�󶨺����ӳ���
		SPRITESTATE sprite_state;
		sprite_state.frame_count = frame_count;
		sprite_state.delay = delay;
		sprite_state.frame_array = frame_array;
		m_sprite_map[state_index] = sprite_state;
		return true;
	}
	else
	{
		ClearSpriteStateIndex(state_index);
		m_sprite_map[state_index].frame_count = frame_count;
		m_sprite_map[state_index].delay = delay;
	}

	return false;
}

//Ϊ��������һ��״̬
bool ImageSprite::AddSpriteStateImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	//��ȡͼƬ����ȡÿ֡��С
	if(!ViewHelper::LoadImageResource(image_file))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_file);
	Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());
	
	if(texture == NULL)
	{	
		return false;
	}

	float width = texture->getContentSize().width / frame_count;
	float height = texture->getContentSize().height;

	//��ͼƬ��Ƭ����붯��֡����
	ZQ::INT32 index; 
	float offset;
	SpriteFrame *sprite_frame;
	Vector<SpriteFrame*> frame_array;
	for(index = 0; index < frame_count; ++index)
	{	
		offset = index * width;
		sprite_frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(offset, 0, width, height));
		frame_array.pushBack(sprite_frame);
	}

	//������������״̬�󶨺����ӳ���
	SPRITESTATE sprite_state;
	sprite_state.frame_count = frame_count;
	sprite_state.delay = delay;
	sprite_state.frame_array = frame_array;
	sprite_state.offset_array.assign(frame_count, ORIGINOFFSET());
	m_sprite_map[state_index] = sprite_state;

	return true;
}

//Ϊ��������һ��״̬,���ļ��ж�ȡ��ֻ֧��png
bool ImageSprite::AddSpriteStatePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	int k;
	float width, height;
	char image_file[128];
    std::string temp_path = FileSystemEx::GetInstance()->GetResourcePath(path);

	CCSpriteFrame *sprite_frame;
	Vector<SpriteFrame*> frame_array;
	for(k=0; k<frame_count; ++k)
	{
		sprintf(image_file, "%s/%d.png", temp_path.c_str(), k+1);
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(image_file);		
		if(texture != NULL)
		{	
			width = texture->getContentSize().width;
			height = texture->getContentSize().height;
			sprite_frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(0, 0, width, height));
			frame_array.pushBack(sprite_frame);
		}
	}

	//������������״̬�󶨺����ӳ���
	SPRITESTATE sprite_state;
	sprite_state.frame_count = frame_count;
	sprite_state.delay = delay;
	sprite_state.frame_array = frame_array;
	sprite_state.offset_array.assign(frame_count, ORIGINOFFSET());
	m_sprite_map[state_index] = sprite_state;

	return true;
}

bool ImageSprite::AddSpriteStateData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count, float delay)
{
	//��ȡͼƬ����ȡÿ֡��С
	if(!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_data.image_path);
	Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if(texture == NULL)
	{	
		return false;
	}

	float width = image_data.width / frame_count;
	float height = image_data.height;

	//��ͼƬ��Ƭ����붯��֡����
	ZQ::INT32 index; 
	float offset;
	SpriteFrame *sprite_frame;
	Vector<cocos2d::SpriteFrame*> frame_array;
	for(index = 0; index < frame_count; ++index)
	{	
		offset = index * width;
		sprite_frame = SpriteFrame::createWithTexture(texture, CCRectMake(image_data.x + offset, image_data.y, width, height));
		frame_array.pushBack(sprite_frame);
	}

	//������������״̬�󶨺����ӳ���
	SPRITESTATE sprite_state;
	sprite_state.frame_count = frame_count;
	sprite_state.delay = delay;
	sprite_state.frame_array = frame_array;
	sprite_state.offset_array.assign(frame_count, ORIGINOFFSET());
	m_sprite_map[state_index] = sprite_state;

	return true;
}

// ���Ļ�����ĳһ��״̬�µĶ���֡
bool ImageSprite::AddSpriteStateIndex(const ImageData &image_data, ZQ::INT32 state_index)
{
	if(m_sprite_map.find(state_index) == m_sprite_map.end())
	{	
		LogUtil::LogError("ImageSprite::AddSpriteStateIndex crack");
		return false;
	}

	//��ȡͼƬ����ȡÿ֡��С
	if(!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_data.image_path);
	Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if(texture == NULL)
	{	
		return false;
	}

	//��ͼƬ���붯��֡����
	ORIGINOFFSET offset;
	CCSpriteFrame *sprite_frame;
	offset.offset_x = image_data.offset_x;
	offset.offset_y = image_data.offset_y;
	offset.scale_size = image_data.scale_size;
	m_sprite_map[state_index].offset_array.push_back(offset);
	sprite_frame = SpriteFrame::createWithTexture(texture, Rect(image_data.x, image_data.y, image_data.width, image_data.height));
	m_sprite_map[state_index].frame_array.pushBack(sprite_frame);
	return true;
}

// ���ĳһ��״̬�µĶ���֡
void ImageSprite::ClearSpriteStateIndex(ZQ::INT32 state_index)
{
	if(!IsSpriteStateExist(state_index))
	{
		return;
	}

	m_sprite_map[state_index].offset_array.clear();
	m_sprite_map[state_index].frame_array.clear();
	
	m_shadow_queue.clear();
	
// 	if(m_state == state_index)
// 	{
// 		CC_SAFE_RELEASE_NULL(m_pobTexture);
// 	}
}

//	���þ����״̬
void ImageSprite::SetSpriteState(ZQ::INT32 state_index)
{
	if(m_sprite_map.find(state_index) == m_sprite_map.end())
	{
		return;
	}

	if(m_state != state_index)
	{
		m_shadow_queue.clear();
	}

	m_state = state_index;
	m_sprite_date = m_sprite_map[state_index];
	m_frame_index = 0; 
	m_is_play_once = false;

	if (0 != m_sprite_date.frame_array.size() && m_sprite_date.frame_array.at(0))
 	{
		SpriteFrame *sprite_frame = m_sprite_date.frame_array.at(0);

		if(sprite_frame)
		{
			setContentSize(sprite_frame->getRect().size);
			setDisplayFrame(sprite_frame);
		}

		ResetStateScaleFactor(m_sprite_date.offset_array.at(m_frame_index).scale_size);
 	}
	else
	{
		return;
	}

	unschedule(schedule_selector(ImageSprite::Update));
	schedule(schedule_selector(ImageSprite::Update), m_sprite_date.delay);
}

//	��ȡ�����״̬
ZQ::INT32 ImageSprite::GetSpriteState()
{
	return m_state;
}

void ImageSprite::setPosition(const CCPoint& pos)
{
	m_x = pos.x;
	m_y = pos.y;
	SetPosition(m_x, m_y);
}

//���þ����λ��
void ImageSprite::SetPosition(float x, float y)
{
	m_x = x;
	m_y = y;

	float dx, dy;
	dx = m_is_filp_x ? -m_dx:m_dx;
	dy = m_is_flip_y ? -m_dy:m_dy;
	Sprite::setPosition(CCPointMake(m_x + dx, m_y + dy));
}

//���º��������һ��ʱ�����һ��
void ImageSprite::Update(float time)
{
	if(!m_is_update || m_state == -1)
	{	
		return;
	}

	m_sprite_date = m_sprite_map[m_state];
	m_frame_index = (++m_frame_index) % m_sprite_date.frame_count;
	
	//��¼�Ƿ��������Ź�һ��
	if(ZQ::INT32(m_frame_index) >= (m_sprite_date.frame_count - 1))
	{
		m_is_play_once = true;
	}	
	
	if (m_frame_index >= (int)m_sprite_date.frame_array.size() || !m_sprite_date.frame_array.at(m_frame_index))
	{
		return;
	}
	
	setSpriteFrame((SpriteFrame*)m_sprite_date.frame_array.at(m_frame_index));
	ResetStateScaleFactor(m_sprite_date.offset_array.at(m_frame_index).scale_size);

	if(m_is_play_once)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_playonce_fun_id, "");
	}
}

//�����Ƿ�ˮƽ��ת
void ImageSprite::SetFlipX(bool flip)
{
	m_is_filp_x = flip;
	setFlipX(flip);

	float dx, dy;
	dx = m_is_filp_x ? -m_dx:m_dx;
	dy = m_is_flip_y ? -m_dy:m_dy;
	Sprite::setPosition(CCPointMake(m_x + dx, m_y + dy));
}

//�����Ƿ�ˮƽ��ת
//In: flip �Ƿ�����ת
void ImageSprite::SetFlipY(bool flip)
{
	m_is_flip_y = flip;
	setFlipY(m_is_flip_y);

	float dx, dy;
	dx = m_is_filp_x ? -m_dx:m_dx;
	dy = m_is_flip_y ? -m_dy:m_dy;
	Sprite::setPosition(CCPointMake(m_x + dx, m_y + dy));
}

//�����Ƿ�ɼ�
void ImageSprite::SetVisible(bool visible)
{
	setVisible(visible);
}

//  �����յľ���
ImageSprite* ImageSprite::Node()
{
	ImageSprite* image_sprite = new ImageSprite();
	
	if(image_sprite && image_sprite->init())
	{
		image_sprite->autorelease();
		return image_sprite;
	}

	CC_SAFE_DELETE(image_sprite);
	return NULL;
}

void ImageSprite::visit()
{
	switch(m_effect)
	{
	case EFFECT_GREY:
		{
			ccColor3B color = getColor();
			setColor(ccc3(96, 96, 96));
			CCSprite::visit();
			setColor(color);
		}
		break;
		//��ӰЧ��
	case EFFECT_SHADOW:
		{
			int k = 0;
			int max = m_shadow_queue.size();
			Vec2 cur_pt = getPosition();
			GLubyte alpha = getOpacity();
			Color3B color = getColor();
			if(max > 1)
			{
				SHADOWINFO shadow_info;
				GLubyte alpha_var = 128 / max;
				//�����������ڵ�״̬������ʾӰ��
				for(k=0; k < max; ++k)
				{
					shadow_info = m_shadow_queue.at(k);
					setDisplayFrame(shadow_info.frame);
					setFlipX(shadow_info.flip);
					Sprite::setPosition(Vec2(shadow_info.x, shadow_info.y));
					setOpacity(alpha_var * k);
					setColor(ccc3(128, 128, 128));
					Sprite::visit();
				}
			}
			//����ǰ����
			if (m_frame_index < (int)m_sprite_date.frame_array.size())
			{
				setSpriteFrame((SpriteFrame*)m_sprite_date.frame_array.at(m_frame_index));
			}
			
			setFlipX(m_is_filp_x);
			Sprite::setPosition(cur_pt);
			setOpacity(alpha);
			setColor(color);
			Sprite::visit();
		}
		break;
	default:
		Sprite::visit();
	}
}

//���ö�λ��
void ImageSprite::SetAnchorPoint(float x, float y)
{
	Sprite::setAnchorPoint(cocos2d::Vec2(x, y));
}

void ImageSprite::setScale(float scale_size)
{
	m_scale_x = m_scale_y = scale_size;
	Sprite::setScale(scale_size / m_state_scale_size);
	ResetOffsetPos();
}

void ImageSprite::setScaleX(float x_scale_size)
{
	m_scale_x = x_scale_size;
	Sprite::setScaleX(m_scale_x / m_state_scale_size);
	ResetOffsetPos();
}

void ImageSprite::setScaleY(float y_scale_size)
{
	m_scale_y = y_scale_size;
	Sprite::setScaleY(m_scale_y / m_state_scale_size);
	ResetOffsetPos();
}

//����zOrder����
void ImageSprite::SetZOrder(ZQ::INT32 order)
{
	_localZOrder = order;
	if (Sprite::getParent())
	{
		Sprite::getParent()->reorderChild(this, _localZOrder);
	}
}

void ImageSprite::SetShadowInfo(float shadow_time, float frame_count)
{
	m_shadow_max = frame_count;
	m_shadow_time = shadow_time;
	unschedule(schedule_selector(ImageSprite::TrackShadow));
	schedule(schedule_selector(ImageSprite::TrackShadow), shadow_time / frame_count);
}

void ImageSprite::TrackShadow(float time)
{
	SHADOWINFO shadow_info;
	shadow_info.x = getPositionX();
	shadow_info.y =	getPositionY();
	shadow_info.sprite_state = m_state;
	shadow_info.index = m_frame_index;
	if (m_state == -1 || m_frame_index >= (int)m_sprite_date.frame_array.size())
	{
		return;
	}
	shadow_info.frame = (SpriteFrame*)m_sprite_date.frame_array.at(m_frame_index);
	shadow_info.flip = m_is_filp_x;
	m_shadow_queue.push_back(shadow_info);
	//����֡����벹��
	while(m_shadow_queue.size() < m_shadow_max)
	{
		m_shadow_queue.push_back(shadow_info);
	}
	//��ౣ��m_shadow_max֡��Ϣ
	while(m_shadow_queue.size() > m_shadow_max)
	{
		m_shadow_queue.pop_front();
	}
}

//���ӹ�Ӱƫ����
void ImageSprite::AddShadowOffset(float x, float y)
{
	int k = 0;
	int max = m_shadow_queue.size();
	for(k=0; k < max; ++k)
	{
		m_shadow_queue.at(k).x += x;
		m_shadow_queue.at(k).y += y;
	}
}

//���ü���Ч��
void ImageSprite::SetHighLight(bool high_light)
{
	if (m_highlight_enable == high_light)
		return;

	m_highlight_enable = high_light;
	if (m_highlight_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		auto gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_BRIGHT_NO_MVP);
		setGLProgram(gl_program);
		GLint bright_location = getGLProgram()->getUniformLocationForName("bright_value");
		if (bright_location != -1)
		{
			getGLProgramState()->setUniformFloat(bright_location, m_highlight_value);
		}
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

void ImageSprite::SetBright(float value)
{
	if (value <= 0 || value > 2.0)
		return;
	m_highlight_value = value;
}

//���ñ䰵Ч��
void ImageSprite::SetGray(bool is_grey)
{
	if(is_grey)
	{
		m_effect = EFFECT_GREY;
	}
	else
	{
		m_effect = EFFECT_NONE;
	}
}

//���ù�ӰЧ��
void ImageSprite::SetShadow(bool shadow)
{
	if(shadow)
	{
		m_effect = EFFECT_SHADOW;
		SetShadowInfo(m_shadow_time, m_shadow_max);
	}
	else
	{
		m_effect = EFFECT_NONE;
		unschedule(schedule_selector(ImageSprite::TrackShadow));
	}
}

//��ȡ���
float ImageSprite::GetWidth()
{
	return Sprite::getContentSize().width;
}

//��ȡ�߶�
float ImageSprite::GetHeight()
{
	return Sprite::getContentSize().height;
}

void ImageSprite::ResetOffsetPos()
{
	//û�����þ���״̬
	if(m_state == -1)
	{
		return;
	}
	
	//û����Ӿ���֡����
	if(m_frame_index >= m_sprite_date.offset_array.size())
	{
		return;
	}

	m_dx = m_sprite_date.offset_array.at(m_frame_index).offset_x * getScaleX();
	m_dy = m_sprite_date.offset_array.at(m_frame_index).offset_y * getScaleY(); 
	SetPosition(m_x, m_y);
}

//	��ѯ�����״̬�Ƿ����
bool ImageSprite::IsSpriteStateExist(ZQ::INT32 state_index)
{
	return m_sprite_map.find(state_index) != m_sprite_map.end();
}

// ���趯��Ƶ��
void ImageSprite::ResetUpdateDelay(ZQ::INT32 state_index, float delay_time)
{
	if(!IsSpriteStateExist(state_index))
	{
		return;
	}

	m_sprite_map[state_index].delay = delay_time;

	if(state_index == m_state)
	{
		unschedule(schedule_selector(ImageSprite::Update));
		schedule(schedule_selector(ImageSprite::Update), delay_time);
	}
}

// ���õ�ǰ�Ĳ���֡
void ImageSprite::SetFrameIndex(unsigned int frame_index)
{
	if(m_sprite_map.find(m_state) == m_sprite_map.end())
		return;

	if (frame_index >= (int)m_sprite_date.frame_array.size() || !m_sprite_date.frame_array.at(frame_index))
		return;

	m_frame_index = frame_index;
	setSpriteFrame(m_sprite_date.frame_array.at(frame_index));
	ResetStateScaleFactor(m_sprite_date.offset_array.at(m_frame_index).scale_size);
}

void ImageSprite::ResetStateScaleFactor(float state_scale)
{
	m_state_scale_size = state_scale;
	setScaleX(m_scale_x);
	setScaleY(m_scale_y);
}

void ImageSprite::onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	cocos2d::GLProgram* glProgram = getGLProgram();
	if (nullptr != glProgram)
	{
		if (m_highlight_enable)
		{
			GLint bright_location = glProgram->getUniformLocationForName("bright_value");
			if (bright_location != -1)
			{
				getGLProgramState()->setUniformFloat(bright_location, m_highlight_value);
			}
		}
	}

	Sprite::onDraw(renderer, transform, flags);
}
