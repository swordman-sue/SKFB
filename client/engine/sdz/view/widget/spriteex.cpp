#include "spriteex.h"
#include "view/struct.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"

using namespace cocos2d;

SpriteEx::SpriteEx():
m_highlight_enable(false),
m_highlight_value(1.6f),
m_grey_enable(false),
m_blur_enable(false),
m_feather_enable(false),
m_feather_type(0),
m_feather_focus_factor(1.0f),
m_feather_focus(0.5f, 0.5f),
m_feather_attenuation_threshold(0.8f),
m_shader_program(nullptr)
{}

bool SpriteEx::InitWithImageFile(const std::string &image_file)
{
	if(!Sprite::init())
	{
		return false;
	}

	if (!SetImage(image_file))
		return false;

	return true;
}

bool SpriteEx::InitWithImageFile(const ImageData& image_data)
{
	if (!Sprite::init())
	{
		return false;
	}

	if (!SetImage(image_data))
		return false;

	return true;
}

bool SpriteEx::InitWithTexture(Texture2D *texture)
{
	return Sprite::initWithTexture(texture);
}

SpriteEx* SpriteEx::Node()
{
	SpriteEx *label = new SpriteEx();
	if(!label->init())
	{
		CC_SAFE_DELETE(label);
		return NULL;
	}

	label->autorelease();
	return label;
}

SpriteEx* SpriteEx::CreateWithImageFile(const std::string &image_file)
{
	SpriteEx *label = new SpriteEx();
	//if(!label->InitWithImageFile(image_file.c_str()))
	//{
	//	CC_SAFE_DELETE(label);
	//	return NULL;
	//}

	label->InitWithImageFile(image_file.c_str());
	label->autorelease();
	return label;
}

SpriteEx* SpriteEx::CreateWithImageFile(const ImageData& image_data)
{
	SpriteEx *label = new SpriteEx();
	//if(!label->InitWithImageFile(image_data))
	//{
	//	CC_SAFE_DELETE(label);
	//	return NULL;
	//}

	label->InitWithImageFile(image_data);
	label->autorelease();
	return label;
}

bool SpriteEx::SetImage(const std::string &image_file)
{
	if("" == image_file)
	{	
		return false;
	}

	if(!ViewHelper::LoadImageResource(image_file))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_file);
	Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());
	Rect image_rect = Rect(0, 0, texture->getContentSize().width, texture->getContentSize().height);

	if(NULL == texture)
	{
		return false;
	}

	Sprite::setTexture(texture);
	Sprite::setTextureRect(image_rect);

	return true;
}

bool SpriteEx::SetImage(const ImageData& image_data)
{
	if("" == image_data.image_path)
	{	
		return false;
	}

	if(!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return false;
	}

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_data.image_path);
	Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());
	Rect image_rect = Rect(image_data.x, image_data.y, image_data.width, image_data.height);

	if(NULL == texture)
	{
		return false;
	}

	Sprite::setTexture(texture);
	Sprite::setTextureRect(image_rect);
	
	return true;
}

//设置加亮效果
void SpriteEx::SetHighLight(bool high_light)
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

void SpriteEx::SetBright(float value)
{
	if (value <= 0 || value > 2.0)
		return;
	m_highlight_value = value;
}

float SpriteEx::GetBright()
{
	return m_highlight_value;
}

