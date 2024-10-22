#include "scale9spriteex.h"
#include "view/struct.h"
#include "view/viewhelper.h"
#include "filesystem/filesystemex.h"

USING_NS_CC;

Scale9SpriteEx::Scale9SpriteEx()
:m_rect_inner(0.4f, 0.4f, 0.2f, 0.2f),
m_stretch_size(0, 0),
m_highlight_enable(false),
m_highlight_value(1.6f),
m_grey_enable(false),
m_shader_program(nullptr),
m_flip_x(false),
m_flip_y(false)
{}

Scale9SpriteEx::~Scale9SpriteEx()
{
}

Scale9SpriteEx* Scale9SpriteEx::Node()
{
	Scale9SpriteEx *label = new Scale9SpriteEx();
	if(!label->init())
	{
		CC_SAFE_DELETE(label);
		return NULL;
	}

	label->autorelease();
	return label;
}

//使用图片创建标签控件
//In : image_file 图片文件路径
Scale9SpriteEx* Scale9SpriteEx::CreateWithImageFile(const std::string &image_file)
{
	Scale9SpriteEx *label = new Scale9SpriteEx();
	//if(!label->InitWithImageFile(image_file))
	//{
	//	CC_SAFE_DELETE(label);
	//	return NULL;
	//}

	label->InitWithImageFile(image_file);
	label->autorelease();
	return label;
}

Scale9SpriteEx* Scale9SpriteEx::CreateWithImageFile(const ImageData& image_data)
{
	Scale9SpriteEx *label = new Scale9SpriteEx();
	//if(!label->InitWithImageFile(image_data))
	//{
	//	CC_SAFE_DELETE(label);
	//	return NULL;
	//}

	label->InitWithImageFile(image_data);
	label->autorelease();
	return label;
}

bool Scale9SpriteEx::InitWithImageFile(const std::string &image_file)
{
	if (!ViewHelper::LoadImageResource(image_file))
	{
		return false;
	}
	
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_file);
	if (!Scale9Sprite::initWithFile(full_name))
	{
		return false;
	}

	m_stretch_size = m_origin_size = _spriteRect.size;
	m_texture_rect = Rect(0, 0, m_origin_size.width, m_origin_size.height);
	SetInnerRect(m_rect_inner);

	return true;
}

bool Scale9SpriteEx::InitWithImageFile(const ImageData& image_data)
{
	if (!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_data.image_path);
	Rect image_rect = Rect(image_data.x, image_data.y, image_data.width, image_data.height);

	if (!Scale9Sprite::initWithFile(full_name.c_str(), image_rect))
	{
		return false;
	}
	
	m_stretch_size = m_origin_size = _spriteRect.size;
	m_texture_rect = image_rect;
	SetInnerRect(m_rect_inner);

	return true;
}

bool Scale9SpriteEx::SetImage(const std::string &image_file)
{
	return InitWithImageFile(image_file);
}

bool Scale9SpriteEx::SetImage(const ImageData& image_data)
{
	return InitWithImageFile(image_data);
}

void Scale9SpriteEx::SetInnerRect(float x, float y, float width, float height)
{
	Rect inner_rect(x, y, width, height);
	SetInnerRect(inner_rect);
}

void Scale9SpriteEx::SetInnerRect(const cocos2d::Rect& rc_inner)
{
	m_rect_inner = rc_inner;
	Rect pixel_rect(m_rect_inner.origin.x * m_origin_size.width, m_rect_inner.origin.y * m_origin_size.height,
		m_rect_inner.size.width * m_origin_size.width, m_rect_inner.size.height * m_origin_size.height);
	Scale9Sprite::setCapInsets(pixel_rect);

	//重置大小
	SetStretchSize(m_stretch_size.width, m_stretch_size.height);
	
	//重置翻转
	if (m_flip_x)
	{
		m_flip_x = false;
		SetFlipX(true);
	}

	if (m_flip_y)
	{
		m_flip_y = false;
		SetFlipY(true);
	}
}

void Scale9SpriteEx::SetStretchSize(float width, float height)
{
	m_stretch_size.width = width;
	m_stretch_size.height = height;
	setPreferredSize(Size(width, height));
}

void Scale9SpriteEx::SetContentSize(float width, float height)
{
	SetStretchSize(width, height);
}

void Scale9SpriteEx::SetContentWidth(float width)
{
	SetContentSize(width, m_stretch_size.height);
}

void Scale9SpriteEx::SetContentHeight(float height)
{
	SetContentSize(m_stretch_size.width, height);
}

//设置颜色
void Scale9SpriteEx::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	Scale9Sprite::setColor(Color3B(red, green, blue));
}

//设置加亮效果
void Scale9SpriteEx::SetHighLight(bool high_light)
{
	if (m_highlight_enable == high_light)
		return;

	m_highlight_enable = high_light;
	cocos2d::GLProgramState* gl_program_state;

	if (m_highlight_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = _scale9Image->getGLProgramState();
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
		gl_program_state = m_shader_program;
		m_shader_program = nullptr;
	}
}

void Scale9SpriteEx::SetBright(float value)
{
	if (value <= 0 || value > 2.0)
		return;

	m_highlight_value = value;
}

//设置灰度化
void Scale9SpriteEx::SetGray(bool grey)
{
	if (m_grey_enable == grey)
		return;

	m_grey_enable = grey;
	cocos2d::GLProgramState* gl_program_state;

	if (m_grey_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = _scale9Image->getGLProgramState();
		}

		gl_program_state = GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP);
	}
	else
	{
		gl_program_state = m_shader_program;
		m_shader_program = nullptr;
	}

	//setGLProgramState(gl_program_state);
	_scale9Image->setGLProgramState(gl_program_state);
}

void Scale9SpriteEx::SetFlipX(bool flip_x)
{
	if (m_flip_x == flip_x)
		return;

	setFlippedX(flip_x);
	m_flip_x = flip_x;
}

void Scale9SpriteEx::SetFlipY(bool flip_y)
{
	if (m_flip_y == flip_y)
		return;

	setFlippedY(m_flip_y);
	m_flip_y = flip_y;
}

void Scale9SpriteEx::onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
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

	Scale9Sprite::onDraw(renderer, transform, flags);
}