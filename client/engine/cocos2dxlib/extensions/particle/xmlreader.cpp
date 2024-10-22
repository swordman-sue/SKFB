
#include "xmlreader.h"
#include "particlehelper.h"
#include "attribute.h"
#include "dynamicattribute.h"
#include "particlesystem.h"
#include "render/renderer.h"

NS_CC_BEGIN

bool PSReader::ParseFile(const char *file_name, MyParticleSystem* ps)
{
	if (ps == 0)
	{
		return false;
	}

	bool ret = true;
	tinyxml2::XMLDocument *my_document = new tinyxml2::XMLDocument();
	if(my_document->LoadFile(file_name)==tinyxml2::XML_NO_ERROR)
	{
		ret = ParseScript(my_document, ps);
	}
	delete my_document;

	return ret;
}

bool PSReader::ParseBuffer(const char *buffer, int len, MyParticleSystem* ps)
{
	if (ps == 0)
	{
		return false;
	}

	bool ret = true;

	tinyxml2::XMLDocument *my_document = new tinyxml2::XMLDocument();
	if(my_document->Parse(buffer, len)==tinyxml2::XML_NO_ERROR)
	{
		ret = ParseScript(my_document, ps);
	}
	delete my_document;

	return ret;
}

 bool PSReader::ParseScript(tinyxml2::XMLDocument *my_document, MyParticleSystem* ps)
{
	tinyxml2::XMLElement *root_element = my_document->RootElement();
	if (root_element == 0)
	{
		return false;
	}

	bool ret = true;

	std::string is_cycle = root_element->Attribute("is_cycle");
	if (is_cycle != "")
	{
		ret = ps->SetAttribute("is_cycle", is_cycle);
	}

	tinyxml2::XMLElement *current_element = root_element->FirstChildElement();
	while (current_element!=0 && ret)
	{
		if (strcmp(current_element->Value(), "Technique") == 0)
		{
			CCParticleTechnique *pt = ps->CreateTechnique();
			ret = ParseTechnique(pt, current_element);
		}
		else if (strcmp(current_element->Value(), "ClippingNode") == 0)
		{
			ps->CreateClippingNode("");
			ret = ParseClipper(ps, current_element);
		}
		else if (current_element->FirstChild())
		{
			ret = ps->SetAttribute(current_element->Value(), current_element->FirstChild()->Value());
		}
		current_element = current_element->NextSiblingElement();
	}

	return ret;
}

