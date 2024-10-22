
#include "renderer.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"
#include "particle/simplespline.h"
#include "particle/technique.h"
#include "particle/particledatamanager.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"

NS_CC_BEGIN

const MaterialType CCParticleRenderer::DEFAULT_MAT_TYPE = MAT_ALPHA_BLEND;
const float CCParticleRenderer::DEFAULT_RATIO = 0.0f;

void CCParticleRenderer::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (m_texture_name != "")
	{
		child = my_document->NewElement("texture_name");
		content = my_document->NewText(m_texture_name.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual((int)m_mat_type, (int)DEFAULT_MAT_TYPE))
	{
		str = CCParticleHelper::ToString(m_mat_type);
		child = my_document->NewElement("mat_type");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_add_mode_power_ratio, DEFAULT_RATIO))
	{
		str = CCParticleHelper::ToString(m_add_mode_power_ratio);
		child = my_document->NewElement("add_power_ratio");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCParticleRenderer::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "texture_name") == 0)
	{
		SetTexture(value);
	}
	else if (strcmp(type, "mat_type") == 0)
	{
		SetMaterialType((MaterialType)CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "add_power_ratio") == 0)
	{
		m_add_mode_power_ratio = CCParticleHelper::ParseFloat(value);
	}
	else
	{
		return false;
	}
	return true;
}

bool CCParticleRenderer::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str = "";
	if (strcmp(type, "texture_name") == 0)
	{
		str = m_texture_name;
	}
	else if (strcmp(type, "mat_type") == 0)
	{
		str = CCParticleHelper::ToString(m_mat_type);
	}
	else if (strcmp(type, "full_tex_name") == 0)
	{
		str = m_texture_full_name;
	}
	else if (strcmp(type, "add_power_ratio") == 0)
	{
		str = CCParticleHelper::ToString(m_add_mode_power_ratio);
	}
	else
	{
		return false;
	}

	strncpy(value, str.c_str(), size);
	return true;
}

void CCParticleRenderer::SetTexture(const std::string& name)
{
	m_texture_name = name;
	m_texture_full_name = PSManager::GetInstance()->GetTexturePath() + m_texture_name + "." + m_texture_suffix;
	//如果不存在该texture 则将路径指向多语言目录
	if (!FileUtils::getInstance()->isFileExist(m_texture_full_name))
	{
		m_texture_full_name = PSManager::GetInstance()->GetMutiLanguagePath() + m_texture_name + "." + m_texture_suffix;
	}
}

NS_CC_END
