#include "dynamicattribute.h"
#include "particlehelper.h"
#include "simplespline.h"
#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <algorithm>

NS_CC_BEGIN

bool DynamicAttributeCurved::m_force_linear = true;

DynamicAttributeFixed::DynamicAttributeFixed(const DynamicAttributeFixed& dynamicAttributeFixed)
{
	m_type = DynamicAttribute::DAT_FIXED;
	m_value = dynamicAttributeFixed.m_value;
}

void DynamicAttributeFixed::CopyAttributesTo (DynamicAttribute* dynamicAttribute)
{
	if (!dynamicAttribute || dynamicAttribute->GetType() != DynamicAttribute::DAT_FIXED)
		return;

	DynamicAttributeFixed* fixed = static_cast<DynamicAttributeFixed*>(dynamicAttribute);
	fixed->m_value = m_value;
}

DynamicAttributeRandom::DynamicAttributeRandom (const DynamicAttributeRandom& dynamicAttributeRandom)
{
	m_type = DynamicAttribute::DAT_RANDOM;
	m_min = dynamicAttributeRandom.m_min;
	m_max = dynamicAttributeRandom.m_max;
}

void DynamicAttributeRandom::CopyAttributesTo(DynamicAttribute* dynamicAttribute)
{
	if (!dynamicAttribute || dynamicAttribute->GetType() != DynamicAttribute::DAT_RANDOM)
		return;

	DynamicAttributeRandom* dynAttr = static_cast<DynamicAttributeRandom*>(dynamicAttribute);
	dynAttr->m_min = m_min;
	dynAttr->m_max = m_max;
}

float DynamicAttributeRandom::GetValue (float x)
{
	return CCParticleHelper::RangeRandom(m_min, m_max);
}

DynamicAttributeCurved::DynamicAttributeCurved (void) : 
	m_interpolation_type(IT_LINEAR), 
	m_range(0),
	m_last_index(0),
	m_last_time(0.0f)
{
	m_type = DynamicAttribute::DAT_CURVED;
}
//-----------------------------------------------------------------------
DynamicAttributeCurved::DynamicAttributeCurved (InterpolationType interpolationType) : 
	m_interpolation_type(interpolationType), 
	m_range(0),
	m_last_index(0),
	m_last_time(0.0f)
{
	m_type = DynamicAttribute::DAT_CURVED;
}
//-----------------------------------------------------------------------
DynamicAttributeCurved::DynamicAttributeCurved (const DynamicAttributeCurved& dynamicAttributeCurved)
{
	m_type = DynamicAttribute::DAT_CURVED;
	m_interpolation_type = dynamicAttributeCurved.m_interpolation_type;
	m_spline = dynamicAttributeCurved.m_spline;
	m_range = dynamicAttributeCurved.m_range;
	m_last_index = 0;
	m_last_time = 0.0f;

	size_t size = dynamicAttributeCurved.m_control_points.size();
	m_control_points.reserve(size);
	m_control_points.resize(size);

	for (size_t index = 0; index < size; ++index)
	{
		m_control_points[index].x = dynamicAttributeCurved.m_control_points[index].x;
		m_control_points[index].y = dynamicAttributeCurved.m_control_points[index].y;
	}
	ProcessControlPoints();
}

void DynamicAttributeCurved::SetInterpolationType (InterpolationType interpolationType)
{
	if (interpolationType != m_interpolation_type)
	{
		// If switched to another InterpolationType, the already existing ControlPoints will be removed.
		RemoveAllControlPoints();
		m_interpolation_type = interpolationType;
	}
}
//-----------------------------------------------------------------------
DynamicAttributeCurved::InterpolationType DynamicAttributeCurved::GetInterpolationType (void) const
{
	return m_interpolation_type;
}
//-----------------------------------------------------------------------

float DynamicAttributeCurved::GetCalculusValue()
{
	float value = 0.0f;
	int size = m_control_points.size() - 1;
	for (int i = 0; i < size; i++)
	{
		Vec2& pos1 = m_control_points[i];
		Vec2& pos2 = m_control_points[i + 1];

		//求出两点之间的线性关系
		float k = 0.0f;
		if (pos1.x != pos2.x)
		{
			k = (pos2.y - pos1.y) / (pos2.x - pos1.x);
		}
		float b = pos1.y - k * pos1.x;
		//计算整个曲线的积分
		value += 0.5 * k * pos2.x * pos2.x + b * pos2.x - (0.5 * k * pos1.x * pos1.x + b * pos1.x);
	}

	return value;
}