//设置灰度化
void SpriteEx::SetGray(bool grey)
{
	if (m_grey_enable == grey)
		return;

	m_grey_enable = grey;
	if (m_grey_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		setGLProgram(GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP));
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

//设置模糊
void SpriteEx::SetBlur(bool var)
{
	if (m_blur_enable == var)
		return;

	m_blur_enable = var;
	if (m_blur_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		setGLProgram(GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_BLUR_NOMVP));

		cocos2d::GLProgram* glProgram = getGLProgram();
		GLint pixel_offset_location = glProgram->getUniformLocationForName("u_pixel_offset");
		if (pixel_offset_location != -1 && nullptr != _texture)
		{
			getGLProgramState()->setUniformVec2(pixel_offset_location, Vec2(1.0f / _texture->getPixelsWide(), 1.0f / _texture->getPixelsHigh()));
		}
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

//设置羽化
void SpriteEx::SetFeather(bool var)
{
	if (m_feather_enable == var)
		return;

	m_feather_enable = var;
	if (m_feather_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		setGLProgram(GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_FEATHER_NOMVP));

		GLProgram* glProgram = getGLProgram();

		GLint type_location = glProgram->getUniformLocationForName("u_type");
		if (type_location != -1)
		{
			getGLProgramState()->setUniformInt(type_location, m_feather_type);
		}

		GLint tex_size_location = glProgram->getUniformLocationForName("u_texSize");
		if (tex_size_location != -1 && nullptr != _texture)
		{
			getGLProgramState()->setUniformVec2(tex_size_location, Vec2(_texture->getPixelsWide(), _texture->getPixelsHigh()));
		}

		GLint focus_center_location = glProgram->getUniformLocationForName("u_focusCenter");
		if (focus_center_location != -1)
		{
			Vec2 focus_center;
			const Tex2F& tlTexCord = _quad.tl.texCoords;
			const Tex2F& brTexCord = _quad.br.texCoords;
			focus_center.x = (tlTexCord.u + (brTexCord.u - tlTexCord.u) * m_feather_focus.x) * _texture->getPixelsWide();
			focus_center.y = (tlTexCord.v + (brTexCord.v - tlTexCord.v) * m_feather_focus.y) * _texture->getPixelsHigh();
			getGLProgramState()->setUniformVec2(focus_center_location, focus_center);
		}

		GLint focus_factor_location = glProgram->getUniformLocationForName("u_focusFactor");
		if (focus_factor_location != -1)
		{
			getGLProgramState()->setUniformFloat(focus_factor_location, m_feather_focus_factor);
		}

		GLint focus_radius_sq_location = glProgram->getUniformLocationForName("u_focusRadiusSq");
		if (focus_radius_sq_location != -1 && nullptr != _texture)
		{
			const Tex2F& tlTexCord = _quad.tl.texCoords;
			const Tex2F& brTexCord = _quad.br.texCoords;
			float radius_u = 0.5 * (brTexCord.u - tlTexCord.u) * _texture->getPixelsWide();
			float radius_v = 0.5 * (brTexCord.v - tlTexCord.v) * _texture->getPixelsHigh();
			float radius = std::min(radius_u, radius_v);
			getGLProgramState()->setUniformFloat(focus_radius_sq_location, radius * radius);
		}

		GLint ratio_location = glProgram->getUniformLocationForName("u_ratio");
		if (ratio_location != -1 && nullptr != _texture)
		{
			getGLProgramState()->setUniformFloat(ratio_location, (float)_texture->getPixelsWide() / _texture->getPixelsHigh());
		}

		GLint attenuation_threshold_location = glProgram->getUniformLocationForName("u_attenuation_threshold");
		if (attenuation_threshold_location != -1)
		{
			getGLProgramState()->setUniformFloat(attenuation_threshold_location, m_feather_attenuation_threshold);
		}
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

void SpriteEx::SetFeatherFocus(float u, float v)
{
	u = std::max(0.0f, std::min(u, 1.0f));
	v = std::max(0.0f, std::min(v, 1.0f));
	m_feather_focus.x = u; 
	m_feather_focus.y = v;
}

void SpriteEx::SetFeatherFocusFactor(float var)
{
	var = var > 0 ? var : 1.0f;
	m_feather_focus_factor = var;
}

void SpriteEx::SetFeatherAttenuationThreshold(float var)
{
	m_feather_attenuation_threshold = var;
}

//自动拉伸到width, height
void SpriteEx::SetStretchSize(ZQ::UINT32 width, ZQ::UINT32 height)
{
	Size origin_size = getContentSize();
	Sprite::setScaleX(width / origin_size.width);
	Sprite::setScaleY(height / origin_size.height);
}

//获取宽度
float SpriteEx::GetWidth()
{
	return Sprite::getContentSize().width;
}

//获取高度
float SpriteEx::GetHeight()
{
	return Sprite::getContentSize().height;
}


void SpriteEx::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	Sprite::setColor(ccc3(red, green, blue));
}

//设置翻转
void SpriteEx::SetFlipX(bool flip_x)
{
	Sprite::setFlipX(flip_x);
}

void SpriteEx::SetFlipY(bool flip_y)
{
	Sprite::setFlipY(flip_y);
}

void SpriteEx::onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	cocos2d::GLProgram* glProgram = getGLProgram();
	if (nullptr != glProgram)
	{
		//高亮相关参数
		if (m_highlight_enable)
		{
			GLint bright_location = glProgram->getUniformLocationForName("bright_value");
			if (bright_location != -1)
			{
				getGLProgramState()->setUniformFloat(bright_location, m_highlight_value);
			}
		}

		//模糊相关参数
		if (m_blur_enable)
		{
			GLint pixel_offset_location = glProgram->getUniformLocationForName("u_pixel_offset");
			if (pixel_offset_location != -1 && nullptr != _texture)
			{
				getGLProgramState()->setUniformVec2(pixel_offset_location, Vec2(1.0f / _texture->getPixelsWide(), 1.0f / _texture->getPixelsHigh()));
			}
		}

		//羽化相关参数
		if (m_feather_enable)
		{
			GLint type_location = glProgram->getUniformLocationForName("u_type");
			if (type_location != -1)
			{
				getGLProgramState()->setUniformInt(type_location, m_feather_type);
			}

			GLint tex_size_location = glProgram->getUniformLocationForName("u_texSize");
			if (tex_size_location != -1 && nullptr != _texture)
			{
				getGLProgramState()->setUniformVec2(tex_size_location, Vec2(_texture->getPixelsWide(), _texture->getPixelsHigh()));
			}

			GLint focus_center_location = glProgram->getUniformLocationForName("u_focusCenter");
			if (focus_center_location != -1 && nullptr != _texture)
			{
				Vec2 focus_center;
				const Tex2F& tlTexCord = _quad.tl.texCoords;
				const Tex2F& brTexCord = _quad.br.texCoords;
				focus_center.x = (tlTexCord.u + (brTexCord.u - tlTexCord.u) * m_feather_focus.x) * _texture->getPixelsWide();
				focus_center.y = (tlTexCord.v + (brTexCord.v - tlTexCord.v) * m_feather_focus.y) * _texture->getPixelsHigh();
				getGLProgramState()->setUniformVec2(focus_center_location, focus_center);
			}

			GLint focus_factor_location = glProgram->getUniformLocationForName("u_focusFactor");
			if (focus_factor_location != -1)
			{
				getGLProgramState()->setUniformFloat(focus_factor_location, m_feather_focus_factor);
			}

			GLint focus_radius_sq_location = glProgram->getUniformLocationForName("u_focusRadiusSq");
			if (focus_radius_sq_location != -1 && nullptr != _texture)
			{
				const Tex2F& tlTexCord = _quad.tl.texCoords;
				const Tex2F& brTexCord = _quad.br.texCoords;
				float radius_u = 0.5 * (brTexCord.u - tlTexCord.u) * _texture->getPixelsWide();
				float radius_v = 0.5 * (brTexCord.v - tlTexCord.v) * _texture->getPixelsHigh();
				float radius = std::min(radius_u, radius_v);
				getGLProgramState()->setUniformFloat(focus_radius_sq_location, radius * radius);
			}

			GLint ratio_location = glProgram->getUniformLocationForName("u_ratio");
			if (ratio_location != -1 && nullptr != _texture)
			{
				getGLProgramState()->setUniformFloat(ratio_location, (float)_texture->getPixelsWide() / _texture->getPixelsHigh());
			}

			GLint attenuation_threshold_location = glProgram->getUniformLocationForName("u_attenuation_threshold");
			if (attenuation_threshold_location != -1)
			{
				getGLProgramState()->setUniformFloat(attenuation_threshold_location, m_feather_attenuation_threshold);
			}			
		}
	}

	Sprite::onDraw(renderer, transform, flags);
}