void PSReader::SaveScript(MyParticleSystem *ps, const char *file_name)
{
	if (ps == 0)
	{
		return;
	}

	tinyxml2::XMLDocument *my_document = new tinyxml2::XMLDocument();
	tinyxml2::XMLDeclaration *declaration = my_document->NewDeclaration(); 
	my_document->LinkEndChild(declaration);

	tinyxml2::XMLElement *xml_root = my_document->NewElement("ParticleSystem");
	my_document->LinkEndChild(xml_root);

	std::string is_cycle = "";
	ps->GetAttribute("is_cycle", is_cycle);
	xml_root->SetAttribute("is_cycle", is_cycle.c_str());

	std::string str;
	tinyxml2::XMLElement *element = 0;
	tinyxml2::XMLText *content = 0;

	float scale_x = ps->getScaleX();
	Vec3 scale(ps->getScaleX(), ps->getScaleY(), ps->GetScaleZ());
	static Vec3 one (1, 1, 1);
	if (!CCParticleHelper::IsEqual(scale, one))
	{
		str = CCParticleHelper::ToString(scale);
		element = my_document->NewElement("ps_scale");
		xml_root->LinkEndChild(element);
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(ps->GetScaleTime(), 1.0f))
	{
		element = my_document->NewElement("scale_time");
		xml_root->LinkEndChild(element);
		content = my_document->NewText(CCParticleHelper::ToString(ps->GetScaleTime()).c_str());
		element->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(ps->GetScaleSpeed(), 1.0f))
	{
		element = my_document->NewElement("scale_speed");
		xml_root->LinkEndChild(element);
		content = my_document->NewText(CCParticleHelper::ToString(ps->GetScaleSpeed()).c_str());
		element->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(ps->GetPreTime(), 0.0f))
	{
		element = my_document->NewElement("pre_time");
		xml_root->LinkEndChild(element);
		content = my_document->NewText(CCParticleHelper::ToString(ps->GetPreTime()).c_str());
		element->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(ps->GetFramePreSecond(), 30))
	{
		element = my_document->NewElement("frame_pre_second");
		xml_root->LinkEndChild(element);
		content = my_document->NewText(CCParticleHelper::ToString(ps->GetFramePreSecond()).c_str());
		element->LinkEndChild(content);
	}

	str = CCParticleHelper::ToString(ps->GetCustomBox());
	element = my_document->NewElement("bound");
	xml_root->LinkEndChild(element);
	content = my_document->NewText(str.c_str());
	element->LinkEndChild(content);

	float cycle_time = ps->GetCycleTotalTime();
	if (!CCParticleHelper::IsEqual(cycle_time, MyParticleSystem::DEFAULT_CYCLETIME))
	{
		element = my_document->NewElement("cycle_total_time");
		content = my_document->NewText(CCParticleHelper::ToString(cycle_time).c_str());
		element->LinkEndChild(content);
		xml_root->LinkEndChild(element);
	}

	int size = ps->GetNumTechniques();
	for (int i = 0; i < size; ++i)
	{
		CCParticleTechnique *pt = ps->GetTechnique(i);
		element = my_document->NewElement("Technique");
		pt->SaveScript(my_document, element);
		xml_root->LinkEndChild(element);
	}

	std::map<std::string, ClippingNode*>& clippers = ps->GetClippers();
	if (clippers.size() > 0)
	{
		element = my_document->NewElement("ClippingNode");

		tinyxml2::XMLElement *child_element = nullptr;
		tinyxml2::XMLText *child_text = nullptr;

		Rect& rect = ps->GetClipperRect();

		//µ²°å¿í
		child_element = my_document->NewElement("clipper_width");
		child_text = my_document->NewText( CCParticleHelper::ToString(rect.size.width).c_str() );
		child_element->LinkEndChild(child_text);
		element->LinkEndChild(child_element);

		//µ²°å¸ß
		child_element = my_document->NewElement("clipper_height");
		child_text = my_document->NewText( CCParticleHelper::ToString(rect.size.height).c_str() );
		child_element->LinkEndChild(child_text);
		element->LinkEndChild(child_element);

		//µ×°å²»¿É¼û
		if (!(ps->GetInverted()))
		{
			child_element = my_document->NewElement("clipper_inverted");
			child_text = my_document->NewText("false");
			child_element->LinkEndChild(child_text);
			element->LinkEndChild(child_element);
		}

		Vec2& pos = ps->GetClipperPos();
		if (pos.x != 0 || pos.y != 0)
		{
			child_element = my_document->NewElement("clipper_pos");
			child_text = my_document->NewText( CCParticleHelper::ToString(Vec3(pos.x, pos.y, 0)).c_str() );
			child_element->LinkEndChild(child_text);
			element->LinkEndChild(child_element);
		}

		std::string exclude_list = ps->GetClipperExcludeList();
		if (exclude_list != "")
		{
			child_element = my_document->NewElement("clipper_technique_list");
			child_text = my_document->NewText(exclude_list.c_str());
			child_element->LinkEndChild(child_text);
			element->LinkEndChild(child_element);
		}

		xml_root->LinkEndChild(element);
	}

	my_document->SaveFile(file_name);
	delete my_document;
	my_document = 0;
}