float DynamicAttributeCurved::GetValue (float x)
{
	if (m_control_points.empty())
		return 0;

	size_t size = m_control_points.size();
	if (m_last_time >= x)
	{
		m_last_index = 0;
	}
	m_last_time = x;

	size_t index = FindNearestControlPointIterator(x);
	m_last_index = index + 1;
	if(m_last_index >= size)
	{
		m_last_index = size;
		return m_control_points[size - 1].y;
	}

	float t = (x - m_control_points[index].x) / (m_control_points[index + 1].x - m_control_points[index].x);

	if (m_force_linear)
	{
		return m_control_points[index].y + (m_control_points[index + 1].y - m_control_points[index].y) * t;
	}
	else
	{
		switch (m_interpolation_type)
		{
		case IT_LINEAR:
			{
				// Search the interval in which 'x' resides and apply linear interpolation
				return m_control_points[index].y + (m_control_points[index + 1].y - m_control_points[index].y) * t;
			}
			break;

		case IT_SPLINE:
			{
				if (m_spline.GetNumPoints() < 1)
					return 0;

				return (m_spline.Interpolate(index, t)).y;
			}
			break;
		}
	}

	return 0;
}
//-----------------------------------------------------------------------
void DynamicAttributeCurved::AddControlPoint (float x, float y)
{
	 Vec2 point(x, y);
	m_control_points.push_back(point);
}

void DynamicAttributeCurved::AddControlPoint (const Vec2& point)
{
	m_control_points.push_back(point);
}
//-----------------------------------------------------------------------
const DynamicAttributeCurved::ControlPointList& DynamicAttributeCurved::GetControlPoints (void) const
{
	return m_control_points;
}
//-----------------------------------------------------------------------
void DynamicAttributeCurved::ProcessControlPoints(void)
{
	if (m_control_points.empty())
		return;

	std::sort(m_control_points.begin(), m_control_points.end(), ControlPointSorter());
	m_range = (*GetLastValidIterator()).x - (*GetFirstValidIterator()).x;

	if (!m_force_linear && m_interpolation_type == IT_SPLINE)
	{
		// Add all sorted control points to the spline
		DynamicAttributeCurved::ControlPointList::iterator it;
		m_spline.Clear();
		m_spline.SetDynamicPoint(m_control_points);
	}
}
//-----------------------------------------------------------------------
size_t DynamicAttributeCurved::GetNumControlPoints(void) const
{
	return m_control_points.size();
}
//-----------------------------------------------------------------------
void DynamicAttributeCurved::RemoveAllControlPoints(void)
{
	m_control_points.clear();
}
//-----------------------------------------------------------------------
size_t DynamicAttributeCurved::FindNearestControlPointIterator(float x)
{
	size_t size = m_control_points.size();
	size_t index = m_last_index;
	for (; index < size; ++index)
	{
		if (x < m_control_points[index].x)
		{
			if (index == 0)
				return index;
			else
				return (index - 1);
		}
	}

	// If not found return the last valid iterator
	return index - 1;
}
//-----------------------------------------------------------------------
DynamicAttributeCurved::ControlIterotor DynamicAttributeCurved::GetFirstValidIterator(void)
{
	return m_control_points.begin();
}
//-----------------------------------------------------------------------
DynamicAttributeCurved::ControlIterotor DynamicAttributeCurved::GetLastValidIterator(void)
{
	return m_control_points.end() - 1;
}
//-----------------------------------------------------------------------
void DynamicAttributeCurved::CopyAttributesTo(DynamicAttribute* dynamicAttribute)
{
	if (!dynamicAttribute || dynamicAttribute->GetType() != DynamicAttribute::DAT_CURVED)
		return;

	DynamicAttributeCurved* dynAttr = static_cast<DynamicAttributeCurved*>(dynamicAttribute);
	dynAttr->m_interpolation_type = m_interpolation_type;
	dynAttr->m_spline = m_spline;
	dynAttr->m_range = m_range;

	size_t size = m_control_points.size();
	dynAttr->m_control_points.reserve(size);
	dynAttr->m_control_points.resize(size);

	for (size_t index = 0; index < size; ++index)
	{
		dynAttr->m_control_points[index].x = m_control_points[index].x;
		dynAttr->m_control_points[index].y = m_control_points[index].y;
	}

	dynAttr->ProcessControlPoints();
}


void DynamicAttributeFixed::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	element->SetAttribute("dyn_type", "fixed");
	str = CCParticleHelper::ToString(m_value);
	child = my_document->NewElement("value");
	content = my_document->NewText(str.c_str());
	element->LinkEndChild(child);
	child->LinkEndChild(content);
}

void DynamicAttributeRandom::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	element->SetAttribute("dyn_type", "random");

	str = CCParticleHelper::ToString(m_min);
	child = my_document->NewElement("min");
	content = my_document->NewText(str.c_str());
	element->LinkEndChild(child);
	child->LinkEndChild(content);

	str = CCParticleHelper::ToString(m_max);
	child = my_document->NewElement("max");
	content = my_document->NewText(str.c_str());
	element->LinkEndChild(child);
	child->LinkEndChild(content);
}

