
#include "attribute.h"
#include "dynamicattribute.h"
#include "particlehelper.h"
#include "simplespline.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"

NS_CC_BEGIN

DynamicAttribute* CCParticleAttribute::ParseDynamicAttribute(const char* data)
{
	std::vector<std::string> data_array;
	CCParticleHelper::Split(data_array, data, ":");
	DynamicAttribute* dyn = 0;
	if (data_array.size() == 2)
	{
		if (data_array[0] == "fixed")
		{
			dyn = new DynamicAttributeFixed();
			dyn->SetAttribute("value", data_array[1].c_str());
		}
		else if (data_array[0] == "random")
		{
			dyn = new DynamicAttributeRandom();
			std::vector<std::string> min_max;
			CCParticleHelper::Split(min_max, data_array[1], ",");
			if (min_max.size() == 2)
			{
				dyn->SetAttribute("min", min_max[0].c_str());
				dyn->SetAttribute("max", min_max[1].c_str());
			}
		}
		else if (data_array[0] == "curve_linear")
		{
			dyn = new DynamicAttributeCurved();
			static_cast<DynamicAttributeCurved*>(dyn)->SetInterpolationType(DynamicAttributeCurved::IT_LINEAR);
			dyn->SetAttribute("point_list", data_array[1].c_str());
		}
		else if (data_array[0] == "curve_spline")
		{
			dyn = new DynamicAttributeCurved();
			static_cast<DynamicAttributeCurved*>(dyn)->SetInterpolationType(DynamicAttributeCurved::IT_SPLINE);
			dyn->SetAttribute("point_list", data_array[1].c_str());
		}
	}

	if (dyn == 0)
	{
		dyn = new DynamicAttributeFixed();
	}
	return dyn;
}

void CCParticleAttribute::GetDynAttribute(DynamicAttribute* dyn, std::string& value)
{
	value.clear();
	if (dyn == 0)
	{
		return;
	}

	if (dyn->GetType() == DynamicAttribute::DAT_FIXED)
	{
		value = "fixed:" + CCParticleHelper::ToString(static_cast<DynamicAttributeFixed*>(dyn)->GetValue());
	}
	else if (dyn->GetType() == DynamicAttribute::DAT_RANDOM)
	{
		char buffer[64];
		value = "random:";
		dyn->GetAttribute("min", buffer, 64);
		value += buffer;
		value += ",";
		dyn->GetAttribute("max", buffer, 64);
		value += buffer;
	}
	else if (dyn->GetType() == DynamicAttribute::DAT_CURVED)
	{
		DynamicAttributeCurved* curved = static_cast<DynamicAttributeCurved*>(dyn);
		if (curved->GetInterpolationType() == DynamicAttributeCurved::IT_LINEAR)
		{
			value = "curve_linear:";
		}
		else
			value = "curve_spline:";

		char buffer[4096];
		dyn->GetAttribute("point_list", buffer, 4096);
		value += buffer;
	}
}

NS_CC_END