bool PSReader::ParseTechnique(CCParticleTechnique* technique, tinyxml2::XMLElement *element)
{
	bool ret = true;

	tinyxml2::XMLElement *current_element = element->FirstChildElement();
	while (current_element && ret)
	{
		if (strcmp(current_element->Value(), "Emitter") == 0)
		{
			const char* type = current_element->Attribute("type");
			if (type != 0)
			{
				CCParticleEmitter *emitter = technique->CreateEmitter(type);
				if (emitter == 0)
				{
					ret = false;
					break;
				}
				ret = ParseParticleAttribute(emitter, current_element);
			}
		}
		else if (strcmp(current_element->Value(), "Affector") == 0)
		{
			const char* type = current_element->Attribute("type");
			if (type != 0)
			{
				size_t index = 0;
				CCParticleAffector *pa = technique->CreateAffector(type, index);
				if (pa == 0)
				{
					ret = false;
					break;
				}
				ret = ParseParticleAttribute(pa, current_element);
			}
		}
		else if (strcmp(current_element->Value(), "render") == 0)
		{
			const char* type = current_element->Attribute("type");
			if (type != 0)
			{
				CCParticleRenderer *renderer = technique->CreateRender(type, false);
				if (renderer == 0)
				{
					ret = false;
					break;
				}
				ret = ParseParticleAttribute(renderer, current_element);
			}
		}
		else if(current_element->FirstChild())
		{
			technique->SetAttribute(current_element->Value(), current_element->FirstChild()->Value());
		}

		current_element = current_element->NextSiblingElement();
	}

	return ret;
}

bool PSReader::ParseClipper(MyParticleSystem* ps, tinyxml2::XMLElement *element)
{
	bool ret = true;

	tinyxml2::XMLElement *child_element = element->FirstChildElement();

	while (child_element && ret)
	{
		if (child_element->FirstChild())
		{
			ret = ps->SetAttribute(child_element->Value(), child_element->FirstChild()->Value());
		}
		
		child_element = child_element->NextSiblingElement();
	}

	return ret;
}

bool PSReader::ParseParticleAttribute(CCParticleAttribute* attribute, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *current_element = element->FirstChildElement();
	bool ret = true;
	while (current_element && ret)
	{
		if (strcmp(current_element->Value(), "dyn") == 0)
		{
			if (strcmp(current_element->Attribute("dyn_type"), "fixed") == 0)
			{
				const char* type = current_element->Attribute("type");
				if (type != 0)
				{
					DynamicAttributeFixed *dyn_fixed = new DynamicAttributeFixed();
					ret = ParseDynAttribute(dyn_fixed, current_element) && attribute->SetDynAttribute(type, dyn_fixed);
				}
			}
			else if (strcmp(current_element->Attribute("dyn_type"), "random") == 0)
			{
				const char* type = current_element->Attribute("type");
				if (type != 0)
				{
					DynamicAttributeRandom *dyn_random = new DynamicAttributeRandom();
					ret = ParseDynAttribute(dyn_random, current_element) && attribute->SetDynAttribute(type, dyn_random);
				}
			}
			else if (strcmp(current_element->Attribute("dyn_type"), "curve_linear") == 0)
			{
				const char* type = current_element->Attribute("type");
				if (type != 0)
				{
					DynamicAttributeCurved *dyn_curved = new DynamicAttributeCurved();
					dyn_curved->SetInterpolationType(DynamicAttributeCurved::IT_LINEAR);
					ret = ParseDynAttribute(dyn_curved, current_element) && attribute->SetDynAttribute(type, dyn_curved);
				}
			}
			else if (strcmp(current_element->Attribute("dyn_type"), "curve_spline") == 0)
			{
				const char* type = current_element->Attribute("type");
				if (type != 0)
				{
					DynamicAttributeCurved *dyn_curved = new DynamicAttributeCurved();
					dyn_curved->SetInterpolationType(DynamicAttributeCurved::IT_SPLINE);
					ret = ParseDynAttribute(dyn_curved, current_element) && attribute->SetDynAttribute(type, dyn_curved);
				}
			}
		}
		else if(current_element->FirstChild())
		{
			attribute->SetAttribute(current_element->Value(), current_element->FirstChild()->Value());
		}

		current_element = current_element->NextSiblingElement();
	}

	return ret;
}

bool PSReader::ParseDynAttribute(DynamicAttribute* attribute, tinyxml2::XMLElement *element)
{
	if (element == 0 || attribute == 0)
	{
		return false;
	}

	bool ret = true;

	tinyxml2::XMLElement *current_element = element->FirstChildElement();
	while(current_element && ret)
	{
		if (current_element->FirstChild())
		{
			ret = attribute->SetAttribute(current_element->Value(), current_element->FirstChild()->Value());
		}

		current_element = current_element->NextSiblingElement();
	}
	return ret;
}

NS_CC_END