void DynamicAttributeCurved::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (m_interpolation_type == IT_LINEAR)
	{
		str = "curve_linear";
	}
	else
	{
		str = "curve_spline";
	}
	element->SetAttribute("dyn_type", str.c_str());

	ControlIterotor it;
	ControlIterotor ie = m_control_points.end();
	for (it = m_control_points.begin(); it != ie; ++it)
	{
		str = CCParticleHelper::ToString(*it);
		child = my_document->NewElement("points");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool DynamicAttributeFixed::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "value") == 0)
	{
		m_value = CCParticleHelper::ParseFloat(value);
	}
	else
	{
		return false;
	}
	return true;
}

bool DynamicAttributeFixed::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "value") == 0)
	{
		str = CCParticleHelper::ToString(m_value);
	}
	else
	{
		return false;
	}
	strncpy(value, str.c_str(), size);
	return true;
}

bool DynamicAttributeRandom::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "min") == 0)
	{
		m_min = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "max") == 0)
	{
		m_max = CCParticleHelper::ParseFloat(value);
	}
	else
	{
		return false;
	}
	return true;
}

bool DynamicAttributeRandom::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "min") == 0)
	{
		str = CCParticleHelper::ToString(m_min);
	}
	else if (strcmp(type, "max") == 0)
	{
		str = CCParticleHelper::ToString(m_max);
	}
	else
	{
		return false;
	}
	strncpy(value, str.c_str(), size);
	return true;
}

// 解决特效不精确问题 add by zhuyi 2014-09-11
float DynamicAttributeCurved::GetMaxValue()
{
	float maxSpeed=0.0f;
	int  size = m_control_points.size();
	for(int i=0;i<size;++i)
	{
		if(m_control_points[i].y>maxSpeed)
		{
			maxSpeed = m_control_points[i].y;
		}
	}
	return maxSpeed;
}

float DynamicAttributeCurved::GetMinValue()
{
	float minSpeed=0.0f;
	int  size = m_control_points.size();
	for(int i=0;i<size;++i)
	{
		if(m_control_points[i].y<minSpeed)
		{
			minSpeed = m_control_points[i].y;
		}
	}
	return minSpeed;
}
//end

bool DynamicAttributeCurved::SetAttribute(const char* type, const char* value)
{
	Vec2 temp (0.0f, 0.0f);
	if (strcmp(type, "points") == 0)
	{
		CCParticleHelper::ParseVec2(value, &temp);
		AddControlPoint(temp);
	}
	else if (strcmp(type, "point_list") == 0)
	{
		m_control_points.clear();
		std::vector<std::string> arr;
		CCParticleHelper::Split(arr, value, "|");
		for (size_t i = 0; i < arr.size(); ++i)
		{
			CCParticleHelper::Trim(arr[i]);
			if (!arr[i].empty())
			{
				CCParticleHelper::ParseVec2(arr[i], &temp);
				AddControlPoint(temp);	
			}
		}
		ProcessControlPoints();
	}
	else
	{
		return false;
	}
	return true;
}

bool DynamicAttributeCurved::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "point_list") == 0)
	{
		size_t len = m_control_points.size();
		for (size_t i = 0; i < len; ++i)
		{
			str += CCParticleHelper::ToString(m_control_points[i]) + "|";
		}
	}
	else
	{
		return false;
	}
	strncpy(value, str.c_str(), size);
	return true;
}


float AttributeCalculate(DynamicAttribute* dyn, float x, float defaultValue)
{
	if (dyn)
	{
		return dyn->GetValue(x);
	}

	return defaultValue;
}

DynamicAttribute* CloneDynamicAttribute(DynamicAttribute* dynamicAttribute)
{
	if (dynamicAttribute)
	{
		switch(dynamicAttribute->GetType())
		{
		case DynamicAttribute::DAT_FIXED:
			{
				DynamicAttributeFixed* dynAttr = new DynamicAttributeFixed();
				dynamicAttribute->CopyAttributesTo(dynAttr);
				return dynAttr;	
			}
			break;

		case DynamicAttribute::DAT_RANDOM:
			{
				DynamicAttributeRandom* dynAttr = new DynamicAttributeRandom();
				dynamicAttribute->CopyAttributesTo(dynAttr);
				return dynAttr;
			}
			break;

		case DynamicAttribute::DAT_CURVED:
			{
				DynamicAttributeCurved* dynAttr = new DynamicAttributeCurved();
				dynamicAttribute->CopyAttributesTo(dynAttr);
				return dynAttr;
			}
			break;
		}
	}

	return 0;
}

DynamicAttribute* CreateDynamicAttribute(const char* type)
{
	DynamicAttribute *dyn = 0;
	if (strcmp(type, "fixed") == 0)
	{
		dyn = new DynamicAttributeFixed();
	}
	else if (strcmp(type, "random") == 0)
	{
		dyn = new DynamicAttributeRandom();
	}
	else if (strcmp(type, "curved") == 0)
	{
		dyn = new DynamicAttributeCurved();
	}

	return dyn;
}

NS_CC_END
